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

/* static */ vector<AudioObjectID>
AudioDeviceUtils::GetAllDeviceIDs()
{
  vector<AudioObjectID> ids;
  UInt32 size = 0;
  const AudioObjectPropertyAddress* addr = &kDevicesPropertyAddress;
  OSStatus r = AudioObjectGetPropertyDataSize(kAudioObjectSystemObject, addr,
                                              0, NULL, &size);
  if (r != noErr) {
    return ids;
  }

  UInt32 numbers = static_cast<UInt32>(size / sizeof(AudioObjectID));
  ids.resize(numbers);
  r = AudioObjectGetPropertyData(kAudioObjectSystemObject, addr,
                                 0, NULL, &size, ids.data());
  if (r != noErr) {
    return ids;
  }

  return ids;
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
GetDeviceName(AudioObjectID aID)
{
  CFStringRef data = nullptr;
  UInt32 size = sizeof(data);
  OSStatus r = AudioObjectGetPropertyData(aID, &kDeviceNameProperty,
                                          0, NULL, &size, &data);
  if (r != noErr || !data) {
    return ""; // TODO: Not a good style. Fix later.
  }

  char* name = CFStringRefToUTF8(data);
  CFRelease(data);
  if (!name) {
    return ""; // TODO: Not a good style. Fix later.
  }

  return std::string(name);
}

/* static */ vector<string>
AudioDeviceUtils::GetAllDeviceNames()
{
  vector<string> names;
  vector<AudioObjectID> ids = GetAllDeviceIDs();
  for (AudioObjectID id : ids) {
    names.push_back(GetDeviceName(id));
  }
  return names;
}