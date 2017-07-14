/*
   lir941r_hw.h
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

#ifndef LIR_941_HW_H
#define LIR_941_HW_H

#include "lir941r.h"

void set_channel_bit(struct lir941r_driver* drv, uint8_t chnum, uint8_t bit);
void set_channel_clk(struct lir941r_driver* drv, uint8_t chnum, uint8_t clk);
void set_channel_pause_rate(struct lir941r_driver* drv, uint8_t chnum, uint8_t prate);

void start_channel_polling(struct lir941r_driver* drv, uint8_t chnum);
void stop_channel_polling(struct lir941r_driver* drv, uint8_t chnum);

void channel_generate_one_req(struct lir941r_driver* drv, uint8_t chnum);

uint8_t is_channel_polling(struct lir941r_driver* drv, uint8_t chnum);

uint32_t get_channel_data(struct lir941r_driver* drv, uint8_t chnum);

#endif

