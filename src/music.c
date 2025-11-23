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
#include <malloc.h>
#include <string.h>
#include <pspmp3.h>
#include <pspthreadman.h>
#include <psputility.h>
#include <pspaudio.h>

#include "music.h"
#include "utils.h"
#include "main.h"

static const char* psp_music_folder = "ms0:/MUSIC/";

void music_playlist_append(music_playlist* playlist, char* append_path, size_t append_path_size)
{
  if (!playlist || playlist->size >= MUSIC_PLAYLIST_SIZE - 1 || !append_path || append_path_size < 1) return;

  playlist->file_path[playlist->size] = (char*)malloc(append_path_size + 1);

  if (playlist->file_path[playlist->size] == NULL)
  {
    return;
  }

  strncpy(playlist->file_path[playlist->size], append_path, append_path_size + 1);
  playlist->size++;
}

void music_playlist_clear(music_playlist* playlist)
{
  if (!playlist) return;

  for ( size_t idx = 0; idx < playlist->size; idx++ )
  {
    free(playlist->file_path[idx]);
    playlist->file_path[idx] = NULL;
  }

  playlist->size = 0;
}

music_playlist current_playlist;

int music_init_modules()
{
  if (sceUtilityLoadModule(PSP_MODULE_AV_AVCODEC) < 0) return -1;
  if (sceUtilityLoadModule(PSP_MODULE_AV_MP3) < 0) return -1;
  return 0;
}

int music_end_modules()
{
  sceUtilityUnloadModule(PSP_MODULE_AV_AVCODEC);
  sceUtilityUnloadModule(PSP_MODULE_AV_MP3);

  return 0;
}

int music_init()
{
  if (!dir_exists(psp_music_folder)) return -1;
  SceUID dir = sceIoDopen(psp_music_folder);
  if (dir < 0) return -1;

  SceIoDirent d_dir;
  while ( sceIoDread(dir, &d_dir) > 0 )
  {
    // Is a MP3 file?
    if ( str_endswith(d_dir.d_name, ".mp3") )
    {
      music_playlist_append(&current_playlist, d_dir.d_name, strlen(d_dir.d_name));
    }
  }
  sceIoDclose(dir);

  // If MP3 music wasn't found, don't play
  if (current_playlist.size <= 0) return -1;
  if (music_init_modules() < 0) return -1;

  music_shuffle_playlist();
  return 0;
}

cbool music_thread_playing = FALSE;
cbool player_skipped_song = FALSE;

// Input and Output buffers
unsigned char	mp3_buf[16*1024]  __attribute__((aligned(64)));
unsigned char	pcm_buf[16*(1152/2)]  __attribute__((aligned(64)));

int music_mp3_fill_stream_buf( int fd, int handle )
{
	unsigned char* dst;
	long int write;
	long int pos;

	// Get Info on the stream (where to fill to, how much to fill, where to fill from)
	if ( sceMp3GetInfoToAddStreamData( handle, &dst, &write, &pos) < 0) return -1;

	// Seek file to position requested
	if ( sceIoLseek32( fd, pos, SEEK_SET ) < 0) return -1;

	// Read the amount of data
	int read = sceIoRead( fd, dst, write );
	if (read < 0) return -1;

  // End of file?
	if (read == 0) return 0;

	// Notify mp3 library about how much we really wrote to the stream buffer
	if ( sceMp3NotifyAddStreamData( handle, read ) < 0) return -1;

	return pos > 0;
}

int music_mp3_thread_end(cbool release_audio, int channel, cbool release_handle, int handle, cbool term_resource, cbool close_file, SceUID fd)
{
  if (release_audio && channel >= 0) sceAudioSRCChRelease();
  if (release_handle) sceMp3ReleaseMp3Handle(handle);
  if (term_resource) sceMp3TermResource();
  if (close_file) sceIoClose(fd);
  music_thread_playing = FALSE;

  // Automatic Song Skip after it ends
  if (app_play_music && !player_skipped_song)
  {
    music_play_random(FALSE);
  }
  // If player skipped song
  else if (player_skipped_song)
  {
    player_skipped_song = FALSE;
  }

  sceKernelExitDeleteThread(0);

  return 0;
}

