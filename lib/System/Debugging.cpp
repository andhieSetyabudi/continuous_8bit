#include "Config.h"
#include "System.h"

bool System::Debugging::debug = SYSTEM_DEBUG;

void System::Debugging::setup(void) {
  pinMode(1,OUTPUT);
  pinMode(0,INPUT_PULLUP);
  char buf[16];
  SYSTEM_SERIAL.begin(SYSTEM_SERIAL_BAUDRATE);
  sprintf_P(buf,(const char*)F("Warming up\r\n"));
  print(buf);
  if (!debug) return;
  sprintf_P(buf,(const char*)F("Debug Mode\r\n"));
  print(buf);
}

void System::Debugging::loop(void) {
  if (!debug) 
  {
    
    while(SYSTEM_SERIAL.available()>0)
      SYSTEM_SERIAL.read();
    SYSTEM_SERIAL.flush();
    return;
  }
  SYSTEM_SERIAL.flush();
}

void System::Debugging::print(const char fmt[], ...) {
  if (!debug) return;
  char buffer[256];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);
  SYSTEM_SERIAL.print(buffer);
  SYSTEM_SERIAL.flush();
}

void System::Debugging::println(const char fmt[], ...) {
  if (!debug) return;
  char buffer[256];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);
  SYSTEM_SERIAL.println(buffer);
  SYSTEM_SERIAL.flush();
}
