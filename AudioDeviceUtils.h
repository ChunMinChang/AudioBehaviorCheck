#ifndef AUDIODEVICEUTILS_H
#define AUDIODEVICEUTILS_H

#include <string>
#include <vector>

#include <CoreAudio/CoreAudio.h>

using std::vector;
using std::string;

class AudioDeviceUtils
{
public:
  static AudioObjectID GetDefaultDeviceId(bool aInput);
  static vector<AudioObjectID> GetAllDeviceIds();
  static bool IsInput(AudioObjectID aId);
  static bool IsOutput(AudioObjectID aId);
  static string GetDeviceName(AudioObjectID aID);
};

#endif // #ifndef AUDIODEVICEUTILS_H