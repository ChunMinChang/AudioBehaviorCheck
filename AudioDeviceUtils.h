#ifndef AUDIODEVICEUTILS_H
#define AUDIODEVICEUTILS_H

#include <CoreAudio/CoreAudio.h>
#include <string>
#include <vector>

using std::vector;
using std::string;

// Provide low-level APIs to get device-related information.
class AudioDeviceUtils
{
public:
  static AudioObjectID GetDefaultDeviceId(bool aInput);
  static string GetDeviceName(AudioObjectID aID);
  static bool IsInput(AudioObjectID aId);
  static bool IsOutput(AudioObjectID aId);
  static bool SetDefaultDevice(AudioObjectID aId, bool aInput);
  // NOTE: It's impossible to get input-only or output-only devices in one API
  //       call. Apple doesn't provide that API(I've comfirmed with my friend
  //       who works in CoreAudio team.). If you want to get input-only or
  //       output-only devices, you need to get all the devices first ans then
  //       check them are input or output ony by one.
  static vector<AudioObjectID> GetAllDeviceIds();
};

#endif // #ifndef AUDIODEVICEUTILS_H