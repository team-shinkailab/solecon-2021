//
// drv_usbSTM32duino.cpp : STM32duino用 USBシリアルの処理
//--------------------------------------------------------------------
// Version    | Date       | Auther | Detail
//--------------------------------------------------------------------
// Ver. 00.01 | 2020/11/24 | Oshiba | test version
//--------------------------------------------------------------------
//
// (c) Team Shinkai Lab

#include "drv_usbSTM32duino.h"

/* @brief USB初期化
*/
void USB_Init(void)
{
  appData.usbRxHead = 0;      appData.usbRxTail = 0;
  appData.usbRxEmpty = true;  appData.usbRxFull = false;
  Serial.begin(def_USB_BPS);
}

/* @brief USB受信割込関数 (arduinoの関数)
*/
void serialEvent(void)
{
  volatile uint8_t wk;
  while (Serial.available()) {
    wk = Serial.read();
    if (!appData.usbRxFull) {
      appData.usbRxBuf[appData.usbRxTail++] = wk;
      appData.usbRxTail &= (def_RNG_USBRX - 1);
      appData.usbRxEmpty = false;
      if (appData.usbRxHead == appData.usbRxTail)
        appData.usbRxFull = true;
    }
  }
}

/* @brief 受信文字1Byte掃出し
   @param 受信文字列
*/
uint8_t USB_RX_pop(void)
{
  uint8_t wk = 0;
  if (!appData.usbRxEmpty) {
    wk = appData.usbRxBuf[appData.usbRxHead++];
    appData.usbRxHead &= (def_RNG_USBRX - 1);
    appData.usbRxFull = false;
    if (appData.usbRxHead == appData.usbRxTail)
      appData.usbRxEmpty = true;
  }
  return wk;
}
