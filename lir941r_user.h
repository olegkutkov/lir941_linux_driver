/*
   lir941r_user.h
	- defines of the user ioctl's

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

#ifndef LIR_941_USER_H
#define LIR_941_USER_H

#define LIR941_START_CHANNEL_POLLING 0x42
#define LIR941_STOP_CHANNEL_POLLING 0x43

#define LIR941_STOP_CHANNEL_ONE_REQUEST 0x44

#define LIR941_CHANNEL_DATAWIDTH 0x45

#define LIR941_CHANNEL_SPEED 0x50
#define LIR941_CHANNEL_PAUSE 0x51

#endif

