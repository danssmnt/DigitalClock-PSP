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

#ifndef MUSIC_H_
#define MUSIC_H_

#include "utils.h"

// A playlist can have up to this many songs
#define MUSIC_PLAYLIST_SIZE 1024

typedef struct
{
    char* file_path[MUSIC_PLAYLIST_SIZE];
    uint size;
} music_playlist;

extern music_playlist current_playlist;
extern cbool music_thread_playing;
extern cbool player_skipped_song;
extern SceUID mp3_play_thid;

int music_init();
int music_stop();
int music_end();
void music_play_random(cbool back);
void music_shuffle_playlist();


#endif