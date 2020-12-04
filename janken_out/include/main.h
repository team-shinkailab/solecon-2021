
#pragma once

#include <Arduino.h>
#include <SoftwareSerial.h>

#include "common_defs.h"

//debug
#define DBG_BLCK_PIRNT  0       //通常は0

//hardware
#define IM920_BUSYPIN   D10
#define IM920_TXPIN     D8
#define IM920_RXPIN     D9
#define def_USB_BPS     115200  //[bps]
#define def_IM920_BPS   19200   //[bps]
#define def_IMRX_INTVL  1000    //[Hz]

//software
#define def_RNG_USBRX   0x0200  //[Byte]
#define def_RNG_IMRX    0x0200  //[Byte]
#define def_TXNODE      "0002"  //子機のノード番号
#define def_INIT_MDL    36000   //中指AD値の閾値テキトーな初期値(=2.36V=36000/65536*4.3) これ以上なら曲がってる
#define def_INIT_RNG    36000   //薬指AD値の閾値テキトーな初期値(=2.36V=36000/65536*4.3)
#define def_POS_MDL     0
#define def_POS_RNG     1

typedef enum {
    POSE_NONE, POSE_RCK, POSE_SSR, POSE_PPR
} JANKEN_POSE;

typedef struct {
    //IM920用
    uint8_t imRxBuf[def_RNG_IMRX];
    uint16_t imRxHead;   uint16_t imRxTail;
    bool imRxEmpty;      bool imRxFull;

    //USB用
    uint8_t usbRxBuf[def_RNG_USBRX];
    uint16_t usbRxHead;   uint16_t usbRxTail;
    bool usbRxEmpty;      bool usbRxFull;

    //その他
    bool advalExist;
    uint16_t adRawVal[2];
    uint8_t op_mode;
    uint8_t im920RxDisp;
} APP_DATA;

extern APP_DATA appData;
extern SoftwareSerial im920;