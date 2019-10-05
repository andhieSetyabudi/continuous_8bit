#include "Config.h"
#include "System.h"

char System::Identity::id[9];
char System::Identity::idBuffer[9];

void System::Identity::setup(void) {
#ifdef DEVICE_ID
  strcpy_P(id, (const char *)F(DEVICE_ID));
  strcpy_P(idBuffer, (const char *)F(DEVICE_ID));
#else
  strcpy_P(id, (const char *)F(""));
  strcpy_P(idBuffer, (const char *)F(""));
#endif
}

void System::Identity::loop(void) {
  if (strcmp(id, idBuffer) != 0) {
    strcpy(id, idBuffer);
  }
}

void System::Identity::setId(const char id[]) {
  strcpy(idBuffer, id);
}
