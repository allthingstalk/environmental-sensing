#include "ATT_SDS011.h"

//volatile variables for the interrupt
uint8_t rxData[SDS011_RX_BUFFERS][SDS011_RX_DATA_LENGHT];
uint8_t rxDataIndex;
uint8_t rxBufferIndex;
uint8_t parsingBufferIndex;
uint8_t parsingDataIndex;
uint8_t checksum;

static void handleRxChar( uint8_t c)
{
  rxData[rxBufferIndex][rxDataIndex] = c;
  rxDataIndex++;
}

ATT_SDS011::ATT_SDS011(uint8_t rxPin, uint8_t txPin):_swSerial(rxPin, txPin)
{
  _swSerial.begin(9600);
  _swSerial.ignore(); //ignore until data needs to be received

  rxDataIndex = 0;
  rxBufferIndex = 0;
  parsingBufferIndex = 0;
  parsingDataIndex = 0;
  checksum = 0;
}




void ATT_SDS011::startMeasurement()
{
  //start the serial port
  _swSerial.attachInterrupt(handleRxChar);
  _swSerial.listen();
}

void ATT_SDS011::stopMeasurement()
{
  _swSerial.ignore();
  _swSerial.detachInterrupt();

  //flush all buffers & index to restart clean
  rxDataIndex = 0;
  rxBufferIndex = 0;
  parsingBufferIndex = 0;
  parsingDataIndex = 0;
  for(int i = 0; i < 10; i++)
  {
    rxData[0][i] = 0;
    rxData[1][i] = 0;
  }
}

SDS011_Measurement_Resposne ATT_SDS011::getMeasurement()
{
  if(rxDataIndex >= SDS011_RX_DATA_LENGHT)
  {
    rxDataIndex = 0; //reset the receiving index
    parsingDataIndex = 0;
    //toggle the buffers
    if(rxBufferIndex == 0)
    {
      rxBufferIndex = 1 ;
      parsingBufferIndex = 0;
    }
    else
    {
      rxBufferIndex = 0 ;
      parsingBufferIndex = 1;
    }

    if(rxData[parsingBufferIndex][0] == 0xAA && rxData[parsingBufferIndex][1] == 0xC0 && rxData[parsingBufferIndex][9] == 0xAB)
    {
      //calculate the checksum
      checksum = 0;
      for (int i = 2; i < 8; i++)
      {
        checksum += rxData[parsingBufferIndex][i];
      }

      if(checksum == rxData[parsingBufferIndex][8])
      {
        pm25 = int(rxData[parsingBufferIndex][3] <<8);
        pm25 |= rxData[parsingBufferIndex][2];

        pm10 = int(rxData[parsingBufferIndex][5] <<8);
        pm10 |= rxData[parsingBufferIndex][4];

        pm25 /= 10;
        pm10/= 10;

       }
       else return packetChecksumError;
    }
    else return packetIdentifierError;
  } else return packetNotComplete;

  return packetValid;


}
