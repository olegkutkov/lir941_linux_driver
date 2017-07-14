/*
   lir941r.h
	- defines for lir device

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

#ifndef LIR_941_R_H
#define LIR_941_R_H

#include <linux/types.h>

#define LIR_941_VENDOR_ID_1 0x0F0F
#define LIR_941_PRODUCT_ID_1 0x0F0F

struct lir941r_driver {
	u8 __iomem *hwmem;
};

#endif

