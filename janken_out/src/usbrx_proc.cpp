//
// usb_proc.cpp : usbによるIM920パラメータ変更モード処理
//--------------------------------------------------------------------
// Version    | Date       | Auther | Details
//--------------------------------------------------------------------
// Ver. 00.01 | 2020/11/24 | Oshiba | test version
//--------------------------------------------------------------------
//
// (c)Team Shinkai Lab

#include "usbrx_proc.h"
#include "drv_usbSTM32duino.h"
#include "drv_flashSTM32duino.h"
#include "janken_proc.h"

//内部関数
static uint8_t scan_cmd(void);
static uint8_t USBRX_SETIM(void);
static uint8_t USBRX_RDALL(void);
static uint8_t USBRX_SWRST(void);
static uint8_t USBRX_PMCLR(void);
static uint8_t USBRX_WRMDL(void);
static uint8_t USBRX_WRRNG(void);
static uint8_t USBRX_ENDSP(void);
static uint8_t USBRX_DSDSP(void);
static uint8_t USBRX_ENJKN(void);

//内部変数
/* @brief コマンド用辞書
 * @memo CMD_DICTIONARYはheaderで定義
 */
static const CMD_DICTIONARY cmdDict[CMDNUM]{
  {(char*)"RDALL", USBRX_RDALL},  {(char*)"SWRST", USBRX_SWRST},  {(char*)"PMCLR", USBRX_PMCLR},
  {(char*)"SETIM", USBRX_SETIM},  {(char*)"WRMDL", USBRX_WRMDL},  {(char*)"WRRNG", USBRX_WRRNG},
  {(char*)"ENDSP", USBRX_ENDSP},  {(char*)"DSDSP", USBRX_DSDSP},  {(char*)"ENJKN", USBRX_ENJKN}
};
static char cmd[20];
static uint8_t cmdLen;
static uint8_t dlmtStateMac;

/* @brief コマンド待ち開始
 */
void USBRX_Init(void)
{
  cmdLen = 0;
  dlmtStateMac = 0;
}

/* @brief コマンドを調べる
 * @return CMDNUM=Error, other=辞書番号
 */
static uint8_t scan_cmd(void)
{
  uint8_t i, j;
  for (i=0; i<CMDNUM; i++) {
    for (j=0; j<CMDLEN; j++) {
      if (cmdDict[i].cmd[j] != cmd[j])
        break;
    }
    if (CMDLEN == j)
      break;
  }
  return i;
}

/* @brief コマンド解析
 * @memo モード選択画面のときまでここに来る
 */
void USBRX_dataParse(void)
{
  char wk;
  uint8_t cmdnum, done;
  bool noErr;
  
  while (!appData.usbRxEmpty) {
    noErr = true;  done = RESP_ERROR;
    wk = USB_RX_pop();              //データを1Byte取り出す
    Serial.print(wk);
    wk = toupper(wk);               //小文字を大文字に
    if ('A'<=wk&&wk<='Z') {         //5文字目までは英語を待つ
      if (cmdLen < CMDLEN)
        cmd[cmdLen++] = wk;
      else
        noErr = false;
    } else if ('-'==wk || '+'==wk) {           //最初は符号でもOK
      if (CMDLEN == cmdLen)
        cmd[cmdLen++] = wk;
      else
        noErr = false;      
    } else if ('0'<=wk&&wk<='9') {  //以降は数値
      if (CMDLEN<=cmdLen && cmdLen<CMDMAXLEN)
        cmd[cmdLen++] = wk;
      else
        noErr = false;
    } else if ('\r' == wk) {        //最後はCR+
      cmd[cmdLen++] = wk;
      dlmtStateMac++;
    } else if ('\n' == wk) {        //LF
      cmd[cmdLen++] = wk;
      if ((1==dlmtStateMac) && (cmdLen>=CMDLEN)) {
        cmdnum = scan_cmd();
        if (cmdnum != CMDNUM) {
          done = cmdDict[cmdnum].func();       //コマンド実行
        }
      }
      noErr = false;
    } else {
      noErr = false;
    }
    if (!noErr) {
      cmdLen = 0;
      dlmtStateMac = 0x00;
      if (done == RESP_ERROR)                 //doneならコマンド成功
        Serial.println("\r\nERROR");
      else if (done == RESP_OK) 
        Serial.println("OK");
      else if (done == RESP_NO) {

      }
    }
  }
}

/* @brief IM920との対話モードへ
 * @memo 終了にはリセットが必要
 */
static uint8_t USBRX_SETIM(void)
{
  Serial.println("IM920 IO Mode");
  appData.op_mode = 1;
  return RESP_OK;
}

/* @brief パラメータを全て読む
 */
static uint8_t USBRX_RDALL(void)
{
  Serial.println("--- Parameter ---");
  PRM_print();
  Serial.println("-----------------");
  return RESP_NO;
}

/* @brief 再起動を行う
 */
static uint8_t USBRX_SWRST(void)
{
  Serial.println("Reboot...");
  Serial.flush();
  NVIC_SystemReset();
  return RESP_OK;  //dummy
}

/* @brief パラメータを全て削除する
 */
static uint8_t USBRX_PMCLR(void)
{
  PRM_Reset();
  return RESP_OK;
}

/* @brief 中指の閾値を書き込む
 */
static uint8_t USBRX_WRMDL(void)
{
  char buf[6];
  uint8_t wk, i, j=0;
  int newval;
  for (i=CMDLEN; i<CMDMAXLEN; i++) {
    wk = cmd[i];
    if ('\r' == wk)
      break;
    else
      buf[j++] = wk;
  }
  newval = (uint16_t) atoi(buf);
  if (newval <= 50296) {
    Serial.println("Save Now...");
    Serial.flush();
    PRM_Wr_Middle(newval);
    return RESP_OK;
  } else {
    return RESP_ERROR;
  }
}

/* @brief 薬指の閾値を書き込む
 */
static uint8_t USBRX_WRRNG(void)
{
  char buf[6];
  uint8_t wk, i, j=0;
  int newval;
  for (i=CMDLEN; i<CMDMAXLEN; i++) {
    wk = cmd[i];
    if ('\r' == wk)
      break;
    else
      buf[j++] = wk;
  }
  newval = (uint16_t) atoi(buf);
  if (newval <= 50296) {
    Serial.println("Save Now...");
    Serial.flush();
    PRM_Wr_Ring(newval);
    return RESP_OK;
  } else {
    return RESP_ERROR;
  }
}

/* @brief 画面表示を有りにする
 */
static uint8_t USBRX_ENDSP(void)
{
  Serial.println("Save Now...");
  Serial.flush();
  PRM_Wr_Disp(1);
  appData.im920RxDisp = 1;
  return RESP_OK;
}

/* @brief 画面表示を無しにする
 */
static uint8_t USBRX_DSDSP(void)
{
  Serial.println("Save Now...");
  Serial.flush();
  PRM_Wr_Disp(0);
  appData.im920RxDisp = 0;
  return RESP_OK;
}

/* @brief じゃんけん判定開始
 * @memo テスト用
 */
static uint8_t USBRX_ENJKN(void)
{
  JKN_EnableIdentify();
  return RESP_OK;
}