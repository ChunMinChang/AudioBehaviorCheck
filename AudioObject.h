#ifndef AUDIOOBJECT_H
#define AUDIOOBJECT_H

#include <CoreAudio/AudioHardware.h>
#include <CoreAudio/AudioHardwareBase.h>
#include <vector>

using std::vector;

class AudioObject
{
public:
  AudioObject(AudioObjectID id, AudioClassID classId);
  ~AudioObject();

  bool HasProperty(const AudioObjectPropertyAddress* address) const;
  bool IsPropertySettable(const AudioObjectPropertyAddress* address) const;

  template<typename T>
  OSStatus GetPropertyData(const AudioObjectPropertyAddress* address, T* data) const {
    return GetPropertyData(_id, address, data);
  }

  OSStatus GetPropertyDataSize(const AudioObjectPropertyAddress* address,
                               UInt32 *size) const {
    return GetPropertyDataSize(_id, address, size);
  }

  template<typename T>
  OSStatus GetPropertyArray(const AudioObjectPropertyAddress* address,
                            vector<T>* array) const {
    return GetPropertyDataSize(_id, address, array);
  }

  template<typename T>
  OSStatus SetPropertyData(const AudioObjectPropertyAddress *address,
                           const T* data) const {
    return AudioObjectSetPropertyData(_id, address, 0, nullptr, sizeof(T),
                                      static_cast<const void*>(data));
  }

private:
  static const AudioObjectID UNKNOWNID = kAudioObjectUnknown;
  static const OSStatus STATUS_OK = kAudioHardwareNoError;

  AudioObjectID _id;
  AudioClassID _classId;

  bool IsClass(AudioClassID classId) const;

  template<typename T>
  static OSStatus GetPropertyData(AudioObjectID id,
                                  const AudioObjectPropertyAddress* address,
                                  T* data) {
    UInt32 size = sizeof(T);
    return AudioObjectGetPropertyData(id, address, 0, nullptr, &size,
                                      static_cast<void*>(data));
  }

  static OSStatus GetPropertyDataSize(AudioObjectID id,
                                      const AudioObjectPropertyAddress* address,
                                      UInt32 *size) {
    return AudioObjectGetPropertyDataSize(id, address, 0, nullptr, size);
  }

  template<typename T>
  static OSStatus GetPropertyArray(AudioObjectID id,
                                   const AudioObjectPropertyAddress* address,
                                   vector<T>* array) {
    UInt32 size = 0;
    OSStatus s = GetPropertyDataSize(id, address, &size);
    if (s != STATUS_OK) {
      return s;
    }
    if (!size) {
      return s; // TODO: Maybe throw an error instead.
    }
    vector<T> data(size / sizeof(T));
    s = AudioObjectGetPropertyData(id, address, 0, nullptr, &size,
                                   static_cast<void*>(data.data()));
    if (s == STATUS_OK) {
      *array = data;
    }
    return s;
  }

  template<typename T>
  static OSStatus SetPropertyData(AudioObjectID id,
                                  const AudioObjectPropertyAddress *address,
                                  const T* data) {
    return AudioObjectSetPropertyData(id, address, 0, nullptr, sizeof(T),
                                      static_cast<const void*>(data));
  }
};

#endif // #ifndef AUDIOOBJECT_H