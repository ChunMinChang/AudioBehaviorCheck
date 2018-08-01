#include "AudioObjectUtils.h"
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

const AudioObjectPropertyAddress kDefaultInputDevicePropertyAddress = {
  kAudioHardwarePropertyDefaultInputDevice,
  kAudioObjectPropertyScopeGlobal,
  kAudioObjectPropertyElementMaster
};

const AudioObjectPropertyAddress kDefaultOutputDevicePropertyAddress = {
  kAudioHardwarePropertyDefaultOutputDevice,
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

const AudioObjectPropertyAddress kInputDeviceSourceNamePropertyAddress = {
  kAudioDevicePropertyDataSourceNameForIDCFString,
  kAudioObjectPropertyScopeInput,
  kAudioObjectPropertyElementMaster
};

const AudioObjectPropertyAddress kOutputDeviceSourceNamePropertyAddress = {
  kAudioDevicePropertyDataSourceNameForIDCFString,
  kAudioObjectPropertyScopeOutput,
  kAudioObjectPropertyElementMaster
};

/* static */ AudioObjectID
AudioObjectUtils::GetDefaultDeviceId(Scope aScope)
{
  AudioObjectID id = kAudioObjectUnknown;
  const AudioObjectPropertyAddress* address = aScope == Input?
    &kDefaultInputDevicePropertyAddress : &kDefaultOutputDevicePropertyAddress;
  OSStatus r = GetPropertyData(kAudioObjectSystemObject, address, &id);
  if (r != kAudioHardwareNoError) {
    return kAudioObjectUnknown; // TODO: Maybe throw an error instead.
  }
  return id;
}

/* static */ UInt32
AudioObjectUtils::GetNumberOfStreams(AudioObjectID aId, Scope aScope) {
  const AudioObjectPropertyAddress* address = aScope == Input ?
    &kInputDeviceStreamsPropertyAddress : &kOutputDeviceStreamsPropertyAddress;
  UInt32 size = 0;
  OSStatus r = GetPropertyDataSize(aId, address, &size);
  return r == kAudioHardwareNoError ?
    static_cast<UInt32>(size / sizeof(AudioStreamID)) : 0;
}

/* static */ bool
AudioObjectUtils::IsInScope(AudioObjectID aId, Scope aScope) {
  return GetNumberOfStreams(aId, aScope) > 0;
}

/* static */ string
AudioObjectUtils::CFStringRefToUTF8(CFStringRef aString)
{
  string s;
  if (!aString) {
    return s;
  }

  CFIndex length = CFStringGetLength(aString);
  CFIndex size = CFStringGetMaximumSizeForEncoding(length,
                                                   kCFStringEncodingUTF8) + 1;
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
AudioObjectUtils::GetDeviceName(AudioObjectID aId)
{
  string name;
  CFStringRef data = nullptr;
  OSStatus r = GetPropertyData(aId, &kDeviceNamePropertyAddress, &data);
  if (r != kAudioHardwareNoError || !data) {
    return name; // TODO: Maybe throw an error instead.
  }

  name = CFStringRefToUTF8(data);
  CFRelease(data);
  return name;
}

/* static */ UInt32
AudioObjectUtils::GetDeviceSource(AudioObjectID aId, Scope aScope) {
  UInt32 data = 0;
  const AudioObjectPropertyAddress* address = aScope == Input ?
    &kInputDeviceSourcePropertyAddress : &kOutputDeviceSourcePropertyAddress;
  OSStatus r = GetPropertyData(aId, address, &data);
  if (r != kAudioHardwareNoError) {
    return 0; // TODO: Maybe throw an error instead.
  }

  return data;
}

/* static */ string
AudioObjectUtils::GetDeviceSourceName(AudioObjectID aId, Scope aScope,
                                      UInt32 aSource) {
  string name;
  CFStringRef source = nullptr;
  AudioValueTranslation translation;
  translation.mInputData = &aSource;
  translation.mInputDataSize = sizeof(aSource);
  translation.mOutputData = &source;
  translation.mOutputDataSize = sizeof(source);

  const AudioObjectPropertyAddress* address = aScope == Input
    ? &kInputDeviceSourceNamePropertyAddress
    : &kOutputDeviceSourceNamePropertyAddress;
  OSStatus r = GetPropertyData(aId, address, &translation);
  if (r != kAudioHardwareNoError) {
    return name; // TODO: Maybe throw an error instead.
  }

  name = CFStringRefToUTF8(source);
  CFRelease(source);
  return name;
}

/* static */ bool
AudioObjectUtils::SetDefaultDevice(AudioObjectID aId, Scope aScope)
{
  const AudioObjectPropertyAddress* address = aScope == Input ?
    &kDefaultInputDevicePropertyAddress : &kDefaultOutputDevicePropertyAddress;
  // TODO: This API returns kAudioHardwareNoError almost in any case. It returns kAudioHardwareNoError if
  //       aId is kAudioObjectUnknown. It returns kAudioHardwareNoError even if we set
  //       a non-input/non-output device to the default input/output device.
  //       It works weirdly. It's better to check the aId by ourselves.
  return SetPropertyData(kAudioObjectSystemObject, address, sizeof(aId), &aId)
         == kAudioHardwareNoError;
}

/* static */ vector<AudioObjectID>
AudioObjectUtils::GetAllDeviceIds()
{
  vector<AudioObjectID> ids;
  UInt32 size = 0;
  const AudioObjectPropertyAddress* address = &kDevicesPropertyAddress;
  OSStatus r = GetPropertyDataSize(kAudioObjectSystemObject, address, &size);
  if (r != kAudioHardwareNoError) {
    return ids;
  }

  UInt32 numbers = static_cast<UInt32>(size / sizeof(AudioObjectID));
  ids.resize(numbers);
  r = GetPropertyData(kAudioObjectSystemObject, address, ids.data(), size);
  if (r != kAudioHardwareNoError) {
    return ids;
  }

  return ids;
}

/* static */ vector<AudioObjectID>
AudioObjectUtils::GetDeviceIds(Scope aScope) {
  vector<AudioObjectID> ids;

  vector<AudioObjectID> all = AudioObjectUtils::GetAllDeviceIds();
  for (AudioObjectID id : all) {
    if (IsInScope(id, aScope)) {
      ids.push_back(id);
    }
  }

  return ids;
}

/* static */ string
AudioObjectUtils::GetDeviceLabel(AudioObjectID aId, Scope aScope)
{
  UInt32 source = GetDeviceSource(aId, aScope);
  string label = GetDeviceSourceName(aId, aScope, source);
  // TODO: We will return device name when the scope is wrong and that's weird.
  //       We should notice users the scope is wrong. If GetDeviceSourceName()
  //       can detect scope and throw an error in this case, it will be easier
  //       to handle this.
  if (label.empty()) {
    label = GetDeviceName(aId);
  }
  return label;
}