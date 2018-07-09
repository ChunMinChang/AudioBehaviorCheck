#include <assert.h>

#include "AudioDeviceListener.h"

const AudioObjectPropertyAddress kDefaultOutputDeviceChangePropertyAddress = {
  kAudioHardwarePropertyDefaultOutputDevice,
  kAudioObjectPropertyScopeGlobal, 
  kAudioObjectPropertyElementMaster };

const AudioObjectPropertyAddress kDefaultInputDeviceChangePropertyAddress = {
  kAudioHardwarePropertyDefaultInputDevice,
  kAudioObjectPropertyScopeGlobal,
  kAudioObjectPropertyElementMaster };

const AudioObjectPropertyAddress kDevicesPropertyAddress = {
  kAudioHardwarePropertyDevices,
  kAudioObjectPropertyScopeGlobal,
  kAudioObjectPropertyElementMaster };

class AudioDeviceListener::PropertyListener {
public:
  PropertyListener(AudioObjectID aObjectId,
                   const AudioObjectPropertyAddress* aAddress,
                   DeviceChangeCallback aCallback)
    : mObjectId(aObjectId)
    , mAddress(aAddress)
    , mCallback(aCallback)
  {}

  ~PropertyListener() {};

  AudioObjectID getObjectId() const { return mObjectId; }
  const AudioObjectPropertyAddress* getProperty() const { return mAddress; }
  const DeviceChangeCallback& getCallback() const { return mCallback; }

private:
  AudioObjectID mObjectId;
  const AudioObjectPropertyAddress* mAddress;
  DeviceChangeCallback mCallback;
};

AudioDeviceListener::AudioDeviceListener(DeviceChangeCallback aCallbck)
{
  mDefaultOutputListener = std::make_unique<PropertyListener> (
    kAudioObjectSystemObject, &kDefaultOutputDeviceChangePropertyAddress,
    aCallbck);
  mDefaultInputListener = std::make_unique<PropertyListener> (
    kAudioObjectSystemObject, &kDefaultInputDeviceChangePropertyAddress,
    aCallbck);
  mDeviceListener = std::make_unique<PropertyListener> (
    kAudioObjectSystemObject, &kDevicesPropertyAddress,
    aCallbck);
  
  if (!AddPropertyListener(mDefaultOutputListener.get())) {
    mDefaultOutputListener.reset();
  }

  if (!AddPropertyListener(mDefaultInputListener.get())) {
    mDefaultInputListener.reset();
  }

  if (!AddPropertyListener(mDeviceListener.get())) {
    mDeviceListener.reset();
  }
}

AudioDeviceListener::~AudioDeviceListener()
{
  if (mDefaultInputListener) {
    RemovePropertyListener(mDefaultInputListener.get());
  }

  if (mDefaultInputListener) {
    RemovePropertyListener(mDefaultInputListener.get());
  }

  if (mDeviceListener) {
    RemovePropertyListener(mDeviceListener.get());
  }
}

bool AudioDeviceListener::AddPropertyListener(PropertyListener* aListener)
{
  return AudioObjectAddPropertyListener(aListener->getObjectId(),
    aListener->getProperty(), &OnEvent, aListener) == noErr;
}

bool AudioDeviceListener::RemovePropertyListener(PropertyListener* aListener)
{
  return AudioObjectRemovePropertyListener(aListener->getObjectId(),
    aListener->getProperty(), &OnEvent, aListener) == noErr;
}

/* static */ OSStatus
AudioDeviceListener::OnEvent(AudioObjectID aObject,
                             UInt32 aNumAddresses,
                             const AudioObjectPropertyAddress aAddresses[],
                             void* aData)
{
  PropertyListener* listener = static_cast<PropertyListener*>(aData);
  if (!listener || aObject != listener->getObjectId()) {
    return noErr;
  }

  for (UInt32 i = 0; i < aNumAddresses; ++i) {
    if (aAddresses[i].mSelector == listener->getProperty()->mSelector &&
        aAddresses[i].mScope == listener->getProperty()->mScope &&
        aAddresses[i].mElement == listener->getProperty()->mElement) {
      DeviceChangeCallback callback = listener->getCallback();
      callback();
      break;
    }
  }

  return noErr;
}