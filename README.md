Cardputer Cyberpunk MP3 Player
Created by Otaku780
A cyberpunk-styled MP3 player for the M5Stack Cardputer Adv. It plays MP3 files straight from the microSD card and can handle libraries of up to 3,000 songs.
Features
• Plays MP3 files from the top folder of the SD card
• Supports up to 3,000 songs by keeping the song list in an index file on the SD card instead of in memory
• Press R on the list screen to rescan the card, so newly added MP3s show up without rebooting
• Jump ten songs at a time to get around big libraries quickly
• Song count shown in the title bar
• Auto-advance: the next song starts when one finishes, and the list loops back to the start
• Six color themes, cycled with the L key
• Volume control
• Animated 16-bar equalizer on the Now Playing screen
• Elapsed time and battery percentage display
• Display sleep: the screen turns off after 3 minutes without a key press to save battery, and any key wakes it (the waking key press is not treated as a command)
Hardware needed
• M5Stack Cardputer Adv
• microSD card with MP3 files in the top (root) folder
• USB-C cable for uploading
Libraries required
• M5Cardputer
• ESP8266Audio
Both can be installed from the Arduino IDE Library Manager.
Setup
1. Install the Arduino IDE and add ESP32 board support.
2. Install the M5Cardputer and ESP8266Audio libraries.
3. In Tools, Board, ESP32, M5Stack, choose M5Cardputer.
4. Open the sketch, connect the Cardputer over USB, and click Upload.
5. Put your MP3 files in the top folder of the microSD card and insert it before powering on.
Controls
Song list screen
• ; move up
• . move down
• , jump up 10 songs
• / jump down 10 songs
• Enter: play the selected song
• R: rescan the SD card for new MP3 files
• L: change theme
• G / V: volume up / down
Now Playing screen
• P: pause or resume
• N: next song
• B: previous song
• Q: back to the song list
• L: change theme
• G / V: volume up / down
Notes
• Filenames longer than 63 characters are skipped, so keep them short.
• The scan can take a few seconds with a large library. The screen stays put until it finishes.
• The player creates a small hidden file called .songindex on the SD card. It is rebuilt on every scan and is safe to delete.
License
MIT License
