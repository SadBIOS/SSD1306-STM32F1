#ifndef FNT_H
#define FNT_H 119

#ifdef __cplusplus
extern C {
    #endif
    #include "stm32f1xx_hal.h"
    #include "string.h"

    typedef struct {
        uint8_t f_w;
        uint8_t f_h;
        const uint16_t *dat;
    }   fn_def;

    typedef struct {
        uint16_t len;
        uint16_t hgt;
    }   fn_szt;

    extern fn_def sml;
    char* fn_get_szt(char* str, fn_szt* szt, fn_def* fnt);
    #ifdef __cplusplus
}
#endif
#endif