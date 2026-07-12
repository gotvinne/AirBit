#pragma once

struct RadioPacketChars {
  char Armed;
  char Pitch;
  char Roll;
  char Throttle;
  char Yaw;

  constexpr RadioPacketChars()
      : Armed('A'), Pitch('P'), Roll('R'), Throttle('T'), Yaw('Y') {}
};

constexpr RadioPacketChars DefaultRadioPacketChars{};