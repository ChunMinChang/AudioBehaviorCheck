#ifndef AUDIOOBJECTUTILS_H
#define AUDIOOBJECTUTILS_H

#include <CoreAudio/CoreAudio.h>
#include <string>
#include <vector>

using std::vector;
using std::string;

// Provide low-level APIs to get device-related information.
class AudioObjectUtils
{
public:
  // TODO: Maybe we should move this enum out since other module may also
  //       need the scope.
  enum Scope: bool {
    Input = true,
    Output = false
  };
  static AudioObjectID GetDefaultDeviceId(Scope aScope);
  static string GetDeviceName(AudioObjectID aId);
  static UInt32 GetDeviceSource(AudioObjectID aId, Scope aScope);
  static string GetDeviceSourceName(AudioObjectID aId, Scope aScope,
                                    UInt32 aSource);
  static bool IsInput(AudioObjectID aId);
  static bool IsOutput(AudioObjectID aId);
  // TODO: Validate the AudioObjectID and scope by ourselves since the
  //       underlying native API always return noErr. See comment in its
  //       implementation.
  static bool SetDefaultDevice(AudioObjectID aId, Scope aScope);
  // NOTE: It's impossible to get input-only or output-only devices in one API
  //       call. Apple doesn't provide that API. To get input-only or
  //       output-only devices, you need to get all the devices first ans then
  //       check if they are input or output ony by one.
  static vector<AudioObjectID> GetAllDeviceIds();
  // TODO: Consider if we should move this API to other module since it's
  //       rather higher level, that depends on the above low level APIs.
  //       When we write unit test for this API, it works only when its base
  //       APIs works. However, the API users should not know this dependence.
  static vector<AudioObjectID> GetDeviceIds(Scope aScope);
private:
  static UInt32 GetNumberOfStreams(AudioObjectID aId, Scope aScope);
};

#endif // #ifndef AUDIOOBJECTUTILS_H