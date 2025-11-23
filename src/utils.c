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

#include <pspiofilemgr.h>
#include <pspkernel.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "utils.h"

/* Check if file exists by checking attributes
 * Better than opening and closing it
 */
cbool file_exists(const char* file_path)
{
  if (file_path == 0) return FALSE;

  SceIoStat stat = {0};
  
  if ( sceIoGetstat(file_path, &stat) < 0 )
  {
    return FALSE;
  }

  return FIO_SO_ISREG(stat.st_attr);
}

/* Check if dir exists by checking attributes
 * Better than opening and closing it
 */
cbool dir_exists(const char* dir_path)
{
  if (dir_path == 0) return FALSE;

  SceIoStat stat = {0};
  
  if ( sceIoGetstat(dir_path, &stat) < 0 )
  {
    return FALSE;
  }

  return FIO_SO_ISDIR(stat.st_attr);
}

void get_app_v_string(const app_info* app_inf)
{
  if (app_inf == 0) return;

  snprintf((char*)app_inf->v_string, sizeof(app_inf->v_string), "%i.%i.%i%c", app_inf->v.major, app_inf->v.minor, app_inf->v.patch, app_inf->v.type);
}

cbool str_endswith(const char *str, const char* endswith) 
{
  return !strcasecmp(str + strlen(str) - strlen(endswith), endswith);
}

uint get_rand_range_uint(uint min, uint max)
{
  return (rand() % (max - min + 1)) + min;
}