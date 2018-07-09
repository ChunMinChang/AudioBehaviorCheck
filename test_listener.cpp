#include "AudioDeviceListener.h"
#include <iostream>

/* DeviceChangeCallback */
void OnDeviceChanged() {
  std::cout << "Device Changed!" << std::endl;
}

int main()
{
  AudioDeviceListener adl(&OnDeviceChanged);
  std::cout << "Try changing your default audio devies"
                "or un/plugging your audio devices."
                "Enter anythin to exit." << std::endl;
  std::cin.ignore();
  return 0;
}