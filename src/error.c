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
#include <pspctrl.h>
#include <pspdisplay.h>

#include "lib/glib2d/glib2d.h"
#include "main.h"
#include "tex.h"
#include "music.h"
#include "error.h"

// Errors
static const app_error app_errors[] = 
{
  {},
  {0x80000000 | -ERROR_SETUP_CALLBACKS,      "ERROR_SETUP_CALLBACKS",      "Something went wrong while configuring the Exit Callback."},
  {0x80000000 | -ERROR_TEXTURES_NOT_FOUND,   "ERROR_TEXTURES_NOT_FOUND",   "Textures are missing from 'assets/textures/'!\n  Make sure all PNGs are in the correct directory."},
  {0x80000000 | -ERROR_ALLOCATING_TEXTURES,  "ERROR_ALLOCATING_TEXTURES",  "Unable to allocate memory for textures."},
  {0x80000000 | -ERROR_GETTING_TIME_RTC,     "ERROR_GETTING_TIME_RTC",     "sceRtcGetCurrentClockLocalTime() failed to provide time."},
  {0x80000000 | -ERROR_UNKNOWN,              "ERROR_UNKNOWN",              "Something unknown went very wrong."},
}; static const uint app_errors_size = ARRAY_SIZE(app_errors);

// Display error to screen (aborts app...)
int app_error_display(app_error_type err)
{
  // Clear stuff
  g2dTerm();
  clock_tex_free();
  music_end();

  cbool error_running = TRUE;
  const app_error* error;
  
  if (-err < 0 || -err >= app_errors_size)
  {
    error = &app_errors[app_errors_size-1];
  }
  else
  {
    error = &app_errors[-err];
  }

  pspDebugScreenInit();

  SceCtrlLatch err_latch;

  while ( error_running )
  {
    // Print error
    pspDebugScreenSetXY(2, 2);
    pspDebugScreenSetTextColor(YELLOW);
    pspDebugScreenPrintf("%s %s by danssmnt", app_inf.name, app_inf.v_string);

    pspDebugScreenSetXY(2, 5);
    pspDebugScreenSetTextColor(RED);
    pspDebugScreenPrintf("An error occurred");

    pspDebugScreenSetXY(2, 9);
    pspDebugScreenSetTextColor(WHITE);
    pspDebugScreenPrintf("Error Name:        %s", error->name);

    pspDebugScreenSetXY(2, 11);
    pspDebugScreenPrintf("Error Code:        0x%08X", error->id);

    pspDebugScreenSetXY(2, 15);
    pspDebugScreenPrintf("Error Description: %s", error->msg);

    pspDebugScreenSetXY(2, 31);
    pspDebugScreenPrintf("Press SELECT to exit the app.");

    sceDisplayWaitVblankStart();

    sceCtrlReadLatch(&err_latch);

    // Press Select to exit the app
    if ( err_latch.uiMake & PSP_CTRL_SELECT )
    {
      error_running = FALSE;
    }
  }

  sceKernelExitGame();

  return 0;
}