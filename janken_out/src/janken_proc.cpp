//
// main.cpp  : ソレコンじゃんけん出力側 メインプログラム
//--------------------------------------------------------------------
// Version    | Date       | Auther | Details
//--------------------------------------------------------------------
// Ver. 00.01 | 2020/11/24 | Oshiba | test version
// Ver. 00.02 | 2020/12/04 | Oshiba | 識別のストップ/スタートを追加
//--------------------------------------------------------------------
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
uint8_t JKN_PoseIdentify(void)
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
            return POSE_NONE;
    }

    return JUDGE_STOP;
}