//
// main.cpp  : ソレコンじゃんけん出力側 無線受信機プログラム
// reference : シールドについて : https://www.interplan.co.jp/support/solution/IM315/manual/IM315SHLDv2_manual.pdf
//
// (c)Team Shinkai Lab

#include "im920_proc.h"
#include "drv_timerSTM32duino.h"

//内部関数
static void im920_read_intrpt(void);

//ステートマシン(内部変数)
static uint8_t recDataStmc; 
static uint8_t bendStmc;

/* @brief IM920 受信バッファ
 * @memo 今は1msで割り込みかけて受信してるけど、
 *       loopにおいてもいいかも？？
 */
void IM920_Init(void)
{
    //IM920 受信データ処理用
    im920.begin(def_IM920_BPS);                           //ソフトウエアシリアル 初期化
    pinMode(IM920_BUSYPIN, INPUT);                        //Busyピン
    appData.imRxEmpty = true; appData.imRxFull = false;   //受信バッファ
    appData.imRxHead = 0;     appData.imRxTail = 0;
    TIM1_Init(im920_read_intrpt, def_IMRX_INTVL);

    //IM920 受信データ解析用
    recDataStmc = 0xFF;
    bendStmc = 0xFF;
}

//*********************************
// IM920　受信時の処理
//*********************************

/* @brief IM920受信データ解析
 */
void IM920_RxParse(uint8_t disp)
{
    uint8_t wk;
    while (!appData.imRxEmpty) {
        wk = IM920_ReadByte();
        if (disp)
            Serial.printf("%c", wk);

            if (bendStmc != 0xFF)
                IM920_ADParse(wk);
            if (recDataStmc != 0xFF)
                IM920_RxDataParse(wk);
                
            if (recDataStmc == 0xFF) 
                if (wk == '0')
                    recDataStmc = 0x00;

    }
}

/* @brief 無線のデータ受信を待つ
 * @param dt 受信データ 1文字
 * @memo aa,bbbb,dd:を待つ。:以降は曲げセンサのデータ
 */
void IM920_RxDataParse(uint8_t dt)
{
    if (recDataStmc == 0x00) {                  //00固定
        if (dt == '0')  recDataStmc++;
        else            recDataStmc = 0xFF;
    } else if (recDataStmc == 0x01) {           //区切り
        if (dt == ',')  recDataStmc++;
        else            recDataStmc = 0xFF;
    } else if (recDataStmc <= 0x05) {           //ノード番号
        if (dt == def_TXNODE[recDataStmc-0x02])
            recDataStmc++;
        else
            recDataStmc = 0xFF;
    } else if (recDataStmc == 0x06) {           //区切り
        if (dt == ',')  recDataStmc++;
        else            recDataStmc = 0xFF;
    } else if (recDataStmc <= 0x08) {           //RSSI
        if ('0'<=dt&&dt<='9')       recDataStmc++;
        else if ('A'<=dt&&dt<='F')  recDataStmc++;
        else                        recDataStmc = 0xFF;
    } else if (recDataStmc == 0x09) {           //区切り
        if (dt == ':')  bendStmc = 0x00;
        recDataStmc = 0xFF;
    } 
}

/* @brief AD変換値を待つ
 * @param dt 受信データ 1文字
 * @memo ee,ff,gg,hh,を待つ
 */
