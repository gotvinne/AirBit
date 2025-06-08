#include "handController.h"
#include <MicroBit.h>

int RADIO_GROUP = 7;

/*
// Packet Spec:
// | 0              | 1 ... 4       | 5 ... 8           | 9 ... 28
// ----------------------------------------------------------------
// | packet type    | system time   | serial number     | payload
//
// Serial number defaults to 0 unless enabled by user

// payload: number (9 ... 12)
export const PACKET_TYPE_NUMBER = 0;
// payload: number (9 ... 12), name length (13), name (14 ... 26)
export const PACKET_TYPE_VALUE = 1;
// payload: string length (9), string (10 ... 28)
export const PACKET_TYPE_STRING = 2;
// payload: buffer length (9), buffer (10 ... 28)
export const PACKET_TYPE_BUFFER = 3;
// payload: number (9 ... 16)
export const PACKET_TYPE_DOUBLE = 4;
// payload: number (9 ... 16), name length (17), name (18 ... 26)
export const PACKET_TYPE_DOUBLE_VALUE = 5;
*/
static void sendValue(char name, int value) {
  uint8_t buf[32];
  int i = 0;

  // Packet type value
  buf[i++] = 0x01;

  // Timestamp (4 bytes)
  uint32_t timestamp = uBit.systemTime();
  memcpy(&buf[i], &timestamp, 4);
  i += 4;

  // Serial number (4 bytes)
  uint32_t serial = 0;
  memcpy(&buf[i], &serial, 4);
  i += 4;

  // Value (4 bytes, little-endian)
  memcpy(&buf[i], &value, 4);
  i += 4;

  // Name length
  buf[i++] = 1;
  memcpy(&buf[i], &name, 1);
  i += 1;

  // Send the packet
  uBit.radio.datagram.send(buf, i);
}

void initRadio() {
  uBit.radio.enable();
  uBit.radio.setGroup(RADIO_GROUP);
}

void transmittData() {
  sendValue('P', pitch);
  sendValue('A', armed);
  sendValue('R', roll);
  sendValue('T', throttle);
}
