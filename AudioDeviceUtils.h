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
  static vector<AudioObjectID> GetAllDeviceIDs();
  static string GetDeviceName(AudioObjectID aID);
  static AudioObjectID GetDefaultDeviceID(bool aInput);
  static bool IsInput(AudioObjectID aId);
  static bool IsOutput(AudioObjectID aId);
};

#endif // #ifndef AUDIODEVICEUTILS_H