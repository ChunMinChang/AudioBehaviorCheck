#include <CoreFoundation/CFString.h>
#include "AudioDeviceUtils.h"

const AudioObjectPropertyAddress kDevicesPropertyAddress = {
  kAudioHardwarePropertyDevices,
  kAudioObjectPropertyScopeGlobal,
  kAudioObjectPropertyElementMaster
};

const AudioObjectPropertyAddress kDeviceNameProperty = {
  kAudioObjectPropertyName,
  kAudioObjectPropertyScopeGlobal,
  kAudioObjectPropertyElementMaster
};

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

/* static */ vector<AudioDeviceID>
AudioDeviceUtils::GetAllDeviceIDs()
{
  UInt32 size = 0;
  const AudioObjectPropertyAddress* addr = &kDevicesPropertyAddress;
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

/* static */ string
GetDeviceName(AudioDeviceID aID)
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
AudioDeviceUtils::GetAllDeviceNames()
{
  vector<string> names;
  vector<AudioDeviceID> ids = GetAllDeviceIDs();
  for (AudioDeviceID id : ids) {
    names.push_back(GetDeviceName(id));
  }
  return names;
}