//
//  common_func.c : common surpport functions for CC1310
//
//  Ver.0.00    2015/4/7
//  Ver.0.01	2017/8/22	CC1310�pdelay�֐���ǉ�
//

#include "common_defs.h"


//1�o�C�g�̐��l��HEX2���ɕϊ����� (HEX�͑啶���ŏo��)
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


//unsigned long�̐��l��HEX�ɕϊ�����
void ultoh_org(uint8_t *hex,uint32_t dt)
{
	LONGCONV lc;

	lc.lw = dt;
	itoh_org(&hex[0],lc.b.hh);
	itoh_org(&hex[2],lc.b.hl);
	itoh_org(&hex[4],lc.b.lh);
	itoh_org(&hex[6],lc.b.ll);
}


//HEX2����1�o�C�g�̐��l�ɕϊ����� (HEX�͑啶���̂ݑΉ�)
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


//��������HEX��unsigned long�ɕϊ�����
uint32_t htoul_org(uint8_t *hex,uint8_t wid)
{
	return 0;
}


//10�i��3����1�o�C�g�̐��l�ɕϊ�����
uint8_t atoi_org(uint8_t *num)
{
    uint16_t dt;

    dt  = (num[0]-'0')*100;
    dt += (num[1]-'0')*10;
    dt += (num[2]-'0');

    if (dt >= 255){         //�I�[�o�[�t���[����
        dt = 255;
    }
    return dt&0x00ff;
}


uint8_t atoi3_org(uint8_t *num)
{
    return atoi_org(num);
}


//1�o�C�g�̐��l��10�i3���ɕϊ�����
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


//2�o�C�g�̐��l��10�i5���ɕϊ�����
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