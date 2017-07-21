/*
   lir941r_hw.c
	- hw communication module

   Copyright 2017  Oleg Kutkov <elenbert@gmail.com>
                   Crimean astrophysical observatory

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  
 */

#include <linux/pci.h>
#include "lir941r_hw.h"

/*
 * define bit nums in channel control/status register
 */
#define CHREG_ON_REQ_BIT 0
#define CHREG_POLLING_BIT 1

#define CHREG_EXT1_BIT 2
#define CHREG_EXT2_BIT 3
#define CHREG_EXT3_BIT 4
#define CHREG_EXT4_BIT 5

#define CHREG_STATUS_TR_ACTIVE_BIT 6
#define CHREG_STATUS_TR_PAUSE_BIT 7

#define CHREG_CONTROL_MODE 7

#define MODE_NORMAL_SSI 0
#define MODE_LEGACY_SSI 1

/* */

/*
 * data access macros
 */
#define CHANNEL_DATA_OFFSET(chnum) (sizeof(uint32_t) * chnum)
#define CHANNEL_DATA_BIT_OFFSET(chnum) (sizeof(uint8_t) * chnum)
#define CHANNEL_CLK_RATE_OFFSET(chnum) (sizeof(uint32_t) + (sizeof(uint16_t) * chnum))
#define CHANNEL_PAUSE_RATE_OFFSET(chnum) ((sizeof(uint32_t) * 3) + (sizeof(uint16_t) * chnum))
#define CHANNEL_RG_ST_OFFSET(chnum) ((sizeof(uint32_t) * 4) + (sizeof(uint8_t) * chnum))
#define CHANNEL_RG_CTRL_OFFSET(chnum) ((sizeof(uint32_t) * 5) + (sizeof(uint8_t) * chnum))

#define CHECK_BIT(var,pos)  ((var) & (1 << (pos)))
#define SET_BIT(var, pos)   var |= (1 << pos)
#define CLEAR_BIT(var, pos) var &= ~(1 << pos)

/* */

static uint8_t ADAPTER_CHANNEL_MODE = MODE_NORMAL_SSI;
static uint8_t ADAPTER_ALLOW_EXT1 = 1;
static uint8_t ADAPTER_ALLOW_EXT2 = 1;
static uint8_t ADAPTER_ALLOW_EXT3 = 1;
static uint8_t ADAPTER_ALLOW_EXT4 = 1;


void set_channel_bit(struct lir941r_driver* drv, uint8_t chnum, uint8_t bit)
{
	void* wr_addr = drv->hwmem + CHANNEL_DATA_BIT_OFFSET(chnum);

	printk("lirdevice channel %i  set data width = %i\n", chnum, bit);

	iowrite8(bit, wr_addr);
}

void set_channel_clk(struct lir941r_driver* drv, uint8_t chnum, uint16_t clk)
{
	void* wr_addr = drv->hwmem + CHANNEL_CLK_RATE_OFFSET(chnum);

	printk("lirdevice channel %i  set clock div = %i\n", chnum, clk);

	iowrite16(clk, wr_addr);
}

void set_channel_pause_rate(struct lir941r_driver* drv, uint8_t chnum, uint16_t prate)
{
	void* wr_addr = drv->hwmem + CHANNEL_PAUSE_RATE_OFFSET(chnum);

	printk("lirdevice channel %i  set pause rate = %i\n", chnum, prate);

	iowrite16(prate, wr_addr);
}

static void wait_for_transaction_finish(struct lir941r_driver* drv, uint8_t chnum)
{
	void* r_addr;
	uint8_t reg;

	r_addr = drv->hwmem + CHANNEL_RG_ST_OFFSET(chnum);

	while (1) {
		reg = ioread8(r_addr);

		if (!CHECK_BIT(reg, CHREG_STATUS_TR_ACTIVE_BIT)) {
			break;
		} else {
			printk("Waiting for transaction finish...\n");
		}
	}
}

static void wait_for_pause_in_tx(struct lir941r_driver* drv, uint8_t chnum)
{
	void* r_addr;
	uint8_t reg;

	r_addr = drv->hwmem + CHANNEL_RG_ST_OFFSET(chnum);

	while(1) {
		reg = ioread8(r_addr);

		if (!CHECK_BIT(reg, CHREG_STATUS_TR_PAUSE_BIT)) {
			break;
		} else {
			printk("Waiting for transaction pause...\n");
		}
	}

}

static uint8_t create_basic_config_reg(void)
{
	uint8_t value = 0x0;

	if (ADAPTER_CHANNEL_MODE != MODE_NORMAL_SSI) {
		SET_BIT(value, CHREG_CONTROL_MODE);
	}

	if (ADAPTER_ALLOW_EXT1) {
		SET_BIT(value, CHREG_EXT1_BIT);
	}

	if (ADAPTER_ALLOW_EXT2) {
		SET_BIT(value, CHREG_EXT2_BIT);
	}

	if (ADAPTER_ALLOW_EXT3) {
		SET_BIT(value, CHREG_EXT3_BIT);
	}

	if (ADAPTER_ALLOW_EXT4) {
		SET_BIT(value, CHREG_EXT4_BIT);
	}

	// polling and single request is unset by default

	return value;
}

static void change_channel_polling_mode(struct lir941r_driver* drv, uint8_t chnum, uint8_t start)
{
	void* w_addr;
	uint8_t reg;

	w_addr = drv->hwmem + CHANNEL_RG_CTRL_OFFSET(chnum);

	reg = create_basic_config_reg();

	if (start) {
		SET_BIT(reg, CHREG_POLLING_BIT);
	}

	printk("config reg value: 0x%X\n", reg);

	iowrite8(reg, w_addr);
}

void start_channel_polling(struct lir941r_driver* drv, uint8_t chnum)
{
	change_channel_polling_mode(drv, chnum, 1);
}

void stop_channel_polling(struct lir941r_driver* drv, uint8_t chnum)
{
	change_channel_polling_mode(drv, chnum, 0);
}

void channel_generate_one_req(struct lir941r_driver* drv, uint8_t chnum)
{
	void* w_addr;
	uint8_t reg;

	w_addr = drv->hwmem + CHANNEL_RG_CTRL_OFFSET(chnum);

	reg = create_basic_config_reg();

	SET_BIT(reg, CHREG_ON_REQ_BIT);

	wait_for_transaction_finish(drv, chnum);

	iowrite8(reg, w_addr);
}

uint8_t is_channel_polling(struct lir941r_driver* drv, uint8_t chnum)
{
	void* read_addr;
	uint8_t reg;

	read_addr = drv->hwmem + CHANNEL_RG_ST_OFFSET(chnum);

	reg = ioread8(read_addr);

	return CHECK_BIT(reg, CHREG_POLLING_BIT);
}

uint32_t get_channel_data(struct lir941r_driver* drv, uint8_t chnum)
{
	uint32_t res;
	void* read_addr = drv->hwmem + CHANNEL_DATA_OFFSET(chnum);

	wait_for_pause_in_tx(drv, chnum);

	res = ioread32(read_addr);

	printk("raw data from reg: 0x%X\n", res);

	return res;
}

