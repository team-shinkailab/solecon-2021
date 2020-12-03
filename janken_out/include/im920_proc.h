
#pragma once

#include "main.h"

void IM920_Init(void);

void IM920_RxParse(uint8_t);
void IM920_RxDataParse(uint8_t);
void IM920_ADParse(uint8_t);

uint8_t IM920_ReadByte(void);
uint8_t IM920_Get_Busy(void);
uint8_t IM920_Reboot(void);
uint8_t IM920_ReadVer(void);
uint8_t IM920_ReadParam(void);