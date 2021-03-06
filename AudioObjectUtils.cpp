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
AudioObjectUtils::GetDefaultDeviceId(Scope scope)
{
  AudioObjectID id = kAudioObjectUnknown;
  const AudioObjectPropertyAddress* address = scope == Input?
    &kDefaultInputDevicePropertyAddress : &kDefaultOutputDevicePropertyAddress;
  OSStatus status = GetPropertyData(kAudioObjectSystemObject, address, &id);
  if (status != kAudioHardwareNoError) {
    return kAudioObjectUnknown; // TODO: Maybe throw an error instead.
  }
  return id;
}

/* static */ UInt32
AudioObjectUtils::GetNumberOfStreams(AudioObjectID id, Scope scope)
{
  const AudioObjectPropertyAddress* address = scope == Input ?
    &kInputDeviceStreamsPropertyAddress : &kOutputDeviceStreamsPropertyAddress;
  UInt32 size = 0;
  OSStatus status = GetPropertyDataSize(id, address, &size);
  return status == kAudioHardwareNoError ?
    static_cast<UInt32>(size / sizeof(AudioStreamID)) : 0;
}

/* static */ bool
AudioObjectUtils::InScope(AudioObjectID id, Scope scope)
{
  return GetNumberOfStreams(id, scope) > 0;
}

// /* static */ string
// AudioObjectUtils::CFStringRefToUTF8(CFStringRef stringRef)
// {
//   string s;
//   if (!stringRef) {
//     return s;
//   }

//   CFIndex length = CFStringGetLength(stringRef);
//   CFIndex size = CFStringGetMaximumSizeForEncoding(length,
//                                                    kCFStringEncodingUTF8) + 1;
//   char* buffer = new char[size];
//   if (CFStringGetCString(stringRef, buffer, size, kCFStringEncodingUTF8)) {
//     s = string(buffer);
//   }

//   delete [] buffer;
//   return s;
// }

/* static */ string
AudioObjectUtils::CFStringRefToUTF8(CFStringRef stringRef)
{
  string s;
  if (!stringRef) {
    return s;
  }

  CFIndex length = CFStringGetLength(stringRef);
  if (!length) {
    return s;
  }

  CFRange stringRange = CFRangeMake(0, length);
  CFIndex size = 0;
  CFIndex converted = CFStringGetBytes(stringRef,
                                       stringRange,
                                       kCFStringEncodingUTF8,
                                       0,       // lossByte
                                       false,   // isExternalRepresentation
                                       nullptr, // buffer
                                       0,       // maxBufLen
                                       &size);
  if (!converted || !size) {
    return s;
  }

  size_t elements = size + 1; // + 1 for a NUL terminator.
  vector<char> buffer(elements);

  converted = CFStringGetBytes(stringRef,
                               stringRange,
                               kCFStringEncodingUTF8,
                               0,      // lossByte
                               false,  // isExternalRepresentation
                               reinterpret_cast<UInt8*>(buffer.data()),
                               size,
                               nullptr);
  if (!converted) {
    return s;
  }

  buffer[elements - 1] = '\0';
  return string(buffer.data(), elements - 1);
}

/* static */ string
AudioObjectUtils::GetDeviceName(AudioObjectID id)
{
  CFStringRef data = nullptr;
  OSStatus status = GetPropertyData(id, &kDeviceNamePropertyAddress, &data);
  if (status != kAudioHardwareNoError || !data) {
    return ""; // TODO: Maybe throw an error instead.
  }

  string s = CFStringRefToUTF8(data);
  CFRelease(data);
  return s;
}

/* static */ UInt32
AudioObjectUtils::GetDeviceSource(AudioObjectID id, Scope scope)
{
  UInt32 data = 0;
  const AudioObjectPropertyAddress* address = scope == Input ?
    &kInputDeviceSourcePropertyAddress : &kOutputDeviceSourcePropertyAddress;
  OSStatus status = GetPropertyData(id, address, &data);
  if (status != kAudioHardwareNoError) {
    return 0; // TODO: Maybe throw an error instead.
  }

  return data;
}

/* static */ string
AudioObjectUtils::GetDeviceSourceName(AudioObjectID id, Scope scope,
                                      UInt32 aSource)
{
  CFStringRef source = nullptr;
  AudioValueTranslation translation;
  translation.mInputData = &aSource;
  translation.mInputDataSize = sizeof(aSource);
  translation.mOutputData = &source;
  translation.mOutputDataSize = sizeof(source);

  const AudioObjectPropertyAddress* address = scope == Input
    ? &kInputDeviceSourceNamePropertyAddress
    : &kOutputDeviceSourceNamePropertyAddress;
  OSStatus status = GetPropertyData(id, address, &translation);
  if (status != kAudioHardwareNoError) {
    return ""; // TODO: Maybe throw an error instead.
  }

  return CFStringRefToUTF8(source);
}

/* static */ bool
AudioObjectUtils::SetDefaultDevice(AudioObjectID id, Scope scope)
{
  // Surprisingly it's ok to set
  //   1. a unknown device
  //   2. a non-input/non-output device
  //   3. the current default input/output device
  // as the new default input/output device by apple's API.
  // We need to check the above things by ourselves.
  if (id == kAudioObjectUnknown ||
      !InScope(id, scope) ||
      id == GetDefaultDeviceId(scope)) {
    return false;
  }

  const AudioObjectPropertyAddress* address = scope == Input ?
    &kDefaultInputDevicePropertyAddress : &kDefaultOutputDevicePropertyAddress;
  return SetPropertyData(kAudioObjectSystemObject, address, &id)
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
AudioObjectUtils::GetDeviceIds(Scope scope) {
  vector<AudioObjectID> ids;

  vector<AudioObjectID> all = AudioObjectUtils::GetAllDeviceIds();
  for (AudioObjectID id : all) {
    if (InScope(id, scope)) {
      ids.push_back(id);
    }
  }

  return ids;
}

/* static */ string
AudioObjectUtils::GetDeviceLabel(AudioObjectID id, Scope scope)
{
  string label;
  if (!InScope(id, scope)) {
    return label;
  }

  UInt32 source = GetDeviceSource(id, scope);
  label = GetDeviceSourceName(id, scope, source);
  if (label.empty()) {
    label = GetDeviceName(id);
  }
  return label;
}