//
//  common_func.c : common surpport functions for CC1310
//
//  Ver.0.00    2015/4/7
//  Ver.0.01	2017/8/22	CC1310用delay関数を追加
//

#include "common_defs.h"


//1バイトの数値をHEX2桁に変換する (HEXは大文字で出力)
void itoh_org(uint8_t *hex,uint8_t dt)
{
    uint8_t wk;

    wk = (dt>>4) & 0x0f;
    if (wk >= 10){
        hex[0] = wk - 10 + 'A';
    } else {
        hex[0] = wk + '0';
    }

    wk = dt & 0x0f;
    if (wk >= 10){
        hex[1] = wk - 10 + 'A';
    } else {
        hex[1] = wk + '0';
    }
}


//unsigned longの数値をHEXに変換する
void ultoh_org(uint8_t *hex,uint32_t dt)
{
	LONGCONV lc;

	lc.lw = dt;
	itoh_org(&hex[0],lc.b.hh);
	itoh_org(&hex[2],lc.b.hl);
	itoh_org(&hex[4],lc.b.lh);
	itoh_org(&hex[6],lc.b.ll);
}


//HEX2桁を1バイトの数値に変換する (HEXは大文字のみ対応)
uint8_t htoi_org(uint8_t *hex)
{
    uint8_t wk;

    if (hex[0] >= 'A'){
        wk = hex[0] - 'A'+10;
    } else {
        wk = hex[0] - '0';
    }
    wk <<= 4;

    if (hex[1] >= 'A'){
        wk |= hex[1] - 'A' + 10;
    } else {
        wk |= hex[1] - '0';
    }

    return wk;
}


//複数桁のHEXをunsigned longに変換する
uint32_t htoul_org(uint8_t *hex,uint8_t wid)
{
	return 0;
}


//10進数3桁を1バイトの数値に変換する
uint8_t atoi_org(uint8_t *num)
{
    uint16_t dt;

    dt  = (num[0]-'0')*100;
    dt += (num[1]-'0')*10;
    dt += (num[2]-'0');

    if (dt >= 255){         //オーバーフロー処理
        dt = 255;
    }
    return dt&0x00ff;
}


uint8_t atoi3_org(uint8_t *num)
{
    return atoi_org(num);
}


//1バイトの数値を10進3桁に変換する
void itoa_org(uint8_t *dec,uint8_t dt)
{
    uint8_t wk;

    wk = dt/100;
    dec[0] = wk+'0';

    wk = dt%100;
    wk = wk/10;
    dec[1] = wk+'0';

    wk = dt%10;
    dec[2] = wk+'0';
}


void itoa3_org(uint8_t *dec,uint8_t dt)
{
    itoa_org(dec,dt);
}


//2バイトの数値を10進5桁に変換する
void ustoa_org(uint8_t *dec,uint16_t dt)
{
    uint16_t wk;

    wk = dt/10000;
    dec[0] = wk+'0';

    wk = dt%10000;
    wk = wk/1000;
    dec[1] = wk+'0';

    wk = dt%1000;
    wk = wk/100;
    dec[2] = wk+'0';

    wk = dt%100;
    wk = wk/10;
    dec[3] = wk+'0';

    wk = dt%10;
    dec[4] = wk+'0';
}