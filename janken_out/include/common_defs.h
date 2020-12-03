/*
    common_defs.h : 
*/

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

typedef union {
    struct {
        uint8_t l;
        uint8_t h;
    } b;
    uint16_t w;
} WORDCONV;

typedef union {
    struct {
        uint8_t ll;
        uint8_t lh;
        uint8_t hl;
        uint8_t hh;
    } b;
    struct {
        uint16_t wl;
        uint16_t wh;
    } w;
    uint32_t lw;
} LONGCONV;

typedef union {
    struct {
        uint8_t lll;
        uint8_t llh;
        uint8_t lhl;
        uint8_t lhh;
        uint8_t hll;
        uint8_t hlh;
        uint8_t hhl;
        uint8_t hhh;
    } b;
    struct {
        uint16_t wll;
        uint16_t wlh;
        uint16_t whl;
        uint16_t whh;
    } w;
    struct {
        uint32_t lwl;
        uint32_t lwh;
    }lw;
    uint64_t llw;
} LONGLONGCONV;


//?????
void itoh_org(uint8_t *,uint8_t);
void ultoh_org(uint8_t *,uint32_t);
uint8_t htoi_org(uint8_t *);
uint32_t htoul_org(uint8_t *,uint8_t);
void itoa_org(uint8_t *,uint8_t);
uint8_t atoi_org(uint8_t *);
void itoa3_org(uint8_t *,uint8_t);
uint8_t atoi3_org(uint8_t *);
void ustoa_org(uint8_t *,uint16_t);

#if 0
//????????stdlib????
#define itoh itoh_org
#define ultoh ultoh_org
#define htoi htoi_org
#define htoul htoul_org
#define itoa itoa_org
#define atoi atoi_org
#define ustoa ustoa_org
#endif