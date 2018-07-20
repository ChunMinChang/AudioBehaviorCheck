#include "AudioDeviceUtils.h"
#include <CoreFoundation/CFString.h> // for CFStringXXX

const AudioObjectPropertyAddress kDevicesPropertyAddress = {
  kAudioHardwarePropertyDevices,
  kAudioObjectPropertyScopeGlobal,
  kAudioObjectPropertyElementMaster
};

const AudioObjectPropertyAddress kDeviceNamePropertyAddress = {
  kAudioObjectPropertyName,
  kAudioObjectPropertyScopeGlobal,
  kAudioObjectPropertyElementMaster
};

const AudioObjectPropertyAddress kDefaultOutputDevicePropertyAddress = {
  kAudioHardwarePropertyDefaultOutputDevice,
  kAudioObjectPropertyScopeGlobal,
  kAudioObjectPropertyElementMaster
};

const AudioObjectPropertyAddress kDefaultInputDevicePropertyAddress = {
  kAudioHardwarePropertyDefaultInputDevice,
  kAudioObjectPropertyScopeGlobal,
  kAudioObjectPropertyElementMaster
};

const AudioObjectPropertyAddress kInputDeviceStreamsPropertyAddress = {
  kAudioDevicePropertyStreams,
  kAudioObjectPropertyScopeInput,
  kAudioObjectPropertyElementMaster
};

const AudioObjectPropertyAddress kOutputDeviceStreamsPropertyAddress = {
  kAudioDevicePropertyStreams,
  kAudioObjectPropertyScopeOutput,
  kAudioObjectPropertyElementMaster
};

const AudioObjectPropertyAddress kInputDeviceSourcePropertyAddress = {
  kAudioDevicePropertyDataSource,
  kAudioObjectPropertyScopeInput,
  kAudioObjectPropertyElementMaster
};

const AudioObjectPropertyAddress kOutputDeviceSourcePropertyAddress = {
  kAudioDevicePropertyDataSource,
  kAudioObjectPropertyScopeOutput,
  kAudioObjectPropertyElementMaster
};

/* static */ AudioObjectID
AudioDeviceUtils::GetDefaultDeviceId(Scope aScope)
{
  AudioObjectID id;
  UInt32 size = sizeof(id);
  const AudioObjectPropertyAddress* address = aScope == Input?
    &kDefaultInputDevicePropertyAddress : &kDefaultOutputDevicePropertyAddress;
  OSStatus r = AudioObjectGetPropertyData(kAudioObjectSystemObject, address,
                                          0, 0, &size, &id);
  return r == noErr ? id : kAudioObjectUnknown;
}

static string
CFStringRefToUTF8(CFStringRef aString)
{
  string s;
  if (!aString) {
    return s;
  }

  CFIndex length = CFStringGetLength(aString);
  CFIndex size = CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8) + 1;
  char* buffer = new char[size];
  if (!CFStringGetCString(aString, buffer, size, kCFStringEncodingUTF8)) {
    delete [] buffer;
    return s;
  }

  s = string(buffer);
  delete [] buffer;
  return s;
}

/* static */ string
AudioDeviceUtils::GetDeviceName(AudioObjectID aId)
{
  CFStringRef data = nullptr;
  UInt32 size = sizeof(data);
  OSStatus r = AudioObjectGetPropertyData(aId, &kDeviceNamePropertyAddress,
                                          0, NULL, &size, &data);
  if (r != noErr || !data) {
    return ""; // TODO: Maybe throw an error instead.
  }

  string name = CFStringRefToUTF8(data);
  CFRelease(data);
  return name;
}

/* static */ UInt32
AudioDeviceUtils::GetDeviceSource(AudioObjectID aId, Scope aScope) {
  UInt32 data;
  UInt32 size = sizeof(data);
  const AudioObjectPropertyAddress* address = aScope == Input ?
    &kInputDeviceSourcePropertyAddress : &kOutputDeviceSourcePropertyAddress;
  OSStatus r = AudioObjectGetPropertyData(aId, address, 0, NULL, &size, &data);
  if (r != noErr) {
    return 0; // TODO: Maybe throw an error instead.
  }

  return data;
}

/* static */ UInt32
AudioDeviceUtils::GetNumberOfStreams(AudioObjectID aId, Scope aScope) {
  const AudioObjectPropertyAddress* address = aScope == Input ?
    &kInputDeviceStreamsPropertyAddress : &kOutputDeviceStreamsPropertyAddress;
  UInt32 size = 0;
  OSStatus r = AudioObjectGetPropertyDataSize(aId, address, 0, NULL, &size);
  return r == noErr ? static_cast<UInt32>(size / sizeof(AudioStreamID)) : 0;
}

/* static */ bool
AudioDeviceUtils::IsInput(AudioObjectID aId) {
  return GetNumberOfStreams(aId, Input) > 0;
}

/* static */ bool
AudioDeviceUtils::IsOutput(AudioObjectID aId) {
  return GetNumberOfStreams(aId, Output) > 0;
}

/* static */ vector<AudioObjectID>
AudioDeviceUtils::GetAllDeviceIds()
{
  vector<AudioObjectID> ids;
  UInt32 size = 0;
  const AudioObjectPropertyAddress* address = &kDevicesPropertyAddress;
  OSStatus r = AudioObjectGetPropertyDataSize(kAudioObjectSystemObject, address,
                                              0, NULL, &size);
  if (r != noErr) {
    return ids;
  }

  UInt32 numbers = static_cast<UInt32>(size / sizeof(AudioObjectID));
  ids.resize(numbers);
  r = AudioObjectGetPropertyData(kAudioObjectSystemObject, address,
                                 0, NULL, &size, ids.data());
  if (r != noErr) {
    return ids;
  }

  return ids;
}

/* static */ bool
AudioDeviceUtils::SetDefaultDevice(AudioObjectID aId, Scope aScope)
{
  const AudioObjectPropertyAddress* address = aScope == Input ?
    &kDefaultInputDevicePropertyAddress : &kDefaultOutputDevicePropertyAddress;
  // TODO: This API returns noErr almost in any case. It returns noErr if
  //       aId is kAudioObjectUnknown. It returns noErr even if we set
  //       a non-input/non-output device to the default input/output device.
  //       It works weirdly. It's better to check the aId by ourselves.
  return AudioObjectSetPropertyData(kAudioObjectSystemObject, address,
                                    0, NULL, sizeof(aId), &aId) == noErr;
}