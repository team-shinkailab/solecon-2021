//
// drv_flashSTM32duino.cpp : STM32duino用 EEPROM動作
// reference : https://github.com/stm32duino/Arduino_Core_STM32/tree/master/libraries/EEPROM
// reference : https://umtkm.github.io/2017/12/19/nucleo-f401-flash-struct/
// reference : https://www.st.com/resource/ja/reference_manual/dm00096844-stm32f401xbc-and-stm32f401xde-advanced-armbased-32bit-mcus-stmicroelectronics.pdf
//--------------------------------------------------------------------
// Version    | Date       | Auther | Details
//--------------------------------------------------------------------
// Ver. 00.01 | 2020/11/24 | Oshiba | test version
//--------------------------------------------------------------------
//
// (c)Team Shinkai Lab

#include "drv_flashSTM32duino.h"


//内部変数
static PARAM prm;
static bool saveFlg;      //パラメータ変更時にFLASHへ書き込むかどうか
static uint16_t blckNum;  //セクタ内の使用場所の確認

//内部関数
static void flash_erase(void);
static void flash_write(uint8_t*, uint32_t);
static void flash_load(uint8_t*, uint32_t);

/* @brief パラメータバッファ初期化
 * @memo 基本的には書き換えない
*/
void PRM_Init(void)
{
  saveFlg = true;
  
  if (EEPROM_SIZE != 256) {
    while (true) {                    //エラー
      Serial.println("FLASH Error");
      delay(1000);
    }
  }
  if (EEPROM_SIZE > EEPROM_MAX_SIZE) {  //バッファサイズがEEPROMサイズより大きい場合は
    while (true) {                        //エラー
      Serial.println("FLASH Error");
      delay(1000);
    }
  }

  PRM_Load();
  if (PRM_Rd_Flg() != def_FLASH_USED) {    //初回起動
    //Serial.flush();                       //送信完了待ち
    PRM_Reset();
    Serial.println("First Stratup");
  } else {
#if DBG_BLCK_PIRNT == 1
    PRM_Wr_Startup(PRM_Rd_Startup()+1);
#endif
  }
  Serial.println("--- Parameter ---");
  PRM_print();
  Serial.println("-----------------");
}

/* @brief 全パラメータの表示
*/
void PRM_print(void)
{
#if DBG_BLCK_PIRNT == 1
  Serial.print("StartupNum : ");
  Serial.println(PRM_Rd_Startup());
#endif
  Serial.print("Mdl Finger Threshold[AD val] : ");
  Serial.println(PRM_Rd_Middle());
  Serial.print("Rng Finger Threshold[AD val] : ");
  Serial.println(PRM_Rd_Ring());
  if (PRM_Rd_Disp())
    Serial.println("ENDSP");
  else
    Serial.println("DSDSP");
}

/* @brief パラメータの初期化
*/
void PRM_Reset(void)
{
  PRM_SetSaveFlg(false);

  PRM_Wr_Flg(def_FLASH_USED);
  PRM_Wr_Startup(1);
  PRM_Wr_Middle(def_INIT_MDL);
  PRM_Wr_Ring(def_INIT_RNG);
  PRM_Wr_Disp(1);
  PRM_SetSaveFlg(true);
  PRM_Save();
}

/* @brief 全パラメータの書込
*/
void PRM_Save(void)
{
  flash_write((uint8_t*)&prm, EEPROM_SIZE);
}

/* @brief セーブ可否を設定
*/
void PRM_SetSaveFlg(bool flg)
{
  saveFlg = flg;
}

/* @brief フラグを読込
 * @return flg フラグ
*/
uint32_t PRM_Rd_Flg(void)
{
  return prm.wrFlg;
}

/* @brief フラグを書き込み
 * @param flg フラグ
*/
void PRM_Wr_Flg(uint32_t flg)
{
  prm.wrFlg = flg;
  flash_write((uint8_t*)&prm, EEPROM_SIZE);
}

/* @brief 起動回数
 * @return num 起動回数
*/
uint16_t PRM_Rd_Startup(void)
{
  return prm.sratrNum;
}

/* @brief 起動回数を書き込み
 * @param num 起動回数
*/
void PRM_Wr_Startup(uint16_t num)
{
  prm.sratrNum  = num;
  flash_write((uint8_t*)&prm, EEPROM_SIZE);
}

/* @brief 中指の曲げ閾値を読み込む
 * @return th_mdl 中指の閾値
*/
uint16_t PRM_Rd_Middle(void)
{
  return prm.th_mdl;
}

/* @brief 中指の曲げ閾値を書き込み
 * @param th_mdl 中指の閾値
*/
void PRM_Wr_Middle(uint16_t thrhld)
{
  prm.th_mdl  = thrhld;
  flash_write((uint8_t*)&prm, EEPROM_SIZE);
}

/* @brief 薬指の曲げ閾値を書き込み
 * @return th_rng 薬指の閾値
*/
uint16_t PRM_Rd_Ring(void)
{
  return prm.th_rng;
}

