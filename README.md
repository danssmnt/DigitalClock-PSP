<div align="center">
    <h1>PSP Digital Clock</h1>
</div>

<div align="center">
    <img src="./pictures/DigitalClock_Red.webp" alt="Red Digital Clock" style="height: 100px;"/>
    <img src="./pictures/DigitalClock_Yellow.webp" alt="Yellow Digital Clock" style="height: 100px;"/>
    <img src="./pictures/DigitalClock_Green.webp" alt="Green Digital Clock" style="height: 100px;"/>
    <img src="./pictures/DigitalClock_LightBlue.webp" alt="Light Blue Digital Clock" style="height: 100px;"/>
    <img src="./pictures/DigitalClock_DarkBlue.webp" alt="Dark Blue Digital Clock" style="height: 100px;"/>
    <img src="./pictures/DigitalClock_Violet.webp" alt="Violet Digital Clock" style="height: 100px;"/>
</div>

---

A digital clock for your **PlayStation Portable**.
Made with [gLib2D by Geecko](https://github.com/libcg/gLib2D/).

---

## Installation

Download the latest release from [releases](https://github.com/danssmnt/Digital-Clock-PSP/releases). Unpack the ``.zip`` file and follow the installation instructions on the ``README.md`` inside it.

### Playing MP3 Music

Put your MP3 Music inside the ``ms0:/MUSIC`` directory. Then press <img src="./pictures/PSPButton_Cross.webp" alt="CROSS" style="height: 20px;"/> to play them inside the app!

For the best experience it's recommended that your MP3 files:
 - Are 44100 Hz
 - Are 128 / 192 kbits
 - Don't have ID3 tags

If your MP3 is considered invalid it won't play!

FFmpeg command for converting your MP3 files to more compatible ones:
```ffmpeg -y -i %INPUT% -ar 44100 -b:a 128k -codec:a libmp3lame -map 0:a -map_metadata -1 %OUTPUT%```
(Replace ``%INPUT%`` and ``%OUTPUT%`` with valid filenames)

---

## How to use

 - Press <img src="./pictures/PSPButton_Start.webp" alt="START" style="height: 20px;"/> to change the clock color.
 - Press <img src="./pictures/PSPButton_Select.webp" alt="SELECT" style="height: 20px;"/> to change the clock brightness.
 - Press <img src="./pictures/PSPButton_Cross.webp" alt="CROSS" style="height: 20px;"/> to toggle music ON/OFF (only works if there's music inside ``ms0:/MUSIC``).
 - Press <img src="./pictures/PSPButton_Left.webp" alt="LEFT" style="height: 20px;"/> or <img src="./pictures/PSPButton_Right.webp" alt="RIGHT" style="height: 20px;"/> to switch between songs (only if music playing is enabled).

---

## Error diagnosing
> [!NOTE]
> If you follow all the installation instructions correctly (which are very few), no errors should popup on the app.

|Error|Code|Description|More info.|
|:----|:--:|:----------|:--------|
|``ERROR_SETUP_CALLBACKS``|``0x80000000``|Something went wrong while configuring the Exit Callback.|Something is likely very wrong with your firmware?|
|``ERROR_TEXTURES_NOT_FOUND``|``0x80000001``|Textures are missing from 'assets/textures/' !  Make sure all PNGs are in the correct directory.|The most common, make sure the texture files (``0.png``, ``1.png``...) are in their correct directory (``assets/textures/``).
|``ERROR_ALLOCATING_TEXTURES``|``0x80000002``|Unable to allocate memory for textures.|Probably you're out of RAM, try enabling `High Memory Layout` in your CFW settings if that option is avaliable. Also, disabling all plugins should help too.|
|``ERROR_GETTING_TIME_RTC``|``0x80000003``|sceRtcGetCurrentClockLocalTime() failed to provide time.|Something is likely very wrong with your firmware? Either this or something is patching the function. Disabling all plugins might help?|
|``ERROR_UNKNOWN``|``0x7FFFFFFF``|Something unknown went very wrong.||

---

## Running / Compiling

### Dependencies

You'll need PSPSDK to compile this homebrew. Just use the latest, it'll work.

### Compiling

 1. Clone the repository, either by using ``git clone https://github.com/danssmnt/Digital-Clock-PSP``, or by going to "Code" -> "Download ZIP" and extracting it on your PC.
 > [!IMPORTANT]
 > (Optional) Set the ``assets/xmb/CUST_PARAM.SFO`` file to Read-Only. (To prevent ``make clean`` from replacing it).
 2. Go inside the cloned repository and, inside a terminal, write ``make``, it should compile without any errors / warnings.
 3. Run the created ``EBOOT.PBP`` on [PPSSPP](https://ppsspp.org), or directly on your PSP / Vita (Adrenaline).

### Replacing Textures

If you want / need, you can replace all textures inside ``assets/textures/`` for your own ones.

Keep in mind:

 - All textures must preserve the original aspect ratios (``1:2``).
 - Textures must be PNGs, otherwise they won't be loaded.
 - Textures must have a max. width / height of 512 (because of PSP hardware limitations).


---

## Future Checklist

 - [ ] Get XMB icon animation (``ICON1.PMF``) working
 - [ ] Timer and chronometer
 - [ ] Alarm
 - [ ] Save clock settings
 - [ ] Make a minimal fork of glib2d

---

## Changelog

### v2.2
 - Added a MP3 Music Player, play your favorite songs from ``ms0:/MUSIC``!
 - Started using VFPU. (Performance might be improved)
 - CPU Clock is now limited to 133 mHz (for battery life improvement)
 - Added a better version control in-code.
 - Slight code improvement...

### v2.1
 - Add Date
 - Add Battery Indicator
 - Added and organized textures
 - Added battery.c
 - Fixed Error Screen not showing on PSP
 - Improved Texture System
 - Now Left / Right change colors, instead of L / R
 - Improved code

### v2.0.1
 - Switched to newest PSPSDK (pspTime -> ScePspDateTime)
 - Improved binary size by cutting JPEG support.
 - Organized the code better
 - Fixed drawing the first 0 when not needed
 - Added more colors
 - File exists func improved

### v2.0
 - Remade the whole app from scratch.
 - Fixed issue [#1](https://github.com/danssmnt/Digital-Clock-PSP/issues/1) (Timezones fix).
 - Updated, optimized and improved textures for digital numbers.
 - Added colors.
 - Added brightness modes.
 - Added error messages for diagnosing problems.
 - Improved / Optimized code (a lot).

### v1.0
 - Created the app.
 - A simple digital clock with minimal features.

---

## Acknowledgements
 - [**@mizabmdgg**](https://github.com/mizabmdgg) - Found issue [#1](https://github.com/danssmnt/Digital-Clock-PSP/issues/1) and even made their own [fork](https://github.com/mizabmdgg/Digital-Clock-PSP/tree/main) to fix it.
