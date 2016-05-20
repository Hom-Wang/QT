/*====================================================================================================*/
/*====================================================================================================*/
#include "drivers\stm32f4_system.h"

#include "app_kSerial.h"
/*====================================================================================================*/
/*====================================================================================================*/
static ptrSerial pSerialFunc = NULL;
/*====================================================================================================*/
/*====================================================================================================*
**函數 : kSerial_calChecksum
**功能 : Set Serial Function
**輸入 : None
**輸出 : None
**使用 : kSerial_Config(Serial_SendByte);
**====================================================================================================*/
/*====================================================================================================*/
void kSerial_Config( ptrSerial pSerial )
{
  pSerialFunc = pSerial;
}
/*====================================================================================================*/
/*====================================================================================================*
**函數 : kSerial_calChecksum
**功能 : kSerial Cal Checksum
**輸入 : *data, dataLens
**輸出 : None
**使用 : kSerial_calChecksum(packet + 2, lens);;
**====================================================================================================*/
/*====================================================================================================*/
static uint16_t kSerial_calChecksum( uint8_t *data, const uint8_t dataLens )
{
  uint8_t count = 0;
  uint32_t checkSum = 0;

  for(count = 0; count < dataLens; count++)
    checkSum += (data[count] << 8) | (data[count + 1]);

  return (uint16_t)((dataLens & 0x01) ? checkSum : checkSum ^ (uint32_t)data[count]);
}
/*====================================================================================================*/
/*====================================================================================================*
**函數 : kSerial_sendData
**功能 : kSerial Send Data
**輸入 : *signalData, type, lens
**輸出 : None
**使用 : kSerial_sendData(signalData, INT16, 12);
**====================================================================================================*/
/*====================================================================================================*/
void kSerial_sendData( void *signalData, uint8_t type, uint8_t lens )
{
  const uint8_t dataLens = lens << ((type & 0x60) >> 5);

  uint8_t packet[32] = {0};           // tmpData lens >= 2 * lens + 4
  uint8_t *pPacket = packet;
  uint8_t packetLens = dataLens + 6;  // all packet bytes
  uint16_t checkSum = 0;

  packet[0] = 'S';                                      // 'S' - Signal
  packet[1] = type | (dataLens & 0x1F);                 // data type & lens
  for(uint8_t count = 0; count < dataLens; count++)     // data
    packet[count + 2] = ((uint8_t*)signalData)[count];
  checkSum = kSerial_calChecksum(packet + 2, lens);
  packet[packetLens - 4] = Byte8H(checkSum);            // checksum H
  packet[packetLens - 3] = Byte8L(checkSum);            // checksum L
  packet[packetLens - 2] = '\r';                        // 'r'
  packet[packetLens - 1] = '\n';                        // 'n'

  do {
    pSerialFunc(*pPacket++);
  } while(--packetLens);
}
/*====================================================================================================*/
/*====================================================================================================*/
