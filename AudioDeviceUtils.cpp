#include <CoreFoundation/CFString.h>
#include "AudioDeviceUtils.h"

const AudioObjectPropertyAddress kDevicesPropertyAddress = {
  kAudioHardwarePropertyDevices,
  kAudioObjectPropertyScopeGlobal,
  kAudioObjectPropertyElementMaster
};

const AudioObjectPropertyAddress kInputDevicesPropertyAddress = {
  kAudioHardwarePropertyDevices,
  kAudioDevicePropertyScopeInput,
  kAudioObjectPropertyElementMaster
};

const AudioObjectPropertyAddress kOutputDevicesPropertyAddress = {
  kAudioHardwarePropertyDevices,
  kAudioDevicePropertyScopeOutput,
  kAudioObjectPropertyElementMaster
};

const AudioObjectPropertyAddress kDeviceNameProperty = {
  kAudioObjectPropertyName,
  kAudioObjectPropertyScopeGlobal,
  kAudioObjectPropertyElementMaster
};

const AudioObjectPropertyAddress kInputDeviceNameProperty = {
  kAudioObjectPropertyName,
  kAudioDevicePropertyScopeInput,
  kAudioObjectPropertyElementMaster
};

const AudioObjectPropertyAddress kInputOutputNameProperty = {
  kAudioObjectPropertyName,
  kAudioDevicePropertyScopeOutput,
  kAudioObjectPropertyElementMaster
};

/* static */ vector<AudioDeviceID>
AudioDeviceUtils::GetDevicesID(bool aIsInput)
{
  UInt32 size = 0;
  const AudioObjectPropertyAddress* addr = aIsInput ?
    &kInputDevicesPropertyAddress : &kOutputDevicesPropertyAddress;

  OSStatus r = AudioObjectGetPropertyDataSize(kAudioObjectSystemObject, addr,
                                              0, NULL, &size);
  if (r != noErr) {
    return vector<AudioObjectID>();
  }

  UInt32 numbers = static_cast<UInt32>(size / sizeof(AudioDeviceID));
  vector<AudioObjectID> IDs(numbers);
  r = AudioObjectGetPropertyData(kAudioObjectSystemObject, addr,
                                 0, NULL, &size, IDs.data());
  if (r != noErr) {
    return vector<AudioObjectID>();
  }

  return IDs;
}

/* static */ vector<AudioDeviceID>
AudioDeviceUtils::GetOutputDevicesID()
{
  return GetDevicesID(false);
}

/* static */ vector<AudioDeviceID>
AudioDeviceUtils::GetInputDevicesID()
{
  return GetDevicesID(true);
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
AudioDeviceUtils::GetDeviceName(AudioDeviceID aID)
{
  CFStringRef data = nullptr;
  UInt32 size = sizeof(data);
  OSStatus r = AudioObjectGetPropertyData(aID, &kDeviceNameProperty,
                                          0, NULL, &size, &data);
  if (r != noErr || !data) {
    return "";
  }

  char* name = CFStringRefToUTF8(data);
  CFRelease(data);
  if (!name) {
    return "";
  }

  return std::string(name);
}

/* static */ vector<string>
AudioDeviceUtils::GetDevicesName(vector<AudioDeviceID> aIDs)
{
  vector<string> names;
  for (AudioDeviceID id : aIDs) {
    names.push_back(GetDeviceName(id));
  }
  return names;
}