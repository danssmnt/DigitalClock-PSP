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

#include <pspkernel.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <pspgu.h>
#include <psprtc.h>
#include <psptypes.h>
#include <pspctrl.h>
#include <time.h>
#include <stdlib.h>
#include <psppower.h>

#include "main.h"
#include "lib/glib2d/glib2d.h"
#include "callbacks.h"
#include "utils.h"
#include "error.h"
#include "tex.h"
#include "battery.h"
#include "music.h"

const app_info app_inf = 
{
  .name = "Digital Clock",
  .v = { .major = 2, .minor = 2, .patch = 0, .type = VER_RELEASE },
};

cbool app_running = TRUE;
cbool app_play_music = FALSE;

PSP_MODULE_INFO("Digital Clock", PSP_MODULE_USER, app_inf.v.major, app_inf.v.minor);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER | PSP_THREAD_ATTR_VFPU);

int main(int args, char* argv[])
{
  // -Wextra
  (void)args; (void)argv;

  // INIT //////////////////////////////////////////

  // Callbacks should never fail
  if ( setup_callbacks() < 0 )
  {
    app_running = FALSE;
    app_error_display(ERROR_SETUP_CALLBACKS);
  }
  
  // Neither should allocating mem for textures
  // Error handling is done by the function itself
  if ( clock_tex_alloc() < 0 )
  {
    app_running = FALSE;
  }

  srand(time(NULL));
  get_app_v_string(&app_inf);

  // Was music initialized correctly?
  cbool music_initialized = music_init() == 0;

  // Forces PPSSPP to play .mp3 with sampling rates != 44100
  // Take a look around https://github.com/hrydgard/ppsspp/blob/master/Core/HLE/sceMp3.cpp#L479
  // for more info...
  // Playback for 48 kHz might be very bad though on PPSSPP
  // This might break in the future?
  sceKernelSetCompiledSdkVersion(0x03060000);

  // No need to run at 222 or even 333 mHz, app is very light and
  // we can preserve more battery power this way
  scePowerSetClockFrequency(133, 133, 66);

  // INIT //////////////////////////////////////////

  const g2dColor bg_color = BLACK;

  // Available Clock Colors (L / R to change)
  const g2dColor clock_colors[] = {RED, ORANGE, YELLOW, CHARTREUSE, 
                                   GREEN, SPRING_GREEN, CYAN, AZURE, BLUE, 
                                   VIOLET, MAGENTA, ROSE, WHITE, GRAY};
  const int clock_colors_size = ARRAY_SIZE(clock_colors);
  int curr_clock_color_index = 0;

  // Available Brightness Modes (Square to change)
  const uchar brightness_modes[] = { 255, 128, 64 };
  const uchar brightness_modes_size = ARRAY_SIZE(brightness_modes);
  int curr_brightness_index = 0;
  
  const ScePspFVector2 clock_big_size_sprites = { 80.0f, 160.0f };
  const ScePspFVector2 clock_small_size_sprites = { 20.0f, 40.0f };
  
  const ScePspFVector2 clock_time_pos_sprites = { 60.0f, 120.0f };
  ScePspFVector2 clock_date_pos_sprites = { 360.0f, 240.0f };

  const ScePspFVector2 clock_bat_pos_sprite = { 330.0f, 240.0f };
  const ScePspFVector2 clock_music_pos_sprite = { 300.0f, 240.0f };
  
  ScePspFVector2 curr_pos_time_sprites = clock_time_pos_sprites;
  ScePspFVector2 curr_pos_date_sprites = clock_date_pos_sprites;

  // Centered
  const ScePspFVector2 clock_time_pos_colon = { (float)G2D_SCR_W / 2.0f, 120.0f };
  const ScePspFVector2 clock_date_pos_dot   = { 410.0f, 240.0f };
  
  ScePspDateTime curr_time = {0};
  int old_min = -1;

  SceCtrlLatch latch;
  ScePspDateTime time_cust;

  app_tex* bat_tex;

  g2dInit();
  
  while ( app_running )
  {
    // GRAPHICS ///////////////////////////////////////

    curr_pos_time_sprites.x = clock_time_pos_sprites.x;
    curr_pos_date_sprites.x = clock_date_pos_sprites.x;

    g2dClear(bg_color);
    
    // Check if curr_time has been initialized
    if (curr_time.year != 0)
    {
      old_min = curr_time.minute;
    }

    // // This should never fail, otherwise something is horribly wrong!
    if ( sceRtcGetCurrentClockLocalTime(&curr_time) < 0 )
    {
      app_running = FALSE;
      app_error_display(ERROR_GETTING_TIME_RTC);
    }
    
    // For checking if min has changed (instead of building the texture array every cycle)
    if ( old_min != curr_time.minute )
    {
      clock_build_curr_tex_draw(&curr_time);
      clock_build_curr_tex_draw(&curr_time);
    }
    
    // Clock display time: 4 digits (2 for hour and 2 for min)
    for ( int tile = 0; tile < 4; tile++ )
    {
      // Draw if it's not NULL (eg.: first tile is 0)
      if (curr_tex_draw.time[tile])
      {
        tex_draw(curr_tex_draw.time[tile], &curr_pos_time_sprites, &clock_big_size_sprites, G2D_MODULATE(clock_colors[curr_clock_color_index], brightness_modes[curr_brightness_index], 255));
      }
      
      curr_pos_time_sprites.x += 120.0f;
    }

    sceRtcGetCurrentClockLocalTime(&time_cust);
    
    // Draw colon every even second (for blinking)
    if ( time_cust.second % 2 == 0 )
    {
      tex_draw(&main_clock_tex.s.colon, &clock_time_pos_colon, &clock_big_size_sprites, G2D_MODULATE(clock_colors[curr_clock_color_index], brightness_modes[curr_brightness_index], 255));
    }

    for ( int tile = 0; tile < 4; tile++ )
    {
      // Draw if it's not NULL (eg.: first tile is 0)
      if (curr_tex_draw.date[tile])
      {
        tex_draw(curr_tex_draw.date[tile], &curr_pos_date_sprites, &clock_small_size_sprites, G2D_MODULATE(clock_colors[curr_clock_color_index], brightness_modes[curr_brightness_index], 255));
      }
      
      curr_pos_date_sprites.x += 30.0f;
    }

    tex_draw(&main_clock_tex.s.dot_bottom, &clock_date_pos_dot, &clock_small_size_sprites, G2D_MODULATE(clock_colors[curr_clock_color_index], brightness_modes[curr_brightness_index], 255));
    
    get_tex_by_curr_bat_status(&bat_tex);

    tex_draw(bat_tex, &clock_bat_pos_sprite, &clock_small_size_sprites, G2D_MODULATE(clock_colors[curr_clock_color_index], brightness_modes[curr_brightness_index], 255));

    if (app_play_music)
    {
      tex_draw(&main_clock_tex.s.icon_music, &clock_music_pos_sprite, &clock_small_size_sprites, G2D_MODULATE(clock_colors[curr_clock_color_index], brightness_modes[curr_brightness_index], 255));
    }

    g2dFlip(G2D_VSYNC);

    // GRAPHICS ///////////////////////////////////////

    // CONTROLS ///////////////////////////////////////

    sceCtrlReadLatch(&latch);

    // Press START to change color
    if ( latch.uiMake & PSP_CTRL_START )
    {
      curr_clock_color_index = (curr_clock_color_index + 1) % clock_colors_size;
    }

    // Press Select to change brightness mode
    if ( latch.uiMake & PSP_CTRL_SELECT )
    {
      curr_brightness_index = (curr_brightness_index + 1) % brightness_modes_size;
    }

    // Press Cross to toggle playing music
    if ( latch.uiMake & PSP_CTRL_CROSS && music_initialized )
    {
      app_play_music = !app_play_music;
      if (app_play_music) music_play_random(FALSE);
      else 
      {
        if (music_thread_playing) music_stop();
      }
    }

    // Press Left or Right to switch current music
    if (app_play_music)
    {
      if ( latch.uiMake & PSP_CTRL_LEFT )
      {
        player_skipped_song = TRUE;
        music_play_random(TRUE);
      }
      else if ( latch.uiMake & PSP_CTRL_RIGHT )
      {
        player_skipped_song = TRUE;
        music_play_random(FALSE);
      }
    }

    // CONTROLS ///////////////////////////////////////
  }

  g2dTerm();
  clock_tex_free();
  music_end();

  sceKernelExitGame();

  return 0;
}