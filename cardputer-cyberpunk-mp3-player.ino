#include "M5Cardputer.h"
#include <AudioOutput.h>
#include <AudioFileSourceSD.h>
#include <AudioGeneratorMP3.h>
#include "SD.h"
#include "SPI.h"
#include "esp_system.h"

SPIClass SDSPI(HSPI);

#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define BLACKC 0x0000

class AudioOutputM5Speaker : public AudioOutput {
 public:
  AudioOutputM5Speaker(m5::Speaker_Class* m5sound, uint8_t virtual_sound_channel = 0) {
    _m5sound = m5sound;
    _virtual_ch = virtual_sound_channel;
    for (int i = 0; i < 3; i++) {
      _tri_buffer[i] = (int16_t*)malloc(tri_buf_size * sizeof(int16_t));
    }
  }
  virtual ~AudioOutputM5Speaker(void) {
    for (int i = 0; i < 3; i++) if (_tri_buffer[i]) free(_tri_buffer[i]);
  }
  virtual bool begin(void) override { return true; }
  virtual bool ConsumeSample(int16_t sample[2]) override {
    if (_tri_buffer_index < tri_buf_size - 1) {
      _tri_buffer[_tri_index][_tri_buffer_index] = sample[0];
      _tri_buffer[_tri_index][_tri_buffer_index + 1] = sample[1];
      _tri_buffer_index += 2;
      return true;
    }
    flush();
    return false;
  }
  virtual void flush(void) {
    if (_tri_buffer_index) {
      _m5sound->playRaw(_tri_buffer[_tri_index], _tri_buffer_index, hertz, true, 1, _virtual_ch);
      _tri_index = _tri_index < 2 ? _tri_index + 1 : 0;
      _tri_buffer_index = 0;
    }
  }
  virtual bool SetRate(int hz) override { hertz = hz; return true; }

 protected:
  m5::Speaker_Class* _m5sound;
  uint8_t _virtual_ch;
  int hertz = 44100;
  int16_t* _tri_buffer[3] = {nullptr, nullptr, nullptr};
  size_t tri_buf_size = 1024;
  int _tri_index = 0;
  size_t _tri_buffer_index = 0;
};

AudioFileSourceSD* file = nullptr;
AudioGeneratorMP3* mp3 = nullptr;
AudioOutputM5Speaker* out;

String songs[50];
int songCount = 0;
int selected = 0;
int topOfList = 0;
bool isPlaying = false;
bool inPlayerScreen = false;
const int visibleRows = 6;

unsigned long songStartMillis = 0;
unsigned long pausedElapsed = 0;
int lastBatteryCheck = -1;
int cachedBattery = 0;
int screenW = 240;
int screenH = 135;

const int numBars = 16;
int barHeights[numBars];
int barTargets[numBars];

void drawBorder() {
  M5Cardputer.Display.drawRect(0, 0, screenW, screenH, CYAN);
  M5Cardputer.Display.drawRect(1, 1, screenW - 2, screenH - 2, CYAN);
}

void drawBattery() {
  unsigned long now = millis();
  if (lastBatteryCheck < 0 || now - (unsigned long)lastBatteryCheck > 2000) {
    cachedBattery = M5Cardputer.Power.getBatteryLevel();
    lastBatteryCheck = now;
  }
  M5Cardputer.Display.setTextColor(CYAN, BLACKC);
  String batStr = String(cachedBattery) + "%";
  int x = screenW - 6 - (batStr.length() * 6);
  M5Cardputer.Display.drawString(batStr, x, 5);
}