void IM920_ADParse(uint8_t dt)
{
    static uint8_t buf[2];
    if (bendStmc <= 0x01) {                     //port1 下位
        buf[bendStmc] = dt;
        if ('0'<=dt&&dt<='9')       bendStmc++;
        else if ('A'<=dt&&dt<='F')  bendStmc++;
        else                        bendStmc = 0xFF;
        if (bendStmc == 0x02)
            appData.adRawVal[0] = htoi_org(buf);
    } else if (bendStmc == 0x02) {              //区切り待ち
        if (dt == ',')  bendStmc++;
        else            bendStmc = 0xFF;
    } else if (bendStmc <= 0x04) {              //port1 上位
        buf[bendStmc-0x03] = dt;
        if ('0'<=dt&&dt<='9')       bendStmc++;
        else if ('A'<=dt&&dt<='F')  bendStmc++;
        else                        bendStmc = 0xFF;
        if (bendStmc == 0x05)
            appData.adRawVal[0] += (htoi_org(buf) * 256);
    } else if (bendStmc == 0x05) {
        if (dt == ',')  bendStmc++;
        else            bendStmc = 0xFF;
    } else if (bendStmc <= 0x07) {              //port2 下位
        buf[bendStmc-0x06] = dt;
        if ('0'<=dt&&dt<='9')       bendStmc++;
        else if ('A'<=dt&&dt<='F')  bendStmc++;
        else                        bendStmc = 0xFF;
        if (bendStmc == 0x08)
            appData.adRawVal[1] = htoi_org(buf);
    } else if (bendStmc == 0x08) {
        if (dt == ',')  bendStmc++;
        else            bendStmc = 0xFF;
    } else if (bendStmc <= 0x0A) {              //port2 上位
        buf[bendStmc-0x09] = dt;
        if ('0'<=dt&&dt<='9')       bendStmc++;
        else if ('A'<=dt&&dt<='F')  bendStmc++;
        else                        bendStmc = 0xFF;
        if (bendStmc == 0x0A)
            appData.adRawVal[1] += (htoi_org(buf) * 256);
    } else if (bendStmc == 0x0B) {
        if (dt == '\n') {
        //if (dt == ',') {
            bendStmc = 0xFF;
            appData.advalExist = true;
            if (appData.op_mode == MODE::MODE_NORMAL || appData.op_mode == MODE::MODE_PRODUCTION) {
                Serial.print("Val1 [AD Val]: ");
                Serial.println(appData.adRawVal[0]);
                Serial.print("Val2 [AD Val]: ");
                Serial.println(appData.adRawVal[1]);
            }
        }
    }
}

//*********************************
// IM920操作
//*********************************
/* @brief 受信バッファから1Byte取り出す
 * @return 受信データ1文字
 */
uint8_t IM920_ReadByte(void)
{
    uint8_t wk = 0x00;
    if (!appData.imRxEmpty) {
        wk = appData.imRxBuf[appData.imRxHead++];
        appData.imRxHead &= (def_RNG_IMRX - 1);
        appData.imRxFull = false;
        if (appData.imRxHead == appData.imRxTail)
            appData.imRxEmpty = true;
    }
    return wk;
}

/* @brief BUSYピンを読み込む
 * @return false=送信可, other=送信不可
 */
uint8_t IM920_Get_Busy(void)
{
    return digitalRead(IM920_BUSYPIN);
}

/* @brief IM920のリセット
 * @return true=成功, false=失敗
 */
uint8_t IM920_Reboot(void)
{
    if (!IM920_Get_Busy()) {
        im920.println("SRST");
    } else {
        return false;
    }
    return true;
}

/* @brief IM920のバージョン読み込み
 * @return true=成功, false=失敗
 */
uint8_t IM920_ReadVer(void)
{
    if (!IM920_Get_Busy()) {
        im920.println("RDVR");
    } else {
        return false;
    }
    return true;
}

/* @brief IM920のパラメータ読み込み
 * @return true=成功, false=失敗
 */
uint8_t IM920_ReadParam(void)
{
    if (!IM920_Get_Busy()) {
        im920.println("RPRM");
    } else {
        return false;
    }
    return true;
}

/* @brief 1ms間隔で割り込みをかけて、データを読む
 */
static void im920_read_intrpt(void)
{
    uint8_t wk;
    while (im920.available()) {
        wk = im920.read();
        if ((' '<=wk&&wk<='~') ||           //文字化け対策
            wk=='\r' || wk=='\n') {
            if (!appData.imRxFull) {
                appData.imRxBuf[appData.imRxTail++] = wk;
                appData.imRxTail &= (def_RNG_IMRX - 1);
                appData.imRxEmpty = false;
                if (appData.imRxHead == appData.imRxTail)
                    appData.imRxFull = true;
            }
        }
    }
}