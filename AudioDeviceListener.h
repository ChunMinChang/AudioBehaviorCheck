#ifndef AUDIODEVICELISTENER_H
#define AUDIODEVICELISTENER_H

#include <memory> // std::unique_ptr

#include <CoreAudio/AudioHardware.h>

typedef void (* DeviceChangeCallback)();

class AudioDeviceListener {
public:
  AudioDeviceListener(DeviceChangeCallback aCallbck);
  ~AudioDeviceListener();

private:
  class PropertyListener;

  bool AddPropertyListener(PropertyListener* aListener);
  bool RemovePropertyListener(PropertyListener* aListener);
  bool RegisterListener();
  static OSStatus OnEvent(AudioObjectID aObject,
                          UInt32 aNumAddresses,
                          const AudioObjectPropertyAddress aAddresses[],
                          void* aData);
  
  std::unique_ptr<PropertyListener> mDefaultOutputListener;
  std::unique_ptr<PropertyListener> mDefaultInputListener;
  std::unique_ptr<PropertyListener> mDeviceListener;
};

#endif // #ifndef AUDIODEVICELISTENER_H