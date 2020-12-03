//
// main.cpp  : ソレコンじゃんけん出力側 メインプログラム
//--------------------------------------------------------------------
// Version    | Date       | Auther | Details
//--------------------------------------------------------------------
// Ver. 00.01 | 2020/11/24 | Oshiba | test version
//--------------------------------------------------------------------
//
// (c)Team Shinkai Lab

#include "janken_proc.h"
#include "drv_flashSTM32duino.h"

/* @brief じゃんけんの形を判断する
 * @return JANKEN_POSE じゃんけんの形
 */
uint8_t JKN_PoseJudge(void)
{
    bool extend_rng=true, extend_mdl=true;

    //AD値は大きいと指が開いていて,小さいと閉じているハズ
    if (appData.adRawVal[def_POS_MDL] <= PRM_Rd_Middle()) 
        extend_mdl = false;
    if (appData.adRawVal[def_POS_RNG] <= PRM_Rd_Middle()) 
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