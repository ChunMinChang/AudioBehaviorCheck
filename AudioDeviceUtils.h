#ifndef AUDIODEVICEUTILS_H
#define AUDIODEVICEUTILS_H

#include <CoreAudio/CoreAudio.h>

#include <string>
#include <vector>

using std::vector;
using std::string;

class AudioDeviceUtils
{
public:
  static vector<AudioDeviceID> GetOutputDevicesID();
  static vector<AudioDeviceID> GetInputDevicesID();
  static vector<AudioDeviceID> GetDevicesID(bool aIsInput);
  static string GetDeviceName(AudioDeviceID aID);
  static vector<string> GetDevicesName(vector<AudioDeviceID> aIDs);
};

#endif // #ifndef AUDIODEVICEUTILS_H