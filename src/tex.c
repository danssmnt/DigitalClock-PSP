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

#include <stdio.h>
#include <psprtc.h>
#include <pspkernel.h>

#include "lib/glib2d/glib2d.h"
#include "utils.h"
#include "error.h"
#include "tex.h"

// All needed textures
union clock_tex main_clock_tex = 
{
  .s = 
  {
    .zero.filename        = "timedate/0",
    .one.filename         = "timedate/1",
    .two.filename         = "timedate/2",
    .three.filename       = "timedate/3",
    .four.filename        = "timedate/4",
    .five.filename        = "timedate/5",
    .six.filename         = "timedate/6",
    .seven.filename       = "timedate/7",
    .eight.filename       = "timedate/8",
    .nine.filename        = "timedate/9",
    .colon.filename       = "timedate/colon",
    .dash.filename        = "timedate/dash",
    .dot_bottom.filename  = "timedate/dot_bottom",
    .icon_music.filename  = "icon_music",
    .alarm_dot.filename   = "alarm_dot",
    .bat_full.filename    = "battery/icon_battery_4bars",
    .bat_3bar.filename    = "battery/icon_battery_3bars",
    .bat_2bar.filename    = "battery/icon_battery_2bars",
    .bat_1bar.filename    = "battery/icon_battery_1bars",
    .bat_empty.filename   = "battery/icon_battery_empty",
  },
};

struct clock_tex_draw curr_tex_draw = {0};

static const char* tex_filepath = "assets/textures/";

int get_tex_full_path(const app_tex* tex, char* out, size_t size)
{
  if ( !tex || (tex->filename[0] == '\0') || !out || size <= 0 )
  {
    return ERROR_UNKNOWN;
  }

  snprintf(out, size, "%s%s.png", tex_filepath, tex->filename);

  return 0;
}

int app_tex_alloc(app_tex* tex)
{
  if ( !tex || (tex->filename[0] == '\0') )
  {
    return ERROR_UNKNOWN;
  }

  char tex_filepath[PATH_MAX];

  if ( get_tex_full_path(tex, tex_filepath, sizeof(tex_filepath)) < 0 )
  {
    return ERROR_UNKNOWN;
  }

  if ( !file_exists(tex_filepath) )
  {
    return ERROR_TEXTURES_NOT_FOUND;
  }

  tex->tex = g2dTexLoad(tex_filepath, G2D_SWIZZLE);

  if ( !tex->tex )
  {
    return ERROR_ALLOCATING_TEXTURES;
  }

  return 0;
}

int app_tex_free(app_tex* tex)
{
  if ( !tex || !tex->tex )
  {
    return ERROR_UNKNOWN;
  }

  g2dTexFree(&tex->tex);

  return 0;
}

int clock_tex_alloc(void)
{
  for (int tex_i = 0; tex_i < T_COUNT; tex_i++)
  {
    int ret = app_tex_alloc(&main_clock_tex.a[tex_i]);

    if (ret < 0)
    {
      app_error_display(ret);
      return -1;
    }
  }

  return 0;
}

// Frees allocated mem for textures
int clock_tex_free(void)
{
  for (int tex_i = 0; tex_i < T_COUNT; tex_i++)
  {
    app_tex_free(&main_clock_tex.a[tex_i]);
  }

  return 0;
}

int clock_build_curr_tex_draw(const ScePspDateTime* time)
{
  // This shouldn't fail, if it fails, nothing will be drawn
  if ( !time )
  {
    return -1;
  }

  int h_1digit = (time->hour / 10);
  int h_2digit = (time->hour % 10);

  int mi_1digit = (time->minute / 10);
  int mi_2digit = (time->minute % 10);

  int d_1digit = (time->day / 10);
  int d_2digit = (time->day % 10);

  int mo_1digit = (time->month / 10);
  int mo_2digit = (time->month % 10);

  // Organize tiles, if first tile is 0, don't draw
  curr_tex_draw.time[0] = h_1digit != 0 ? &main_clock_tex.a[h_1digit] : NULL;
  curr_tex_draw.time[1] = &main_clock_tex.a[h_2digit];
  curr_tex_draw.time[2] = &main_clock_tex.a[mi_1digit];
  curr_tex_draw.time[3] = &main_clock_tex.a[mi_2digit];

  curr_tex_draw.date[0] = d_1digit != 0 ? &main_clock_tex.a[d_1digit] : NULL;
  curr_tex_draw.date[1] = &main_clock_tex.a[d_2digit];
  curr_tex_draw.date[2] = mo_1digit != 0 ? &main_clock_tex.a[mo_1digit] : NULL;
  curr_tex_draw.date[3] = &main_clock_tex.a[mo_2digit];

  return 0;
}

// Draw textures on screen (with glib2d)
int tex_draw(app_tex* tex, const ScePspFVector2* pos, const ScePspFVector2* size, g2dColor color)
{
  if ( !tex || !pos || !size || G2D_GET_A(color) == 0)
  {
    return -1;
  }
  
  // Render texture on screen with params
  g2dBeginRects(tex->tex);
  g2dSetCoordMode(G2D_CENTER);
  g2dSetCoordXY(pos->x, pos->y);
  g2dSetScaleWH(size->x, size->y);
  g2dSetColor(color);
  g2dAdd();
  g2dEnd();

  return 0;
}