void drawList() {
  M5Cardputer.Display.fillScreen(BLACKC);
  drawBorder();
  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(MAGENTA, BLACKC);
  M5Cardputer.Display.drawString("MP3 PLAYER // v2", 6, 5);
  drawBattery();
  M5Cardputer.Display.drawFastHLine(4, 16, screenW - 8, CYAN);

  if (songCount == 0) {
    M5Cardputer.Display.setTextColor(CYAN, BLACKC);
    M5Cardputer.Display.drawString("No MP3 files found", 6, 26);
  } else {
    for (int i = 0; i < visibleRows; i++) {
      int idx = topOfList + i;
      if (idx >= songCount) break;
      int y = 20 + i * 14;
      if (idx == selected) {
        M5Cardputer.Display.fillRect(4, y - 1, screenW - 8, 13, CYAN);
        M5Cardputer.Display.setTextColor(BLACKC, CYAN);
      } else {
        M5Cardputer.Display.setTextColor(CYAN, BLACKC);
      }
      String name = songs[idx];
      if (name.length() > 32) name = name.substring(0, 32);
      M5Cardputer.Display.drawString(name, 8, y);
    }
  }

  M5Cardputer.Display.drawFastHLine(4, screenH - 16, screenW - 8, CYAN);
  M5Cardputer.Display.setTextColor(MAGENTA, BLACKC);
  M5Cardputer.Display.drawString(";/.=move  ENTER=play", 6, screenH - 12);
}

String formatTime(unsigned long ms) {
  unsigned long totalSec = ms / 1000;
  unsigned long mins = totalSec / 60;
  unsigned long secs = totalSec % 60;
  String o = String(mins) + ":";
  if (secs < 10) o += "0";
  o += String(secs);
  return o;
}

int barAreaX, barAreaY, barAreaW, barAreaH;

void drawPlayerScreen(String status) {
  M5Cardputer.Display.fillScreen(BLACKC);
  drawBorder();
  M5Cardputer.Display.setTextSize(1);
  M5Cardputer.Display.setTextColor(MAGENTA, BLACKC);
  M5Cardputer.Display.drawString("NOW PLAYING", 6, 5);
  drawBattery();
  M5Cardputer.Display.drawFastHLine(4, 16, screenW - 8, CYAN);

  String name = songs[selected];
  if (name.length() > 32) name = name.substring(0, 32);
  M5Cardputer.Display.setTextColor(MAGENTA, BLACKC);
  M5Cardputer.Display.drawString(name, 8, 22);

  M5Cardputer.Display.setTextColor(CYAN, BLACKC);
  M5Cardputer.Display.drawString(status, 8, 36);

  barAreaX = 8;
  barAreaY = 50;
  barAreaW = screenW - 16;
  barAreaH = 36;
  M5Cardputer.Display.drawRect(barAreaX - 2, barAreaY - 2, barAreaW + 4, barAreaH + 4, CYAN);

  for (int i = 0; i < numBars; i++) {
    barHeights[i] = 2;
    barTargets[i] = 2;
  }

  M5Cardputer.Display.drawFastHLine(4, screenH - 30, screenW - 8, CYAN);
  M5Cardputer.Display.setTextColor(MAGENTA, BLACKC);
  M5Cardputer.Display.drawString("P:pause N:next B:prev Q:list", 6, screenH - 26);
  M5Cardputer.Display.drawFastHLine(4, screenH - 16, screenW - 8, CYAN);
}

void updatePlayerTime() {
  if (!inPlayerScreen) return;
  unsigned long elapsed = isPlaying ? (millis() - songStartMillis + pausedElapsed) : pausedElapsed;
  M5Cardputer.Display.fillRect(90, 36, 60, 12, BLACKC);
  M5Cardputer.Display.setTextColor(CYAN, BLACKC);
  M5Cardputer.Display.drawString(formatTime(elapsed), 90, 36);
}

void updateVisualizer() {
  if (!inPlayerScreen) return;
  int barW = barAreaW / numBars;
  for (int i = 0; i < numBars; i++) {
    if (isPlaying) {
      if (random(0, 100) < 40) {
        barTargets[i] = random(2, barAreaH);
      }
      if (barHeights[i] < barTargets[i]) {
        barHeights[i] += 4;
        if (barHeights[i] > barTargets[i]) barHeights[i] = barTargets[i];
      } else if (barHeights[i] > barTargets[i]) {
        barHeights[i] -= 2;
        if (barHeights[i] < barTargets[i]) barHeights[i] = barTargets[i];
      }
    } else {
      barTargets[i] = 2;
      if (barHeights[i] > 2) barHeights[i] -= 2;
    }

    int x = barAreaX + i * barW;
    M5Cardputer.Display.fillRect(x, barAreaY, barW - 1, barAreaH, BLACKC);
    int h = barHeights[i];
    M5Cardputer.Display.fillRect(x, barAreaY + (barAreaH - h), barW - 1, h, MAGENTA);
  }
}

