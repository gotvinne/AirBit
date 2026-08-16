#include "receiver.h"
#include "../constants.h"
#include "flightController.h"

#include <MicroBit.h>

static RadioPacketChars radioPacketChars = DefaultRadioPacketChars;
const uint8_t PACKET_CHAR_INDEX = 14;
const uint8_t PACKET_VALUE_INDEX = 9;

static uint8_t packet[32];

// Reference variables
static int pitch = 0;
static int roll = 0;
static int throttle = 0;
static int yaw = 0;
static bool armed = false;

static void readRadioPacket() {
  uBit.radio.datagram.recv(packet, sizeof(packet));
  char packetChar = packet[PACKET_CHAR_INDEX];

  if (packetChar == radioPacketChars.Pitch) {
    memcpy(&pitch, &packet[PACKET_VALUE_INDEX], 4);
  } else if (packetChar == radioPacketChars.Armed) {
    int32_t value = 0;
    memcpy(&value, &packet[PACKET_VALUE_INDEX], 4);
    armed = value != 0;
  } else if (packetChar == radioPacketChars.Roll) {
    memcpy(&roll, &packet[PACKET_VALUE_INDEX], 4);
  } else if (packetChar == radioPacketChars.Throttle) {
    memcpy(&throttle, &packet[PACKET_VALUE_INDEX], 4);
  } else if (packetChar == radioPacketChars.Yaw) {
    memcpy(&yaw, &packet[PACKET_VALUE_INDEX], 4);
  }
}

void FlushRadioBuffer() {
  // sizeof returns the byte size of struct
  for (unsigned int i = 0; i < sizeof(radioPacketChars); i++) {
    readRadioPacket();
  }
}

bool IsDroneArmed() { return armed; }
int GetDroneThrottle() { return throttle; }
int GetDroneRoll() { return roll; }
int GetDronePitch() { return pitch; }
int GetDroneYaw() { return yaw; }