/* @brief 薬指の曲げ閾値を読み込む
 * @param th_rng 薬指の閾値
*/
void PRM_Wr_Ring(uint16_t thrhld)
{
  prm.th_rng  = thrhld;
  flash_write((uint8_t*)&prm, EEPROM_SIZE);
}

/* @brief 画面表示有無を書き込み
 * @return disp 画面表示有無
*/
uint8_t PRM_Rd_Disp(void)
{
  return prm.disp;
}

/* @brief 画面表示有無を読み込む
 * @param disp 画面表示有無
*/
void PRM_Wr_Disp(uint8_t disp)
{
  prm.disp  = disp;
  flash_write((uint8_t*)&prm, EEPROM_SIZE);
}

/* @brief 全パラメータの読込
 * @memo 基本的には書き換えない
*/
void PRM_Load(void)
{
  blckNum = EEPROM_BLOCKNUM;
  prm.wrFlg = 0x00;
  while (PRM_Rd_Flg() != def_FLASH_USED) { //後ろから読み込む
    if (blckNum)  blckNum--;              
    else          break;                    //全て確認したら終了
    flash_load((uint8_t*)&prm, EEPROM_SIZE);
  }
#if DBG_BLCK_PIRNT == 1
  Serial.print("Read Page : ");
  Serial.println(blckNum);
#endif
  if (PRM_Rd_Flg() == def_FLASH_USED) {   //一致した場合のみ
                                          //(初回起動以外)
    blckNum++;                              //次回書込ブロックを更新する
    if (blckNum == EEPROM_BLOCKNUM) 
      blckNum = 0;
  }
}

//***************************************
// 低レイヤ
//***************************************
/* @brief FLASHメモリの削除
*/
static void flash_erase(void)
{
  FLASH_EraseInitTypeDef erase;

#if defined(ARDUINO_NUCLEO_F401RE) || defined(ARDUINO_NUCLEO_F411RE) 
  erase.TypeErase = FLASH_TYPEERASE_SECTORS;  //セクタを全消去
  erase.Sector = FLASH_SECTOR_7;              //セクタ7を指定
  erase.NbSectors = 1;                        //消すセクタの数．今回は1つだけ
  erase.VoltageRange = FLASH_VOLTAGE_RANGE_3; //3.3Vで駆動するならこれで
#elif defined(ARDUINO_NUCLEO_L476RG)
  erase.TypeErase = FLASH_TYPEERASE_PAGES;    //ページを全消去
  erase.Banks = FLASH_BANK_2;                 //Bank2の
  erase.Page = 255;                           //255ページ目(ページ254)
  erase.NbPages = 1;                          //のみを削除
#endif
  uint32_t pageError = 0;
  HAL_FLASHEx_Erase(&erase, &pageError);      // HAL_FLASHExの関数で消去
}

/* @brief FLASHメモリへ書込
*/
static void flash_write(uint8_t *data, uint32_t len)
{
#if defined(ARDUINO_NUCLEO_L476RG)
  uint64_t wk, i;
#endif
  uint32_t address = EEPROM_START_ADDRESS;
  address += EEPROM_SIZE * blckNum;
  if (saveFlg) {              //保存可状態なら
    HAL_FLASH_Unlock();         //フラッシュをアンロック
    if (!blckNum) {               //0に書き込むときのみ
      Serial.println("Deleate");
      Serial.flush();
      flash_erase();              //セクタを消去
    }
    do {                        //1Byteずつフラッシュに書き込む         
#if defined(ARDUINO_NUCLEO_F401RE) || defined(ARDUINO_NUCLEO_F411RE)       
      HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, address, *data);
    } while (++address, ++data, --len);
#elif defined(ARDUINO_NUCLEO_L476RG)
      //8Byteまとめて保存する
      //ただし、逆順に保存する
      wk = 0;               
      data += 7;
      for (i=0; i<8; i++) {
        wk <<= 8;
        wk |= *data;
        data--;
        address++;
      }
      len -= 7;
      data += 9;
      HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, address-8, wk);
    } while (--len);
#endif
    HAL_FLASH_Lock();           //フラッシュをロック
#if DBG_BLCK_PIRNT == 1
    Serial.print("Write Page: ");
    Serial.println(blckNum);
#endif
    blckNum++;                  //次に書き込むブロックを更新する
    if (blckNum >= EEPROM_BLOCKNUM) {
      blckNum = 0;
    }
#if DBG_BLCK_PIRNT == 1
    Serial.print("Next Page : ");
    Serial.println(blckNum);
#endif
  }
}

/* @brief FLASHメモリから読み込み
*/
static void flash_load(uint8_t *data, uint32_t len)
{
  uint32_t address = EEPROM_START_ADDRESS;
  address += EEPROM_SIZE * blckNum;
  memcpy(data, (uint8_t*)address, len);
}
