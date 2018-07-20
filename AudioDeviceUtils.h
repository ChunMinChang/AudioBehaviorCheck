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
  enum Scope: bool {
    Input = true,
    Output = false
  };
  static AudioObjectID GetDefaultDeviceId(Scope aScope);
  static string GetDeviceName(AudioObjectID aId);
  static UInt32 GetDeviceSource(AudioObjectID aId, Scope aScope);
  static bool IsInput(AudioObjectID aId);
  static bool IsOutput(AudioObjectID aId);
  static bool SetDefaultDevice(AudioObjectID aId, Scope aScope);
  // NOTE: It's impossible to get input-only or output-only devices in one API
  //       call. Apple doesn't provide that API. To get input-only or
  //       output-only devices, you need to get all the devices first ans then
  //       check if they are input or output ony by one.
  static vector<AudioObjectID> GetAllDeviceIds();
private:
  static UInt32 GetNumberOfStreams(AudioObjectID aId, Scope aScope);
};

#endif // #ifndef AUDIODEVICEUTILS_H