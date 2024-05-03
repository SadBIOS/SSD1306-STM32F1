#ifndef SS13_H
#define SS13_H 213

#ifdef __cplusplus
extern C {
    #endif
    #include "stm32f1xx_hal.h"
    #include "stdlib.h"
    #include "string.h"
    #include "fnt.h"

    #ifndef i2c_div_adr
    #define i2c_div_adr 0x78 //device i2c address
    #endif

    #ifndef dis_wid
    #define dis_wid 128 //display width in pixels
    #endif

    #ifndef dis_hgt
    #define dis_hgt 64 //display height in pixels
    #endif

    #ifdef i2c_gen_timeout
    #define i2c_gen_timeout 20000
    #endif

    typedef enum {
        px_off = 0x00,
        px_on = 0x01
    }   dis_clr_state;

    uint8_t dis_init(void);
    void dis_updt(void);
    void dis_pix_invrt(void);
    void dis_blnk_fill(dis_clr_state);
    void dis_pos_XY(uint16_t X, uint16_t Y);
    char dis_ldRAM_char(char chr, fn_def* fnt, dis_clr_state clr);
    char dis_ldRAM_str(char* str, fn_def* fnt, dis_clr_state clr);
    void dis_drwPX(uint16_t X, uint16_t Y, dis_clr_state clr);
    void dis_i2c_bus_init();
    void dis_i2c_bus_wr_sng(uint8_t adr, uint8_t reg, uint8_t dat);
    void dis_i2c_bus_wr_mul(uint8_t adr, uint8_t reg, uint8_t *dat, uint16_t cnt);
    void dis_inv_gbl(int k); // k = 1 inverted, k = 0 normal
    void dis_clr(void);
    void dis_on(void);
    
    #ifdef __cplusplus
}
#endif
#endif