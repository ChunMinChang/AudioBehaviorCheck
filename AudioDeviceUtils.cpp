#include <CoreFoundation/CFString.h> // for CFStringXXX

#include "AudioDeviceUtils.h"

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

/* static */ AudioObjectID
AudioDeviceUtils::GetDefaultDeviceId(bool aInput)
{
  AudioObjectID id;
  UInt32 size = sizeof(id);
  const AudioObjectPropertyAddress* address = aInput ?
    &kDefaultInputDevicePropertyAddress : &kDefaultOutputDevicePropertyAddress;
  OSStatus r = AudioObjectGetPropertyData(kAudioObjectSystemObject, address,
                                          0, 0, &size, &id);
  return r == noErr ? id : kAudioObjectUnknown;
}

static char*
CFStringRefToUTF8(CFStringRef aString)
{
  if (!aString) {
    return nullptr;
  }

  CFIndex length = CFStringGetLength(aString);
  CFIndex size = CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8) + 1;
  char *buffer = new char[size];
  if (!CFStringGetCString(aString, buffer, size, kCFStringEncodingUTF8)) {
    delete [] buffer;
    buffer = nullptr;
  }

  return buffer;
}

/* static */ string
AudioDeviceUtils::GetDeviceName(AudioObjectID aId)
{
  CFStringRef data = nullptr;
  UInt32 size = sizeof(data);
  OSStatus r = AudioObjectGetPropertyData(aId, &kDeviceNamePropertyAddress,
                                          0, NULL, &size, &data);
  if (r != noErr || !data) {
    return ""; // TODO: Not a good style. Fix it later.
  }

  char* name = CFStringRefToUTF8(data);
  CFRelease(data);
  if (!name) {
    return ""; // TODO: Not a good style. Fix it later.
  }

  return std::string(name);
}

UInt32
GetNumberOfStreams(AudioObjectID aId, bool aInput) {
  const AudioObjectPropertyAddress* address = aInput ?
    &kInputDeviceStreamsPropertyAddress : &kOutputDeviceStreamsPropertyAddress;
  UInt32 size = 0;
  OSStatus r = AudioObjectGetPropertyDataSize(aId, address, 0, NULL, &size);
  return r == noErr ? static_cast<UInt32>(size / sizeof(AudioStreamID)) : 0;
}

/* static */ bool
AudioDeviceUtils::IsInput(AudioObjectID aId) {
  return GetNumberOfStreams(aId, true) > 0;
}

/* static */ bool
AudioDeviceUtils::IsOutput(AudioObjectID aId) {
  return GetNumberOfStreams(aId, false) > 0;
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

/* static */
vector<AudioObjectID>
AudioDeviceUtils::GetDeviceIds(bool aInput) {
  vector<AudioObjectID> ids;

  vector<AudioObjectID> all = GetAllDeviceIds();
  for (AudioObjectID id : all) {
    if (GetNumberOfStreams(id, aInput) > 0) {
      ids.push_back(id);
    }
  }

  return ids;
}

bool
SetDefaultDevice(AudioObjectID aId, bool aInput)
{
  const AudioObjectPropertyAddress* address = aInput ?
    &kDefaultInputDevicePropertyAddress : &kDefaultOutputDevicePropertyAddress;
  return AudioObjectSetPropertyData(kAudioObjectSystemObject, address,
                                    0, NULL, sizeof(aId), &aId) == noErr;
}

/* static */ bool
AudioDeviceUtils::ChangeDefaultDevice(bool aInput)
{
  vector<AudioObjectID> ids = GetDeviceIds(aInput);
  if (ids.size() < 2) { // No other choice!
    return false;
  }

  AudioObjectID currentId = GetDefaultDeviceId(aInput);
  // Get next available device.
  AudioObjectID newId;
  for (AudioObjectID id: ids) {
    if (id != currentId) {
      newId = id;
      break;
    }
  }

  return SetDefaultDevice(newId, aInput);
}