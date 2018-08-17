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
  static AudioObjectID GetDefaultDeviceId(Scope scope);
  static bool IsInScope(AudioObjectID id, Scope scope);
  static string GetDeviceName(AudioObjectID id);
  static UInt32 GetDeviceSource(AudioObjectID id, Scope scope);
  static string GetDeviceSourceName(AudioObjectID id, Scope scope,
                                    UInt32 source);
  static bool SetDefaultDevice(AudioObjectID id, Scope scope);
  static vector<AudioObjectID> GetAllDeviceIds();
  // NOTE: The following two APIs are rather higher level. They are implemented
  //       based on the above APIs.

  // NOTE: Apple has no API to get input-only or output-only devices. To do
  //       that, we need to get all the devices first ans then check if they
  //       are input or output ony by one.
  static vector<AudioObjectID> GetDeviceIds(Scope scope);
  static string GetDeviceLabel(AudioObjectID id, Scope scope);

private:
  static UInt32 GetNumberOfStreams(AudioObjectID id, Scope scope);
  static string CFStringRefToUTF8(CFStringRef stringRef);

  template<typename T>
  static OSStatus GetPropertyData(AudioObjectID id,
                                  const AudioObjectPropertyAddress* address,
                                  T* data) {
    UInt32 size = sizeof(T);
    return AudioObjectGetPropertyData(id, address, 0, nullptr, &size,
                                      static_cast<void*>(data));
  }

  template<typename T>
  static OSStatus GetPropertyArray(AudioObjectID id,
                                   const AudioObjectPropertyAddress* address,
                                   vector<T>* array) {
    UInt32 size = 0;
    OSStatus r = GetPropertyDataSize(id, address, &size);
    if (r != kAudioHardwareNoError) {
      return r;
    }
    if (!size) {
      return r; // TODO: Maybe throw an error instead.
    }
    vector<T> data(size / sizeof(T));
    r = AudioObjectGetPropertyData(id, address, 0, nullptr, &size,
                                   static_cast<void*>(data.data()));
    if (r == kAudioHardwareNoError) {
      *array = data;
    }
    return r;
  }

  static OSStatus GetPropertyDataSize(AudioObjectID id,
                                      const AudioObjectPropertyAddress* address,
                                      UInt32 *size) {
    return AudioObjectGetPropertyDataSize(id, address, 0, nullptr, size);
  }

  template<typename T>
  static OSStatus SetPropertyData(AudioObjectID id,
                                  const AudioObjectPropertyAddress *address,
                                  const T* data) {
    return AudioObjectSetPropertyData(id, address, 0, nullptr, sizeof(T),
                                      static_cast<const void*>(data));
  }
};

#endif // #ifndef AUDIOOBJECTUTILS_H