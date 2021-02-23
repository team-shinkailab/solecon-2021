//
// drv_timerSTM32duino.cpp  : STM32duinoタイマー用
//
// (c)Team Shinkai Lab

#include "drv_timerSTM32duino.h"

/* @brief TIMER1の設定
 * @param func 割り込みをかける関数
 * @param intvl 割り込みの間隔[Hz]
 * @memo im920_procで使用中
 */
void TIM1_Init(callback_function_t func, uint16_t intvl)
{
  TIM_TypeDef *Instance = TIM1;                     //タイマー
  HardwareTimer *MyTim = new HardwareTimer(Instance);
  MyTim->setOverflow(intvl, HERTZ_FORMAT);           //1000 Hz = 1ms
  MyTim->attachInterrupt(func);
  MyTim->resume();
}
/* @brief TIMER2の設定
 * @param func 割り込みをかける関数
 * @param intvl 割り込みの間隔[Hz]
 * @memo 未使用
 */
void TIM2_Init(callback_function_t func, uint16_t intvl)
{
  TIM_TypeDef *Instance = TIM2;                     //タイマー
  HardwareTimer *MyTim = new HardwareTimer(Instance);
  MyTim->setOverflow(intvl, HERTZ_FORMAT);           //1000 Hz = 1ms
  MyTim->attachInterrupt(func);
  MyTim->resume();
}