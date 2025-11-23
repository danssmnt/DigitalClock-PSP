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

#ifndef TEX_H_
#define TEX_H_

#include <psprtc.h>

#include "lib/glib2d/glib2d.h"

typedef struct 
{
  const char* filename;
  g2dTexture* tex;
} app_tex;

enum 
{
  T_ZERO, T_ONE, T_TWO, T_THREE, T_FOUR, 
  T_FIVE, T_SIX, T_SEVEN, T_EIGHT, T_NINE, 
  T_COLON, 
  T_DASH, 
  T_DOT_BOTTOM, 
  T_ICON_MUSIC, 
  T_ALARM_DOT,
  T_BAT_FULL,
  T_BAT_3BAR,
  T_BAT_2BAR,
  T_BAT_1BAR,
  T_BAT_EMPTY,
  
  T_COUNT
};

struct clock_tex_struct
{
  app_tex zero, one, two, three, four, 
          five, six, seven, eight, nine;
  app_tex colon;
  app_tex dash;
  app_tex dot_bottom;
  app_tex icon_music;
  app_tex alarm_dot;

  app_tex bat_full;
  app_tex bat_3bar;
  app_tex bat_2bar;
  app_tex bat_1bar;
  app_tex bat_empty;
};

union clock_tex
{
  struct clock_tex_struct s;
  app_tex a[T_COUNT];
};

struct clock_tex_draw
{
  app_tex* time[4];
  app_tex* date[4];
};

extern union clock_tex main_clock_tex;
extern struct clock_tex_draw curr_tex_draw;

int clock_tex_alloc(void);
int clock_tex_free(void);
int clock_build_curr_tex_draw(const ScePspDateTime* time);
int tex_draw(app_tex* tex, const ScePspFVector2* pos, const ScePspFVector2* size, g2dColor color);

#endif /* TEX_H_ */