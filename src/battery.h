/*
 *  Digital Clock for PSP
 *
 *  Copyright (C) 2025, danssmnt
 *  
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef BATTERY_H_
#define BATTERY_H_

#include "utils.h"
#include "tex.h"

typedef int bat_perc;
typedef uchar bat_status;

enum
{
    BAT_STATUS_FULL,
    BAT_STATUS_3BARS,
    BAT_STATUS_2BARS,
    BAT_STATUS_1BARS,
    BAT_STATUS_EMPTY,
};

bat_status bat_get_status();
void get_tex_by_curr_bat_status(app_tex** out);

#endif /* BATTERY_H_ */