int music_mp3_player_thread(SceSize args, void* argp)
{
  if ( args < 1 || !argp )
  {
    music_mp3_thread_end(FALSE, 0, FALSE, 0, FALSE, FALSE, 0);
    return -1;
  }

  char** music_path = argp;

  // Get Music Full Path
  char music_full_path[PATH_MAX];
  snprintf(music_full_path, sizeof(music_full_path), "%s%s", psp_music_folder, *music_path);

  sceKernelPrintf("Playing: '%s'", music_full_path);

  // Open the input file
	int fd = sceIoOpen( music_full_path, PSP_O_RDONLY, 0777 );
	if (fd < 0)
  {
    music_mp3_thread_end(FALSE, 0, FALSE, 0, FALSE, FALSE, 0);
    return -1;
  }

	if ( sceMp3InitResource() < 0 )
  {
    music_mp3_thread_end(FALSE, 0, FALSE, 0, FALSE, TRUE, fd);
    return -1;
  }

  SceMp3InitArg mp3Init;
	mp3Init.mp3StreamStart = 0;
	mp3Init.mp3StreamEnd = sceIoLseek32( fd, 0, SEEK_END );
	mp3Init.mp3Buf = mp3_buf;
	mp3Init.mp3BufSize = sizeof(mp3_buf);
	mp3Init.pcmBuf = pcm_buf;
	mp3Init.pcmBufSize = sizeof(pcm_buf);

  int handle = sceMp3ReserveMp3Handle( &mp3Init );

  if ( handle < 0 )
  {
    music_mp3_thread_end(FALSE, 0, FALSE, 0, TRUE, TRUE, fd);
    return -1;
  }

  if ( music_mp3_fill_stream_buf( fd, handle ) < 0 )
  {
    music_mp3_thread_end(FALSE, 0, TRUE, handle, TRUE, TRUE, fd);
    return -1;
  }

  // On PPSSPP, if you play a .mp3 file with sampling rate != 44100, it errors out here
  // "invalid data: not 44.1kHz"
  // On PSP / PSVita, it plays normally
  // So, more sample rates are simply not implemented for PPSSPP lol
  // According to Hrydgard: "It's simply not implemented."
  if ( sceMp3Init( handle ) < 0 )
  {
    music_mp3_thread_end(FALSE, 0, TRUE, handle, TRUE, TRUE, fd);
    return -1;
  }

  int channel = -1;
  int volume = PSP_AUDIO_VOLUME_MAX;
	int samplingRate = sceMp3GetSamplingRate( handle );
	int numChannels = sceMp3GetMp3ChannelNum( handle );
	int lastDecoded = 0;

  // If you don't set the looping amount to 0, it will keep looping forever (why Sony)
  sceMp3SetLoopNum(handle, 0);

  music_thread_playing = TRUE;

  // Little pause before playing song
  // sceKernelDelayThread(1e6 * 0.5f);

  while (music_thread_playing && app_play_music)
  {
    // If more data is needed, fill stream buffer
    if (sceMp3CheckStreamDataNeeded(handle) > 0) music_mp3_fill_stream_buf(fd, handle);

    short* buf;

    int bytesDecoded = sceMp3Decode(handle, &buf);

    // Something wrong happened decoding the mp3
    if (bytesDecoded < 0 && bytesDecoded != 0x80671402) break;

    // End of stream -> stop playback
    if (bytesDecoded == 0 || bytesDecoded == 0x80671402) break;

    if (channel < 0 || lastDecoded != bytesDecoded)
    {
      if (channel >= 0) sceAudioSRCChRelease();
      channel = sceAudioSRCChReserve(bytesDecoded / (2 * numChannels), samplingRate, numChannels);
    }
    sceAudioSRCOutputBlocking( volume, buf );
    lastDecoded = bytesDecoded;
  }

  music_mp3_thread_end(TRUE, channel, TRUE, handle, TRUE, TRUE, fd);
  return 0;
}

SceUID mp3_play_thid = -1;

int music_play(char* music_path)
{
  // Don't play if music wasn't found
  if (!music_path || current_playlist.size <= 0) return -1;

  // If user skipped song, stop playing the current one
  if (music_thread_playing)
  {
    music_stop();
  }

  mp3_play_thid = sceKernelCreateThread("MP3 Player", music_mp3_player_thread, 0x11, 0x2000, 0, NULL);
  if (mp3_play_thid < 0) return -1;

  int res = sceKernelStartThread(mp3_play_thid, sizeof(char**), &music_path);
  if (res < 0) return -1;

  return 0;
}

static int current_playlist_order[MUSIC_PLAYLIST_SIZE];
static int current_song_index = 0;

void music_shuffle_playlist()
{
  // Fill playlist order array
  for (int i = 0; i < current_playlist.size; i++)
  {
    current_playlist_order[i] = i;
  }

  // Shuffle playlist order array (swaps indexes around)
  for (int i = current_playlist.size - 1; i > 0; i--)
  {
    int j = (int)get_rand_range_uint(0, i);

    // Swap
    int temp = current_playlist_order[i];
    current_playlist_order[i] = current_playlist_order[j];
    current_playlist_order[j] = temp;
  }
}

void music_play_random(cbool back)
{
  // Don't play if music wasn't found
  if (current_playlist.size <= 0) return;

  // Loop through playlist again, player has played all mp3 music available
  if (current_song_index >= current_playlist.size)
  {
    current_song_index = 0;
    music_shuffle_playlist();
  }

  // Go back to previous song
  if (back)
  {
    current_song_index -= 2;
    if (current_song_index < 0) current_song_index = 0;
  }

  char* music = current_playlist.file_path[current_playlist_order[current_song_index]];

  music_play(music);
  current_song_index++;
}

int music_stop()
{
  // Stop playing music and wait until music thread ends
  music_thread_playing = FALSE;
  sceKernelWaitThreadEnd(mp3_play_thid, NULL);

  return 0;
}

int music_end()
{
  app_play_music = FALSE;
  music_end_modules();
  music_playlist_clear(&current_playlist);
  return 0;
}