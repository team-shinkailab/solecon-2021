//
// drv_timerSTM32duino.cpp  : STM32duinoタイマー用
//--------------------------------------------------------------------
// Version    | Date       | Auther | Details
//--------------------------------------------------------------------
// Ver. 00.01 | 2020/11/24 | Oshiba | test version
//--------------------------------------------------------------------
//
// (c)Team Shinkai Lab

#include "drv_timerSTM32duino.h"

/* @brief TIMER1の設定
 * @param func 割り込みをかける関数
 * @param intvl 割り込みの間隔[Hz]
 */
void TIM1_Init(callback_function_t func, uint16_t intvl)
{
  TIM_TypeDef *Instance = TIM1;                     //タイマー
  HardwareTimer *MyTim = new HardwareTimer(Instance);
  MyTim->setOverflow(intvl, HERTZ_FORMAT);           //1000 Hz = 1ms
  MyTim->attachInterrupt(func);
  MyTim->resume();
}