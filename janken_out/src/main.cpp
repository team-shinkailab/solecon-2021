//
// main.cpp  : ソレコンじゃんけん出力側 メインプログラム
//--------------------------------------------------------------------
// Version    | Date       | Auther | Details
//--------------------------------------------------------------------
// Ver. 00.01 | 2020/11/24 | Oshiba | test version (L476RG)
// Ver. 00.02 | 2020/12/04 | Oshiba | F401REボードへ対応(L476RGは非対応)
// Ver. 00.03 | 2020/12/11 | Oshiba | テストモード追加
//--------------------------------------------------------------------
//
// (c)Team Shinkai Lab

#include "main.h"
#include "drv_usbSTM32duino.h"
#include "drv_flashSTM32duino.h"
#include "im920_proc.h"
#include "usbrx_proc.h"
#include "janken_proc.h"
#include "SoleHand.h"
#include <random>
#include <array>

#define DIGITAL_FLIP(PIN) digitalWrite((PIN), !digitalRead((PIN)))

APP_DATA appData;
SoftwareSerial im920(IM920_TXPIN, IM920_RXPIN);

SoleHand soleHand(THUMB_FINGER_PIN, INDEX_FINGER_PIN, MIDDLE_FINGER_PIN, RING_FINGER_PIN, LITTLE_FINGER_PIN);

std::random_device seed_gen;
std::mt19937 engine(seed_gen());
std::uniform_int_distribution<int> uniform(0, JANKEN_POSE::POSE_INVALID);

JANKEN_JUDGE playJanken();

void SW_Init()
{
  appData.advalExist = false;
  appData.op_mode = MODE::MODE_PRODUCTION;
  appData.im920RxDisp = PRM_Rd_Disp();
}

void GPIO_INIT()
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(WIN_LED_PIN, OUTPUT);
  pinMode(LOSE_LED_PIN, OUTPUT);
  pinMode(THUMB_FINGER_PIN, OUTPUT);
  pinMode(INDEX_FINGER_PIN, OUTPUT);
  pinMode(MIDDLE_FINGER_PIN, OUTPUT);
  pinMode(RING_FINGER_PIN, OUTPUT);
  pinMode(LITTLE_FINGER_PIN, OUTPUT);
}

void setup() 
{
  USB_Init();
  IM920_Init();
  USBRX_Init();
  PRM_Init();
  SW_Init();
  GPIO_INIT();
}

const char* poseToString(JANKEN_POSE pose){
  return
    pose == JANKEN_POSE::POSE_RCK ? "Gu-" :
    pose == JANKEN_POSE::POSE_SSR ? "Choki" :
    pose == JANKEN_POSE::POSE_PPR ? "Pa-" : "Invalid";
}

uint8_t wk;
void loop() 
{
  if (!appData.usbRxEmpty)              //パラメータ等書き換え用
    USBRX_dataParse();

//***********************************************
// 通常モード
//***********************************************
  if (appData.op_mode == MODE::MODE_NORMAL) {
    if (!appData.imRxEmpty)               //受信データ解析+画面表示
      IM920_RxParse(appData.im920RxDisp);
        
    if (appData.advalExist) {
      appData.advalExist = false;
      wk = JKN_PoseIdentify();               
      if (wk != POSE_NONE){
        Serial.println(poseToString(static_cast<JANKEN_POSE>(wk)));
      }
    }

// Ver. 00.03・・・テストモード追加
//***********************************************
// 指定回数受信を待って平均,最小値,最大値出すモード
// 出力後はappData.op_mode=0に戻る
//***********************************************
  } else if (appData.op_mode == MODE::MODE_AVG) {
    if (!appData.imRxEmpty)               //受信データ解析+画面表示
      IM920_RxParse(0);
    
    if (appData.startAvgMode) {           //エンターでスタート
      if (appData.advalExist) {
        appData.advalExist = false;
        JKN_MeasurementAdval();             //ここで計測
      }
    } else {                              //エンターあるまではここで待つ
      appData.advalExist = false;           //これいらんかも
      if (!appData.usbRxEmpty)              
        USBRX_dataParse();                    //\r\n待ち
    }

//***********************************************
// IM920s設定モード(無線モジュールと直接通信する)
// 解除はPORのみ, 本番用では削除可能
//***********************************************
  } else if (appData.op_mode == MODE::MODE_SETIM) {
    if (!appData.usbRxEmpty) {
      wk = USB_RX_pop();
      im920.printf("%c", wk);
      Serial.printf("%c", wk);
    }
    if (!appData.imRxEmpty)               //受信データ画面表示
      Serial.printf("%c", IM920_ReadByte());

//***********************************************
// 本番モード
//***********************************************
  } else if (appData.op_mode == MODE::MODE_PRODUCTION) {
    JKN_EnableIdentify();

    if (!appData.usbRxEmpty)              //パラメータ等書き換え用
      USBRX_dataParse();
    
    // スタートボタン待ち
    if(digitalRead(START_BUTTON_PIN) == LOW) {
        // じゃんけん開始
        while(playJanken() == JANKEN_JUDGE::JUDGE_DRAW) {
            // あいこ
            delay(1000);
        }
    }
    DIGITAL_FLIP(LED_BUILTIN);
    delay(100);
  }
}

/**
* @brief 初期化
*/
void initializeHand() {
    soleHand.initialize();
    digitalWrite(WIN_LED_PIN, LOW);
    digitalWrite(LOSE_LED_PIN, LOW);
}

