#ifndef softI2C_h
#define softI2C_h

#include <Arduino.h>
#include <Wire.h>

// Transmission status error, the return value of endTransmission()
#define softI2C_NO_ERROR       0
#define softI2C_BUFFER_FULL    1
#define softI2C_ADDRESS_NACK   2
#define softI2C_DATA_NACK      3
#define softI2C_OTHER          4

#define softI2C_BUFSIZE 32        // same as buffer size of Arduino Wire library


class softI2C : public TwoWire
{
public:
  softI2C();
  softI2C(uint8_t sdaPin, uint8_t sclPin, boolean pullups = true, boolean detectClockStretch = true);
  ~softI2C();
  void init();//(uint8_t sdaPin, uint8_t sclPin, boolean pullups = true, boolean detectClockStretch = true);
  void end();
  
  void begin();

  // Generate compile error when slave mode begin(address) is used
  void __attribute__ ((error("I2C/TWI Slave mode is not supported by the softI2C library"))) begin(uint8_t addr);
  void __attribute__ ((error("I2C/TWI Slave mode is not supported by the softI2C library"))) begin(int addr);

  void setClock(uint32_t clock);
  void beginTransmission(uint8_t address);
  void beginTransmission(int address);
  uint8_t endTransmission(boolean sendStop = true);
  uint8_t requestFrom(uint8_t address, uint8_t size, boolean sendStop = true);
  uint8_t requestFrom(int address, int size, boolean sendStop = true);
  size_t write(uint8_t data);
  size_t write(const uint8_t*, uint8_t size);
  size_t write(char* data);
  int available(void);
  int read(void);
  int readBytes(uint8_t* buf, uint8_t size);
  int readBytes(char * buf, uint8_t size);
  int readBytes(char * buf, int size);
  int peek(void);
  void setTimeout(long timeout);  // timeout to wait for the I2C bus
  void printStatus(Print& Ser);   // print information using specified object class

  uint8_t _sdaPin;
  uint8_t _sclPin;
  
private:
  // per object data

  uint8_t _sdaBitMask;
  uint8_t _sclBitMask;
  
  volatile uint8_t *_sdaPortReg;
  volatile uint8_t *_sclPortReg;
  volatile uint8_t *_sdaDirReg;
  volatile uint8_t *_sclDirReg;
  volatile uint8_t *_sdaPinReg;
  volatile uint8_t *_sclPinReg;

  uint8_t _transmission;      // transmission status, returned by endTransmission(). 0 is no error.
  uint16_t _i2cdelay;         // delay in micro seconds for sda and scl bits.
  boolean _pullups;           // using the internal pullups or not
  boolean _stretch;           // should code handle clock stretching by the slave or not.
  unsigned long _timeout;     // timeout in ms. When waiting for a clock pulse stretch. 2017, Fix issue #6

  uint8_t rxBuf[softI2C_BUFSIZE];   // buffer inside this class, a buffer per softI2C.
  uint8_t rxBufPut;           // index to rxBuf, just after the last valid byte.
  uint8_t rxBufGet;           // index to rxBuf, the first new to be read byte.
  
  // private methods
  
  void i2c_writebit( uint8_t c );
  uint8_t i2c_readbit(void);
  void i2c_init(void);
  boolean i2c_start(void);
  void i2c_repstart(void);
  void i2c_stop(void);
  uint8_t i2c_write(uint8_t c);
  uint8_t i2c_read(boolean ack);
};

#endif // softI2C_h
