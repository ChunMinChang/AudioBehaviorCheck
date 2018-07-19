#include "AudioDeviceListener.h"
#include <iostream> // for std::cout, std::endl

/* DeviceChangeCallback */
void OnDeviceChanged() {
  std::cout << "Device Changed!" << std::endl;
}

int main()
{
  AudioDeviceListener adl(&OnDeviceChanged);
  std::cout << "Try changing your default audio devies"
                " or un/plugging your audio devices."
                " Press Enter to exit." << std::endl;
  std::cin.ignore();
  return 0;
}