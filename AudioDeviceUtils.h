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
  static string GetDeviceName(AudioObjectID aID);
  static bool IsInput(AudioObjectID aId);
  static bool IsOutput(AudioObjectID aId);
  static bool SetDefaultDevice(AudioObjectID aId, bool aInput);
  static vector<AudioObjectID> GetAllDeviceIds();
  static vector<AudioObjectID> GetDeviceIds(bool aInput);
  // TODO: This should move to other module!
  //       We should only do simple get/set method here.
  static bool ChangeDefaultDevice(bool aInput);
};

#endif // #ifndef AUDIODEVICEUTILS_H