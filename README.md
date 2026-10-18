Cardputer Cyberpunk MP3 Player
Created by Otaku780

A neon-styled MP3 player for the M5Stack Cardputer Adv, featuring switchable color themes, animated equalizer bars, a battery percentage display, elapsed playback time, and volume control.

Features
Six switchable color themes, cycled with one key, all keeping the same cyberpunk layout: cyan and magenta, green monochrome, red alert, pink, orange, and yellow. Scrollable song list read from the microSD card. Animated bouncing equalizer bars while a track plays. Battery percentage shown on screen. Elapsed playback time counter. Volume control. Play, pause, next, previous, back-to-list, theme cycling, and volume controls.

Hardware needed
M5Stack Cardputer Adv, ESP32-S3 based. A microSD card loaded with MP3 files in the root folder.

Libraries required, install via Arduino IDE Library Manager
M5Cardputer. ESP8266Audio, by earlephilhower.

Setup instructions
Install the Arduino IDE and add ESP32 board support. Under Tools, Board, select the ESP32 family, then the M5Stack folder, then choose M5Cardputer. Install the M5Cardputer and ESP8266Audio libraries through the Library Manager. Copy MP3 files onto a microSD card and insert it into the Cardputer. Open the sketch file in this repository, upload it to the device, and enjoy.

Controls
Semicolon key moves up in the song list. Period key moves down in the song list. Enter plays the selected song. P pauses or resumes. N skips to the next track. B goes to the previous track. Q stops and returns to the song list. L cycles through six color themes: cyan and magenta, green monochrome, red alert, pink, orange, and yellow. G increases volume. V decreases volume.

License
MIT License. Free to use, modify, and share.


