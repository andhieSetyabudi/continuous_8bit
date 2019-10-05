#include "Indicating.h"
#include "lang.h"
#include <EEPROM.h>
#include <FastCRC.h>
#include "Storage.h"

unsigned char Indicating::Translation::locale = 0;
char Indicating::Translation::buffer[65] = "";

const char* Indicating::Translation::get(int key) {
  strcpy_P(buffer, (const char*)pgm_read_word(&(trans[locale][key])));
  return buffer;
}

void Indicating::Translation::setup() {
  locale = Storage::Eeprom::getLocale();
  if (locale > 1) {
    locale = 0;
  }
}

bool Indicating::Translation::setLocale(unsigned char lang) {
  Storage::Eeprom::setLocale(lang);
  locale = lang;
  return true;
}