void scanSongs() {
  songCount = 0;
  File root = SD.open("/");
  File entry = root.openNextFile();
  while (entry && songCount < 50) {
    String name = String(entry.name());
    if (!entry.isDirectory() && name.endsWith(".mp3")) {
      songs[songCount] = name;
      songCount++;
    }
    entry = root.openNextFile();
  }
}

void startSong(int idx) {
  if (mp3) {
    if (mp3->isRunning()) mp3->stop();
    delete mp3;
    mp3 = nullptr;
  }
  if (file) { delete file; file = nullptr; }

  String path = "/" + songs[idx];
  file = new AudioFileSourceSD(path.c_str());
  mp3 = new AudioGeneratorMP3();
  bool ok = mp3->begin(file, out);
  Serial.printf("mp3 begin result: %d for %s\n", ok, path.c_str());
  isPlaying = true;
  inPlayerScreen = true;
  songStartMillis = millis();
  pausedElapsed = 0;
  drawPlayerScreen("Playing...");
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Booting player...");

  esp_reset_reason_t reason = esp_reset_reason();
  Serial.printf("Reset reason code: %d\n", (int)reason);

  auto cfg = M5.config();
  M5Cardputer.begin(cfg, true);
  M5Cardputer.Speaker.begin();
  M5Cardputer.Speaker.setVolume(255);
  M5Cardputer.Display.setRotation(1);

  screenW = M5Cardputer.Display.width();
  screenH = M5Cardputer.Display.height();

  randomSeed(analogRead(0));

  SDSPI.begin(40, 39, 14, 12);
  bool sdok = SD.begin(12, SDSPI, 25000000);
  Serial.printf("SD begin: %d\n", sdok);

  out = new AudioOutputM5Speaker(&M5Cardputer.Speaker, 0);

  scanSongs();
  Serial.printf("Found %d songs\n", songCount);
  drawList();
}

unsigned long lastTimeUpdate = 0;
unsigned long lastVisUpdate = 0;

void loop() {
  M5Cardputer.update();

  if (isPlaying && mp3 && mp3->isRunning()) {
    bool ok = mp3->loop();
    if (!ok) {
      mp3->stop();
      isPlaying = false;
      if (inPlayerScreen) drawPlayerScreen("Finished");
    }
  }

  if (inPlayerScreen && millis() - lastTimeUpdate > 500) {
    updatePlayerTime();
    lastTimeUpdate = millis();
  }

  if (inPlayerScreen && millis() - lastVisUpdate > 80) {
    updateVisualizer();
    lastVisUpdate = millis();
  }

  if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) {
    Keyboard_Class::KeysState ks = M5Cardputer.Keyboard.keysState();

    if (!inPlayerScreen) {
      for (auto c : ks.word) {
        if (c == ';') {
          if (selected > 0) selected--;
          if (selected < topOfList) topOfList = selected;
          drawList();
        } else if (c == '.') {
          if (selected < songCount - 1) selected++;
          if (selected >= topOfList + visibleRows) topOfList = selected - visibleRows + 1;
          drawList();
        }
      }
      if (ks.enter) {
        if (songCount > 0) startSong(selected);
      }
    } else {
      for (auto c : ks.word) {
        if (c == 'p' || c == 'P') {
          if (isPlaying) {
            isPlaying = false;
            pausedElapsed += millis() - songStartMillis;
            drawPlayerScreen("Paused");
          } else {
            isPlaying = true;
            songStartMillis = millis();
            drawPlayerScreen("Playing...");
          }
        } else if (c == 'n' || c == 'N') {
          if (selected < songCount - 1) {
            selected++;
            startSong(selected);
          }
        } else if (c == 'b' || c == 'B') {
          if (selected > 0) {
            selected--;
            startSong(selected);
          }
        } else if (c == 'q' || c == 'Q') {
          if (mp3 && mp3->isRunning()) mp3->stop();
          isPlaying = false;
          inPlayerScreen = false;
          drawList();
        }
      }
    }
  }
}