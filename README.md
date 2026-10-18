Cardputer Cyberpunk MP3 Player
Created by Otaku780

A neon-styled MP3 player for the M5Stack Cardputer Adv, featuring a cyan and magenta terminal interface, animated equalizer bars, a battery percentage display, and elapsed playback time.

Features
Cyan-on-black cyberpunk terminal look with magenta accents. Scrollable song list read from the microSD card. Animated bouncing equalizer bars while a track plays. Battery percentage shown on screen. Elapsed playback time counter. Play, pause, next, previous, and back-to-list controls.

Hardware needed
M5Stack Cardputer Adv, ESP32-S3 based. A microSD card loaded with MP3 files in the root folder.

Libraries required, install via Arduino IDE Library Manager
M5Cardputer. ESP8266Audio, by earlephilhower.

Setup instructions
Install the Arduino IDE and add ESP32 board support. Under Tools, Board, select the ESP32 family, then the M5Stack folder, then choose M5Cardputer. Install the M5Cardputer and ESP8266Audio libraries through the Library Manager. Copy MP3 files onto a microSD card and insert it into the Cardputer. Open the sketch file in this repository, upload it to the device, and enjoy.

Controls
Semicolon key moves up in the song list. Period key moves down in the song list. Enter plays the selected song. P pauses or resumes. N skips to the next track. B goes to the previous track. Q stops and returns to the song list.

License
MIT License. Free to use, modify, and share.


