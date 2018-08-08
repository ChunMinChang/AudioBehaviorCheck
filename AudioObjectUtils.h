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
  static bool IsInScope(AudioObjectID aId, Scope aScope);
  static string GetDeviceName(AudioObjectID aId);
  static UInt32 GetDeviceSource(AudioObjectID aId, Scope aScope);
  static string GetDeviceSourceName(AudioObjectID aId, Scope aScope,
                                    UInt32 aSource);
  // TODO: Validate the AudioObjectID and scope by ourselves since the
  //       underlying native API always return noErr. See comment in its
  //       implementation.
  static bool SetDefaultDevice(AudioObjectID aId, Scope aScope);
  // NOTE: It's impossible to get input-only or output-only devices in one API
  //       call. Apple doesn't provide that API. To get input-only or
  //       output-only devices, you need to get all the devices first ans then
  //       check if they are input or output ony by one.
  static vector<AudioObjectID> GetAllDeviceIds();
  // TODO: Consider if we should move following APIs to other module since they
  //       are rather higher level, that depends on the above low level APIs.
  //       When we write unit tests for these APIs, they work only when their
  //       base APIs work. However, the API users shouldn't know these
  //       dependences.
  static vector<AudioObjectID> GetDeviceIds(Scope aScope);
  static string GetDeviceLabel(AudioObjectID aId, Scope aScope);

private:
  static UInt32 GetNumberOfStreams(AudioObjectID aId, Scope aScope);
  static string CFStringRefToUTF8(CFStringRef aString);

  template<typename T>
  static OSStatus GetPropertyData(AudioObjectID aId,
                                  const AudioObjectPropertyAddress* address,
                                  T* data) {
    UInt32 size = sizeof(T);
    return AudioObjectGetPropertyData(aId, address, 0, nullptr, &size,
                                      static_cast<void*>(data));
  }

  template<typename T>
  static OSStatus GetPropertyArray(AudioObjectID aId,
                                   const AudioObjectPropertyAddress* address,
                                   vector<T>* array) {
    UInt32 size = 0;
    OSStatus r = GetPropertyDataSize(aId, address, &size);
    if (r != kAudioHardwareNoError) {
      return r;
    }
    // TDDO: what if size is 0 ?
    vector<T> data(size / sizeof(T));
    r = AudioObjectGetPropertyData(aId, address, 0, nullptr, &size,
                                   static_cast<void*>(data.data()));
    if (r == kAudioHardwareNoError) {
      *array = data;
    }
    return r;
  }

  static OSStatus GetPropertyDataSize(AudioObjectID aId,
                                      const AudioObjectPropertyAddress* address,
                                      UInt32 *size) {
    return AudioObjectGetPropertyDataSize(aId, address, 0, nullptr, size);
  }

  template<typename T>
  static OSStatus SetPropertyData(AudioObjectID aId,
                                  const AudioObjectPropertyAddress *address,
                                  const T* data) {
    return AudioObjectSetPropertyData(aId, address, 0, nullptr, sizeof(T),
                                      static_cast<const void*>(data));
  }
};

#endif // #ifndef AUDIOOBJECTUTILS_H