/*
   lir941r.c
	- kernel module entry point

   Copyright 2017  Oleg Kutkov <elenbert@gmail.com>

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

#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");

static int lir941r_init(void)
{
	printk(KERN_ALERT "Hello, world\n");

	return 0;
}

static void lir941r_exit(void)
{
	printk(KERN_ALERT "Goodbye\n");
}

module_init(lir941r_init);
module_exit(lir941r_exit);

