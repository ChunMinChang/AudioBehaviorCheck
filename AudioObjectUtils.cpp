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
  OSStatus status = GetPropertyData(kAudioObjectSystemObject, address, &id);
  if (status != kAudioHardwareNoError) {
    return kAudioObjectUnknown; // TODO: Maybe throw an error instead.
  }
  return id;
}

/* static */ UInt32
AudioObjectUtils::GetNumberOfStreams(AudioObjectID aId, Scope aScope)
{
  const AudioObjectPropertyAddress* address = aScope == Input ?
    &kInputDeviceStreamsPropertyAddress : &kOutputDeviceStreamsPropertyAddress;
  UInt32 size = 0;
  OSStatus status = GetPropertyDataSize(aId, address, &size);
  return status == kAudioHardwareNoError ?
    static_cast<UInt32>(size / sizeof(AudioStreamID)) : 0;
}

/* static */ bool
AudioObjectUtils::IsInScope(AudioObjectID aId, Scope aScope)
{
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
  if (CFStringGetCString(aString, buffer, size, kCFStringEncodingUTF8)) {
    s = string(buffer);
  }

  CFRelease(aString);
  delete [] buffer;
  return s;
}

/* static */ string
AudioObjectUtils::GetDeviceName(AudioObjectID aId)
{
  CFStringRef data = nullptr;
  OSStatus status = GetPropertyData(aId, &kDeviceNamePropertyAddress, &data);
  if (status != kAudioHardwareNoError || !data) {
    return ""; // TODO: Maybe throw an error instead.
  }

  return CFStringRefToUTF8(data);
}

/* static */ UInt32
AudioObjectUtils::GetDeviceSource(AudioObjectID aId, Scope aScope)
{
  UInt32 data = 0;
  const AudioObjectPropertyAddress* address = aScope == Input ?
    &kInputDeviceSourcePropertyAddress : &kOutputDeviceSourcePropertyAddress;
  OSStatus status = GetPropertyData(aId, address, &data);
  if (status != kAudioHardwareNoError) {
    return 0; // TODO: Maybe throw an error instead.
  }

  return data;
}

/* static */ string
AudioObjectUtils::GetDeviceSourceName(AudioObjectID aId, Scope aScope,
                                      UInt32 aSource)
{
  CFStringRef source = nullptr;
  AudioValueTranslation translation;
  translation.mInputData = &aSource;
  translation.mInputDataSize = sizeof(aSource);
  translation.mOutputData = &source;
  translation.mOutputDataSize = sizeof(source);

  const AudioObjectPropertyAddress* address = aScope == Input
    ? &kInputDeviceSourceNamePropertyAddress
    : &kOutputDeviceSourceNamePropertyAddress;
  OSStatus status = GetPropertyData(aId, address, &translation);
  if (status != kAudioHardwareNoError) {
    return ""; // TODO: Maybe throw an error instead.
  }

  return CFStringRefToUTF8(source);
}

/* static */ bool
AudioObjectUtils::SetDefaultDevice(AudioObjectID aId, Scope aScope)
{
  const AudioObjectPropertyAddress* address = aScope == Input ?
    &kDefaultInputDevicePropertyAddress : &kDefaultOutputDevicePropertyAddress;
  // TODO: This API returns kAudioHardwareNoError almost in any case.
  //       It returns kAudioHardwareNoError if aId is kAudioObjectUnknown.
  //       It returns kAudioHardwareNoError even if we set a non-input/non-output
  //       device to the default input/output device. It works weirdly.
  //       It's better to check the aId by ourselves.
  return SetPropertyData(kAudioObjectSystemObject, address, &aId)
         == kAudioHardwareNoError;
}

/* static */ vector<AudioObjectID>
AudioObjectUtils::GetAllDeviceIds()
{
  vector<AudioObjectID> ids;
  OSStatus status = GetPropertyArray(kAudioObjectSystemObject,
                                     &kDevicesPropertyAddress, &ids);
  if (status != kAudioHardwareNoError) {
    return {};
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