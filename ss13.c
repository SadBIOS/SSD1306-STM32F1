#include "ss13.h"

extern I2C_HandleTypeDef hi2c1;

#define dis_wCMD(cmd) dis_i2c_bus_wr_sng(i2c_div_adr, 0x00, (cmd)) //cmd write
#define dis_wDAT(dat) dis_i2c_bus_wr_sng(i2c_div_adr, 0x40, (dat)) //dat write

static uint8_t ctlr_bfr[dis_wid * dis_hgt / 8];

typedef struct {
    uint16_t curX;
    uint16_t curY;
    uint8_t inv;
    uint8_t init;
} dis_ctlr_priv_strct;

static dis_ctlr_priv_strct ctlr;

#define dINV 0xA7 //display inverted mode
#define dNML 0xA6 //display normal mode
#define dSCLR_deACT 0x2E //disable display scrolling

void dis_inv_gbl(int k) {
    if(k) {
        dis_wCMD(dINV);
    }
    else {
        dis_wCMD(dNML);
    }
}

uint8_t dis_init(void) {
    dis_i2c_bus_init();
    
    if (HAL_I2C_IsDeviceReady(&hi2c1, i2c_div_adr, 1, 20000) != HAL_OK) {
        return 0;
    }

    uint32_t dlc = 2500;
    while(dlc > 0)
        dlc--;

    dis_wCMD(0xAE);
	dis_wCMD(0x20);  
	dis_wCMD(0x10);
	dis_wCMD(0xB0);
	dis_wCMD(0xC8);
	dis_wCMD(0x00);
	dis_wCMD(0x10);
	dis_wCMD(0x40);
	dis_wCMD(0x81);
	dis_wCMD(0xFF);
	dis_wCMD(0xA1);
	dis_wCMD(0xA6);
	dis_wCMD(0xA8);
	dis_wCMD(0x3F);
	dis_wCMD(0xA4);
	dis_wCMD(0xD3);
	dis_wCMD(0x00);
	dis_wCMD(0xD5);
	dis_wCMD(0xF0);
	dis_wCMD(0xD9);
	dis_wCMD(0x22);
	dis_wCMD(0xDA);
	dis_wCMD(0x12);
	dis_wCMD(0xDB);
	dis_wCMD(0x20);
	dis_wCMD(0x8D);
	dis_wCMD(0x14);
	dis_wCMD(0xAF);
    dis_wCMD(dSCLR_deACT);
    dis_wCMD(px_off);
    dis_updt();

    ctlr.curX = 0;
    ctlr.curY = 0;
    ctlr.init = 1;

    return 1;
}

void dis_updt(void) {
    uint8_t lct;
    
    for(lct = 0; lct < 8; lct++) {
        dis_wCMD(0xB0 + lct);
        dis_wCMD(0x00);
        dis_wCMD(0x10);

        dis_i2c_bus_wr_mul(i2c_div_adr, 0x40, &ctlr_bfr[dis_wid * lct], dis_wid);
    }
}

void dis_pix_invrt(void) {
    uint16_t invmem;

    ctlr.inv = !ctlr.inv;

    for(invmem = 0; invmem < sizeof(ctlr_bfr); invmem++) {
        ctlr_bfr[invmem] = ~ctlr_bfr[invmem];
    }
}

void dis_blnk_fill(dis_clr_state clr) {
    memset(ctlr_bfr, (clr == px_off) ? 0x00 : 0xFF, sizeof(ctlr_bfr));
}

void dis_drwPX(uint16_t X, uint16_t Y, dis_clr_state clr) {
    if(X >= dis_wid || Y >= dis_hgt) {
        return;
    }

    if(ctlr.inv) {
        clr = (dis_clr_state)!clr;
    }

    if(clr == px_on) {
        ctlr_bfr[X + (Y / 8) * dis_wid] |= 1<< (Y % 8);
    }
    else {
        ctlr_bfr[X + (Y / 8) * dis_wid] &= ~(1<< (Y % 8));
    }
}

void dis_pos_XY(uint16_t X, uint16_t Y) {
    ctlr.curX = X;
    ctlr.curY = Y;
}

char dis_ldRAM_char(char chr, fn_def* fnt, dis_clr_state clr) {
    uint32_t a, b, c;
    
    if(dis_wid <= (ctlr.curX + fnt->f_w) || dis_hgt <= (ctlr.curY + fnt->f_h)) {
        return 0;
    }

    for (a = 0; a < fnt->f_h; a++) {
        b = fnt->dat[(chr - 32) * fnt->f_h + a];
        for (c = 0; c < fnt->f_w; c++) {
            if ((b << c) & 0x8000) {
                dis_drwPX(ctlr.curX + c, (ctlr.curY + a), (dis_clr_state) clr);
            }
            else {
                dis_drwPX(ctlr.curX + c, (ctlr.curY + a), (dis_clr_state)!clr);
            }
        }
	}
    ctlr.curX += fnt->f_w;
    return chr;
}

char dis_ldRAM_str(char* str, fn_def* fnt, dis_clr_state clr) {
    while(*str) {
        if(dis_ldRAM_char(*str, fnt, clr) != *str) {
            return *str;
        }
        str++;
    }
    return *str;
}

void dis_clr(void) {
    dis_blnk_fill (0x00);
    dis_updt();
}

void dis_on(void) {
    dis_wCMD(0x80);
    dis_wCMD(0x14);
    dis_wCMD(0xAF);
}

void dis_off(void) {
    dis_wCMD(0x8D);
    dis_wCMD(0x10);
    dis_wCMD(0xAE);
}

void dis_i2c_bus_init() {
    uint32_t ctr = 250000;
    while(ctr > 0) {
        ctr--;
    }
}


void dis_i2c_bus_wr_mul(uint8_t adr, uint8_t reg, uint8_t *dat, uint16_t cnt) {
    uint8_t dtx[256];
    dtx[0] = reg;
    uint8_t a;
    for(a = 0; a < cnt; a++) {
        dtx[a+1] = dat[a];
    }
    HAL_I2C_Master_Transmit(&hi2c1, adr, dtx, cnt+1, 10);
}

void dis_i2c_bus_wr_sng(uint8_t adr, uint8_t reg, uint8_t dat) {
    uint8_t dtx[2];
    dtx[0] = reg;
    dtx[1] = dat;
    HAL_I2C_Master_Transmit(&hi2c1, adr, dtx, 2, 10);
}