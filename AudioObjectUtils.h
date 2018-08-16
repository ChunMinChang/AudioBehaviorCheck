#ifndef AUDIOOBJECTUTILS_H
#define AUDIOOBJECTUTILS_H

#include <CoreAudio/AudioHardware.h>
#include <CoreAudio/AudioHardwareBase.h>
#include <string>
#include <vector>

using std::vector;
using std::string;

// Provide low-level APIs to get device-related information or set
// default input or output devices.
class AudioObjectUtils
{
public:
  // TODO: Maybe we should move this enum out since other module may also
  //       need the scope.
  enum Scope: bool {
    Input = true,
    Output = false
  };
  // NOTE: When there is no valid device, the underlying API will return
  //       kAudioObjectUnknown.
  static AudioObjectID GetDefaultDeviceId(Scope aScope);
  static bool IsInScope(AudioObjectID aId, Scope aScope);
  static string GetDeviceName(AudioObjectID aId);
  static UInt32 GetDeviceSource(AudioObjectID aId, Scope aScope);
  static string GetDeviceSourceName(AudioObjectID aId, Scope aScope,
                                    UInt32 aSource);
  static bool SetDefaultDevice(AudioObjectID aId, Scope aScope);
  static vector<AudioObjectID> GetAllDeviceIds();
  // NOTE: The following two APIs are rather higher level. They are implemented
  //       based on the above APIs.

  // NOTE: Apple has no API to get input-only or output-only devices. To do
  //       that, we need to get all the devices first ans then check if they
  //       are input or output ony by one.
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
    if (!size) {
      return r; // TODO: Maybe throw an error instead.
    }
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