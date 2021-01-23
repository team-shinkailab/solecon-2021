
#pragma once

#include "main.h"


#define CMDNUM    0x0C  //コマンドの数
#define CMDLEN    0x05  //コマンドの英語部分の長さ
#define CMDMAXLEN 0x0A  //\r\n以外のコマンド長

typedef struct {      //関数保存用辞書リスト
  char* cmd;
  uint8_t (*func)(void);
} CMD_DICTIONARY;

typedef enum {
  RESP_ERROR, RESP_OK, RESP_NO
} CMD_RESULT;

void USBRX_Init(void);
void USBRX_dataParse(void);
