
#pragma once

#include "main.h"

typedef struct {
    uint32_t wrFlg;         //書き込み済みフラグ 4B
    uint16_t sratrNum;      //起動回数 2B
    uint16_t th_mdl;        //中指の曲げ閾値 2B
    uint16_t th_rng;        //薬指の曲げ閾値 2B
    uint8_t disp;
    uint8_t EMPTY[245];     //空き(256-11)B
} PARAM;

#define def_FLASH_USED        0xBF25A249    //FLASH使用済みの値 
#define EEPROM_SIZE           sizeof(PARAM) //配列のサイズ

#if defined(ARDUINO_NUCLEO_F401RE) || defined(ARDUINO_NUCLEO_F411RE)
#define EEPROM_START_ADDRESS  0x8060000     //FLASHのSECTOR7の開始場所
#define EEPROM_MAX_SIZE       0x20000       //FLASHのSECTOR7のサイズ(= 128*1024)
#define EEPROM_BLOCKNUM       0x0200        //=EEPROM_MAX_SIZE/EEPROM_SIZE

#elif defined(ARDUINO_NUCLEO_L476RG)
#define EEPROM_START_ADDRESS  0x080FF800    //FLASHのPage511の開始場所
#define EEPROM_MAX_SIZE       0x00800       //FLASHのPage511のサイズ(= 2*1024)
#define EEPROM_BLOCKNUM       0x0008        //=EEPROM_MAX_SIZE/EEPROM_SIZE
#endif

//全パラメータ操作 
void PRM_Init(void);
void PRM_print(void);
void PRM_Load(void);
void PRM_Save(void);
void PRM_SetSaveFlg(bool);
void PRM_Reset(void);

//個々のパラメータ操作
void PRM_Wr_Flg(uint32_t);
uint32_t PRM_Rd_Flg(void);
uint16_t PRM_Rd_Startup(void);
void PRM_Wr_Startup(uint16_t);
uint16_t PRM_Rd_Middle(void);
void PRM_Wr_Middle(uint16_t);
uint16_t PRM_Rd_Ring(void);
void PRM_Wr_Ring(uint16_t);
uint8_t PRM_Rd_Disp(void);
void PRM_Wr_Disp(uint8_t);
