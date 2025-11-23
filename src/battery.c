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

#include <psppower.h>

#include "battery.h"
#include "tex.h"

bat_status bat_get_status()
{
  bat_perc perc = scePowerGetBatteryLifePercent();
  
  if (perc >= 80)
    return BAT_STATUS_FULL;

  else if (perc >= 60)
    return BAT_STATUS_3BARS;

  else if (perc >= 30)
    return BAT_STATUS_2BARS;

  else if (perc >= 10)
    return BAT_STATUS_1BARS;
  
  return BAT_STATUS_EMPTY;
}

void get_tex_by_curr_bat_status(app_tex** out)
{
  switch(bat_get_status())
    {
      case BAT_STATUS_FULL:
        *out = &main_clock_tex.s.bat_full;
        break;

      case BAT_STATUS_3BARS:
        *out = &main_clock_tex.s.bat_3bar;
        break;

      case BAT_STATUS_2BARS:
        *out = &main_clock_tex.s.bat_2bar;
        break;

      case BAT_STATUS_1BARS:
        *out = &main_clock_tex.s.bat_1bar;
        break;

      case BAT_STATUS_EMPTY:
      default:
        *out = &main_clock_tex.s.bat_empty;
        break;
    }
}