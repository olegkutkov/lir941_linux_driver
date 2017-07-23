/*
   log.h
	- simple logger

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


#ifndef LIR_LOG_H
#define LIR_LOG_H

#include <linux/kernel.h>
#include "info.h"

#ifndef LOG_LEVEL_INFO
#define LOG_LEVEL_INFO "INFO"
#endif

#ifndef LOG_LEVEL_ERROR
#define LOG_LEVEL_ERROR "ERROR"
#endif

#ifndef LOG_LEVEL_WARNING
#define LOG_LEVEL_WARNING "WARNING"
#endif

/*
 */
static void __log_func(const char* lvl, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	printk("%s [%s]: ", DRIVER_NAME, lvl);
	vprintk(fmt, args);
	va_end(args);
}

#ifndef LOG_INFO
#define LOG_INFO(fmt, args...) \
		__log_func(LOG_LEVEL_INFO, fmt, ## args)
#endif

#ifndef LOG_ERROR
#define LOG_ERROR(fmt, args...) \
		__log_func(LOG_LEVEL_ERROR, fmt, ## args)
#endif

#ifndef LOG_WARNING
#define LOG_WARNING(fmt, args...) \
		__log_func(LOG_LEVEL_WARNING, fmt, ## args)
#endif

#endif
