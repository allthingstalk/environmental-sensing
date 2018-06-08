#ifndef ATT_SDS011_h
#define ATT_SDS011_h

#include "Arduino.h"
#include "NeoSWSerial.h"

#define SDS011_RX_DATA_LENGHT 10
#define SDS011_RX_BUFFERS 2

enum SDS011_Measurement_Resposne
{
  packetChecksumError = -2,
  packetIdentifierError = -1,
  packetNotComplete = 0,
  packetValid = 1
};

class ATT_SDS011
{
  public:
    ATT_SDS011(uint8_t txPin, uint8_t rxPin);

    int pm25;
    int pm10;

    void startMeasurement();
    void stopMeasurement();
    SDS011_Measurement_Resposne getMeasurement();

    //static void handleRxChar( uint8_t c);

  private:
    NeoSWSerial _swSerial;
};


#endif
