//
// main.cpp  : ソレコンじゃんけん出力側 メインプログラム
//--------------------------------------------------------------------
// Version    | Date       | Auther | Details
//--------------------------------------------------------------------
// Ver. 00.01 | 2020/11/24 | Oshiba | test version (L476RG)
// Ver. 00.02 | 2020/12/04 | Oshiba | F401RE board is now supported.
//--------------------------------------------------------------------
//
// (c)Team Shinkai Lab

#include "main.h"
#include "drv_usbSTM32duino.h"
#include "drv_flashSTM32duino.h"
#include "im920_proc.h"
#include "usbrx_proc.h"
#include "janken_proc.h"

APP_DATA appData;
SoftwareSerial im920(IM920_TXPIN, IM920_RXPIN);

void SW_Init()
{
  appData.advalExist = false;
  appData.op_mode = 0;
  appData.im920RxDisp = PRM_Rd_Disp();
}

void setup() 
{
  USB_Init();
  IM920_Init();
  USBRX_Init();
  PRM_Init();
  SW_Init();
}

uint8_t wk;
void loop() 
{
  if (appData.op_mode == 0) {
    if (!appData.imRxEmpty)               //受信データ解析+画面表示
      IM920_RxParse(appData.im920RxDisp);
    
    if (!appData.usbRxEmpty)              //パラメータ等書き換え用
      USBRX_dataParse();
    
    if (appData.advalExist) {
      appData.advalExist = false;
      wk = JKN_PoseIdentify();               
      if (wk == POSE_RCK)
        Serial.println("Gu-");
      else if (wk == POSE_SSR)
        Serial.println("Choki");
      else if (wk == POSE_PPR)
        Serial.println("Pa-");
      else if (wk == POSE_NONE)
        Serial.println("Yamero");
      //else if (wk == JUDGE_STOP)
      //  Serial.println("Stop Now");
    }


//***********************************************
// IM920s設定モード(無線モジュールと直接通信する)
// 解除はPORのみ, 本番用では削除可能
//***********************************************
  } else if (appData.op_mode == 1) {
    if (!appData.usbRxEmpty) {
      wk = USB_RX_pop();
      im920.printf("%c", wk);
      Serial.printf("%c", wk);
    }
    if (!appData.imRxEmpty)               //受信データ画面表示
      Serial.printf("%c", IM920_ReadByte());
  }
}