/**
* @brief 判定
* @return Judge aから見た勝敗
*/
JANKEN_JUDGE judge(JANKEN_POSE a, JANKEN_POSE b) {    
    switch(a) {
    case JANKEN_POSE::POSE_RCK:
        return
            b == JANKEN_POSE::POSE_SSR ? JANKEN_JUDGE::JUDGE_WIN :
            b == JANKEN_POSE::POSE_PPR ? JANKEN_JUDGE::JUDGE_LOSE : JANKEN_JUDGE::JUDGE_DRAW;
    case JANKEN_POSE::POSE_SSR:
        return
            b == JANKEN_POSE::POSE_PPR ? JANKEN_JUDGE::JUDGE_WIN :
            b == JANKEN_POSE::POSE_RCK ? JANKEN_JUDGE::JUDGE_LOSE : JANKEN_JUDGE::JUDGE_DRAW;
    case JANKEN_POSE::POSE_PPR:
        return
            b == JANKEN_POSE::POSE_RCK ? JANKEN_JUDGE::JUDGE_WIN :
            b == JANKEN_POSE::POSE_SSR ? JANKEN_JUDGE::JUDGE_LOSE : JANKEN_JUDGE::JUDGE_DRAW;
    default:
        return JANKEN_JUDGE::JUDGE_DRAW;
    }
}

/**
 * @brief プレイヤーの手待ち受け
 */
JANKEN_POSE waitHand() {
  while(true) {
    if (!appData.imRxEmpty)               //受信データ解析+画面表示
      IM920_RxParse(appData.im920RxDisp);
    
    if(appData.advalExist) {
      appData.advalExist = false;

      JANKEN_POSE pose =  JKN_PoseIdentify();
      if(pose != JANKEN_POSE::POSE_NONE){
        return pose;
      }
    }
    
    delay(10);
  }

  return JANKEN_POSE::POSE_NONE;
}

/**
* @brief じゃんけん実行
* @return Judge プレイヤーから見た勝敗
*/
JANKEN_JUDGE playJanken() {
    Serial.println("play janken");
    
    initializeHand();
    
    // プレイヤーのグー待ち
    while(waitHand() != JANKEN_POSE::POSE_RCK){
      ;
    }

    Serial.println("Gu detected");
    
    // じゃん
    digitalWrite(WIN_LED_PIN, HIGH);
    digitalWrite(LOSE_LED_PIN, HIGH);
    delay(500);
    digitalWrite(WIN_LED_PIN, LOW);
    digitalWrite(LOSE_LED_PIN, LOW);
    delay(500);
    
    // けん
    digitalWrite(WIN_LED_PIN, HIGH);
    digitalWrite(LOSE_LED_PIN, HIGH);
    delay(500);
    digitalWrite(WIN_LED_PIN, LOW);
    digitalWrite(LOSE_LED_PIN, LOW);
    delay(500);
    
    // ぽん
    digitalWrite(WIN_LED_PIN, HIGH);
    digitalWrite(LOSE_LED_PIN, HIGH);
    // CPUの手決定
    JANKEN_POSE cpuPose = static_cast<JANKEN_POSE>(uniform(engine));
    soleHand.setPose(cpuPose);
    // プレイヤーの手決定
    JANKEN_POSE playerPose = waitHand();
    
    // 結果表示
    JANKEN_JUDGE result = judge(playerPose, cpuPose);

    Serial.printf("player: %s, cpu: %s\n", poseToString(playerPose), poseToString(cpuPose));
    digitalWrite(WIN_LED_PIN, LOW);
    digitalWrite(LOSE_LED_PIN, LOW);
    switch(result) {
    case JANKEN_JUDGE::JUDGE_WIN:
        Serial.println("player win");
        for(int i = 0; i < 20; i++){
            DIGITAL_FLIP(WIN_LED_PIN);
            delay(100);
        }
        break;
    case JANKEN_JUDGE::JUDGE_LOSE:
        Serial.println("player lose");
        for(int i = 0; i < 20; i++){
            DIGITAL_FLIP(LOSE_LED_PIN);
            delay(100);
        }
        break;
    case JANKEN_JUDGE::JUDGE_DRAW:
        Serial.println("draw");
        for(int i = 0; i < 20; i++){
            DIGITAL_FLIP(WIN_LED_PIN);
            DIGITAL_FLIP(LOSE_LED_PIN);
            delay(100);
        }
        break;
    }
    
    return result;
}

/**
 * @brief モード変更
 */
void CHANGE_MODE(MODE mode) {
  appData.op_mode = mode;

  switch(mode) {
  case MODE::MODE_NORMAL:
    Serial.println("Normal Mode");
    break;
  case MODE::MODE_AVG:
    appData.startAvgMode = 0;
    appData.avgRecNum = 0;        
    appData.adSumVal[0] = 0;      appData.adSumVal[1] = 0;
    appData.adMaxVal[0] = 0;      appData.adMaxVal[1] = 0;
    appData.adMinVal[0] = 0xFFFF; appData.adMinVal[1] = 0xFFFF;
    Serial.println("Measuring Average Mode");
    Serial.print("press the enter key.");
    break;
  case MODE::MODE_SETIM:
    Serial.println("IM920 IO Mode");
    break;
  case MODE::MODE_PRODUCTION:
    Serial.println("Production Mode");
    break;
  default:
    break;
  }
}
