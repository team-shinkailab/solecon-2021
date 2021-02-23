//
// main.cpp  : ソレコンじゃんけん出力側 メインプログラム
//
// (c)Team Shinkai Lab

#include "janken_proc.h"
#include "drv_flashSTM32duino.h"

static bool judge = false;

/* @brief じゃんけんの形識別を許可
 */
void JKN_EnableIdentify(void)
{
    judge = true;
}

/* @brief じゃんけんの形識別を停止
 */
void JKN_DisableIdentify(void)
{
    judge = false;
}

/* @brief じゃんけんの形を識別する
 * @return JANKEN_POSE じゃんけんの形
 */
JANKEN_POSE JKN_PoseIdentify(void)
{
    bool extend_rng=true, extend_mdl=true;

    if (judge) {
        //AD値は大きいと指が閉じていて,小さいと開いているハズ
        if (appData.adRawVal[def_POS_MDL] >= PRM_Rd_Middle()) 
            extend_mdl = false;
        if (appData.adRawVal[def_POS_RNG] >= PRM_Rd_Ring()) 
            extend_rng = false;
        
        if (!extend_mdl && !extend_rng)
            return POSE_RCK;
        else if (extend_mdl && !extend_rng)
            return POSE_SSR;
        else if (extend_mdl && extend_rng)
            return POSE_PPR;
        else
            return POSE_INVALID;
    }

    return POSE_NONE;
}

/* @brief AD値の平均、最大値、最小値を調査する
 */
void JKN_MeasurementAdval(void)
{
    uint8_t i;
    appData.avgRecNum++;
    for (i=0; i<2; i++) {
        appData.adSumVal[i] += appData.adRawVal[i];         //合計
        if (appData.adMaxVal[i] < appData.adRawVal[i]) {    //最大値
            appData.adMaxVal[i] = appData.adRawVal[i];
        }
        if (appData.adMinVal[i] > appData.adRawVal[i]) {    //最小値
            appData.adMinVal[i] = appData.adRawVal[i];
        }
    }

    if (appData.avgRecNum >= PRM_Rd_AvgNum()) {
        Serial.println("--- Average --- ");
        Serial.print("Val1 [AD Val]: ");
        Serial.println(appData.adSumVal[0] / appData.avgRecNum);
        Serial.print("Val2 [AD Val]: ");
        Serial.println(appData.adSumVal[1] / appData.avgRecNum);
        Serial.println();
        Serial.println("--- Maximum --- ");
        Serial.print("Val1 [AD Val]: ");
        Serial.println(appData.adMaxVal[0]);
        Serial.print("Val2 [AD Val]: ");
        Serial.println(appData.adMaxVal[1]);
        Serial.println();
        Serial.println("--- Minimum --- ");
        Serial.print("Val1 [AD Val]: ");
        Serial.println(appData.adMinVal[0]);
        Serial.print("Val2 [AD Val]: ");
        Serial.println(appData.adMinVal[1]);
        Serial.println();
        Serial.println("Exit the average measurement mode.");
        delay(5000);
        appData.op_mode = MODE::MODE_NORMAL;
        NVIC_SystemReset();
    }
}