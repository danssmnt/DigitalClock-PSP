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

#ifndef UTILS_H_
#define UTILS_H_

typedef unsigned int uint;
typedef unsigned char uchar, cbool, byte;

#define FALSE 0
#define TRUE  1

extern cbool file_exists(const char* file_path);
extern cbool dir_exists(const char* dir_path);

typedef enum
{
  VER_ALPHA   = 'a',
  VER_BETA    = 'b',
  VER_PREVIEW = 'p',
  VER_DEBUG   = 'd',
  VER_RELEASE = 'r',
} app_version_type;

typedef struct
{
  // App Name
  char name[16];

  // App Version
  struct
  {
    const byte major;
    const byte minor;
    const byte patch;
    const app_version_type type;
  } v;
  char v_string[32];
} app_info;

void get_app_v_string(const app_info* app_inf);
cbool str_endswith(const char *str, const char* endswith);
uint get_rand_range_uint(uint min, uint max);

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#endif /* UTILS_H_ */