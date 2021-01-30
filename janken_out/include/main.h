
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
#define WIN_LED_PIN     PA13    // 勝ちLED
#define LOSE_LED_PIN    PA15    // 負けLED
#define USER_BUTTON_PIN PC13    // B1 USER
#define START_BUTTON_PIN    PC3     // スタートボタン
#define THUMB_FINGER_PIN    PB14    // 親指
#define INDEX_FINGER_PIN    PB15    // 人差し指
#define MIDDLE_FINGER_PIN   PB15    // 中指
#define RING_FINGER_PIN     PB1     // 薬指
#define LITTLE_FINGER_PIN   PB1     // 小指

//software
#define def_RNG_USBRX   0x0200  //[Byte]
#define def_RNG_IMRX    0x0200  //[Byte]
#define def_TXNODE      "0002"  //子機のノード番号
#define def_INIT_MDL    36000   //中指AD値の閾値テキトーな初期値(=2.36V=36000/65536*4.3) これ以上なら曲がってる
#define def_INIT_RNG    36000   //薬指AD値の閾値テキトーな初期値(=2.36V=36000/65536*4.3)
#define def_POS_MDL     0
#define def_POS_RNG     1

enum JANKEN_POSE {
    POSE_VALID_BEGIN,
    POSE_RCK = POSE_VALID_BEGIN,    // グー
    POSE_SSR,                       // チョキ
    POSE_PPR,                       // パー
    POSE_VALID_END = POSE_PPR,

    POSE_INVALID,   // 不正値
    POSE_NONE,      // なし
};

enum JANKEN_JUDGE {
    JUDGE_WIN,
    JUDGE_LOSE,
    JUDGE_DRAW,
};

enum MODE {
    MODE_NORMAL,
    MODE_SETIM,
    MODE_AVG,
    MODE_PRODUCTION,
    MODE_OUTPUTTEST,
    MODE_END,
};

typedef struct {
    //IM920用
    uint8_t imRxBuf[def_RNG_IMRX];
    uint16_t imRxHead;   uint16_t imRxTail;
    bool imRxEmpty;      bool imRxFull;

    //USB用
    uint8_t usbRxBuf[def_RNG_USBRX];
    uint16_t usbRxHead;   uint16_t usbRxTail;
    bool usbRxEmpty;      bool usbRxFull;

    //平均値計測用
    uint8_t startAvgMode;
    uint16_t avgRecNum;
    uint64_t adSumVal[2];
    uint64_t adMaxVal[2];
    uint64_t adMinVal[2];

    //その他
    bool advalExist;
    uint16_t adRawVal[2];
    MODE op_mode;
    uint8_t im920RxDisp;
} APP_DATA;

extern APP_DATA appData;
extern SoftwareSerial im920;

void CHANGE_MODE(MODE mode);
