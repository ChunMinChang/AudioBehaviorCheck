#include "AudioObjectUtils.h"
#include <cassert>  // for assert
#include <iostream> // for std::cout, std::endl

using std::cout;
using std::endl;

AudioObjectUtils::Scope Input = AudioObjectUtils::Input;
AudioObjectUtils::Scope Output = AudioObjectUtils::Output;

bool validId(AudioObjectID aId)
{
  return aId != kAudioObjectUnknown;
}

void testGetDefaultDeviceId()
{
  // If we don't have input/output devices, the returned ids must be invalid?
  // TODO: Find a way to unload all input/outpu devices so we don't have
  //       default devices and then we can check the behaviors here.
  //       Find a way to reload all input/outpu devices so we can continue
  //       tests.

  // If we have default input/output devices, then they must be valid ids.
  AudioObjectID inId = AudioObjectUtils::GetDefaultDeviceId(Input);
  assert(validId(inId));
  AudioObjectID outId = AudioObjectUtils::GetDefaultDeviceId(Output);
  assert(validId(outId));
}

void testIsInScope()
{
  // Return false for invalid ids.
  assert(!AudioObjectUtils::IsInScope(kAudioObjectUnknown, Input));
  assert(!AudioObjectUtils::IsInScope(kAudioObjectUnknown, Output));

  // Return true if we have valid ids.
  AudioObjectID inId = AudioObjectUtils::GetDefaultDeviceId(Input);
  assert(validId(inId) == AudioObjectUtils::IsInScope(inId, Input));

  AudioObjectID outId = AudioObjectUtils::GetDefaultDeviceId(Output);
  assert(validId(outId) == AudioObjectUtils::IsInScope(outId, Output));
}

void testGetDeviceName()
{
  // Return empty string if it's a inavlid id.
  string unknown = AudioObjectUtils::GetDeviceName(kAudioObjectUnknown);
  assert(unknown.empty());

  // Return an non-empty string if it's a valid id.
  AudioObjectID id = AudioObjectUtils::GetDefaultDeviceId(Output);
  assert(validId(id) == !AudioObjectUtils::GetDeviceName(id).empty());
}

void testGetDeviceSourceInvalidId()
{
  UInt32 data = 0;

  data = AudioObjectUtils::GetDeviceSource(kAudioObjectUnknown, Input);
  assert(!data);

  data = AudioObjectUtils::GetDeviceSource(kAudioObjectUnknown, Output);
  assert(!data);
}

void testGetDeviceSourceInvalidScope()
{
  UInt32 data = 0;

  AudioObjectID inId = AudioObjectUtils::GetDefaultDeviceId(Input);
  bool validInputIsNotOutput = validId(inId) &&
                               !AudioObjectUtils::IsInScope(inId, Output);
  if (validInputIsNotOutput) {
    data = AudioObjectUtils::GetDeviceSource(inId, Output);
    assert(!data);
  }

  AudioObjectID outId = AudioObjectUtils::GetDefaultDeviceId(Output);
  bool validOutputIsNotInput = validId(outId) &&
                               !AudioObjectUtils::IsInScope(outId, Input);
  if (validOutputIsNotInput) {
    data = AudioObjectUtils::GetDeviceSource(outId, Input);
    assert(!data);
  }
}

void testGetDeviceSourceValidParameters()
{
  UInt32 data = 0;

  AudioObjectID inId = AudioObjectUtils::GetDefaultDeviceId(Input);
  if (validId(inId)) {
    data = AudioObjectUtils::GetDeviceSource(inId, Input);
    cout << "input source data: " << data << endl;
    // Some USB headset(e.g., Plantronics .Audio 628) fails to get its source.
  }

  data = 0;

  AudioObjectID outId = AudioObjectUtils::GetDefaultDeviceId(Output);
  if (validId(outId)) {
    data = AudioObjectUtils::GetDeviceSource(outId, Output);
    cout << "output source data: " << data << endl;
    // Some USB headset(e.g., Plantronics .Audio 628) fails to get its source.
  }
}

void testGetDeviceSource()
{
  testGetDeviceSourceInvalidId();
  testGetDeviceSourceInvalidScope();
  testGetDeviceSourceValidParameters();
}

// TODO: What if data is valid?
void testGetDeviceSourceNameInvalidId()
{
  string name;

  name = AudioObjectUtils::GetDeviceSourceName(kAudioObjectUnknown, Input, 0);
  assert(name.empty());

  name = AudioObjectUtils::GetDeviceSourceName(kAudioObjectUnknown, Output, 0);
  assert(name.empty());
}

// TODO: What if data is valid?
void testGetDeviceSourceNameInvalidScope()
{
  string name;

  AudioObjectID inId = AudioObjectUtils::GetDefaultDeviceId(Input);
  bool validInputIsNotOutput = validId(inId) &&
                               !AudioObjectUtils::IsInScope(inId, Output);
  if (validInputIsNotOutput) {
    name = AudioObjectUtils::GetDeviceSourceName(inId, Output, 0);
    assert(name.empty());
  }

  AudioObjectID outId = AudioObjectUtils::GetDefaultDeviceId(Output);
  bool validOutputIsNotInput = validId(outId) &&
                               !AudioObjectUtils::IsInScope(outId, Input);
  if (validOutputIsNotInput) {
    name = AudioObjectUtils::GetDeviceSourceName(outId, Input, 0);
    assert(name.empty());
  }
}

void testGetDeviceSourceNameInvalidSource()
{
  string name;
  UInt32 source = 0; // TODO: Check if Apple states it's invalid

  AudioObjectID inId = AudioObjectUtils::GetDefaultDeviceId(Input);
  if (validId(inId)) {
    name = AudioObjectUtils::GetDeviceSourceName(inId, Input, source);
    assert(name.empty());
  }

  AudioObjectID outId = AudioObjectUtils::GetDefaultDeviceId(Output);
  if (validId(outId)) {
    name = AudioObjectUtils::GetDeviceSourceName(outId, Output, source);
    assert(name.empty());
  }
}

void testGetDeviceSourceNameValidParameters()
{
  UInt32 data = 0;
  string name;

  AudioObjectID inId = AudioObjectUtils::GetDefaultDeviceId(Input);
  if (validId(inId)) {
    data = AudioObjectUtils::GetDeviceSource(inId, Input);
    name = AudioObjectUtils::GetDeviceSourceName(inId, Input, data);
    bool validData = !!data;
    bool validName = !name.empty();
    assert(validData == validName);
    cout << "input source data: " << data << ", name: " << name << endl;
    // Some USB headset(e.g., Plantronics .Audio 628) fails to get its source.
  }

  name.clear();
  data = 0;

  AudioObjectID outId = AudioObjectUtils::GetDefaultDeviceId(Output);
  if (validId(outId)) {
    data = AudioObjectUtils::GetDeviceSource(outId, Output);
    name = AudioObjectUtils::GetDeviceSourceName(outId, Output, data);
    bool validData = !!data;
    bool validName = !name.empty();
    assert(validData == validName);
    cout << "input source data: " << data << ", name: " << name << endl;
    // Some USB headset(e.g., Plantronics .Audio 628) fails to get its source.
  }
}

void testGetDeviceSourceName()
{
  testGetDeviceSourceInvalidId();
  testGetDeviceSourceNameInvalidScope();
  testGetDeviceSourceNameInvalidSource();
  testGetDeviceSourceNameValidParameters();
}

void testGetDeviceLabelInvalidId()
{
  string label;

  label = AudioObjectUtils::GetDeviceLabel(kAudioObjectUnknown, Input);
  assert(label.empty());

  label = AudioObjectUtils::GetDeviceLabel(kAudioObjectUnknown, Output);
  assert(label.empty());
}

// TODO: The GetDeviceLabel will fall back to get device name if there is no
//       defined source name. If the passed scope is wrong and it's the reason
//       of getting nothing on source-name, users should know that. However, in
//       the current implementation, we fall back to get device-name. This
//       should be fixed!
void testGetDeviceLabelInvalidScope()
{
  string label;

  AudioObjectID inId = AudioObjectUtils::GetDefaultDeviceId(Input);
  if (!AudioObjectUtils::IsInScope(inId, Output)) {
    // TODO: After AudioObjectUtils::GetDeviceLabel is changed, we should use
    //       assert(label.empty()) instead.
    label = AudioObjectUtils::GetDeviceLabel(inId, Output);
    string name = AudioObjectUtils::GetDeviceName(inId);
    assert(label == name);
  }

  AudioObjectID outId = AudioObjectUtils::GetDefaultDeviceId(Output);
  if (!AudioObjectUtils::IsInScope(outId, Input)) {
    // TODO: After AudioObjectUtils::GetDeviceLabel is changed, we should use
    //       assert(label.empty()) instead.
    label = AudioObjectUtils::GetDeviceLabel(outId, Input);
    string name = AudioObjectUtils::GetDeviceName(outId);
    assert(label == name);
  }
}

void testGetDeviceLabelValidParameters()
{
  string label;

  AudioObjectID inId = AudioObjectUtils::GetDefaultDeviceId(Input);
  label = AudioObjectUtils::GetDeviceLabel(inId, Input);
  bool validInputId = validId(inId);
  bool validInputLabel = !label.empty();
  assert(validInputId == validInputLabel);

  label.clear();

  AudioObjectID outId = AudioObjectUtils::GetDefaultDeviceId(Output);
  label = AudioObjectUtils::GetDeviceLabel(outId, Output);
  bool validOutputId = validId(outId);
  bool validOutputLabel = !label.empty();
  assert(validOutputId == validOutputLabel);
}

void testGetDeviceLabel()
{
  testGetDeviceLabelInvalidId();
  testGetDeviceLabelInvalidScope();
  testGetDeviceLabelValidParameters();
}

void printDeviceInfo(AudioObjectID aId)
{
  string name = AudioObjectUtils::GetDeviceName(aId);
  cout << "Device " << aId << " : " << name << " >";
  if (AudioObjectUtils::IsInScope(aId, Input)) {
    cout << " input";
  }
  if (AudioObjectUtils::IsInScope(aId, Output)) {
    cout << " output";
  }
  cout << endl;
}

void testGetAllDeviceIds()
{
  vector<AudioObjectID> ids = AudioObjectUtils::GetAllDeviceIds();
  for (AudioObjectID id : ids) {
    printDeviceInfo(id);
  }

  AudioObjectID inId = AudioObjectUtils::GetDefaultDeviceId(Input);
  AudioObjectID outId = AudioObjectUtils::GetDefaultDeviceId(Output);
  // If we have at least one device, the device id list must not be empty.
  bool atLeastOneDevice = validId(inId) || validId(outId);
  assert(atLeastOneDevice == !ids.empty());
}

void testSetDefaultDeviceInvalidId()
{
  // Surprisingly it's ok to set default input device to a unknown device
  // in apple's API. The system do nothing in this case.
  assert(AudioObjectUtils::SetDefaultDevice(kAudioObjectUnknown, Input));

  // Surprisingly it's ok to set default output device to a unknown device
  // in apple's API. The system do nothing in this case.
  assert(AudioObjectUtils::SetDefaultDevice(kAudioObjectUnknown, Output));
}

void testSetDefaultDeviceSameDefaultDevice()
{
  AudioObjectID inId = AudioObjectUtils::GetDefaultDeviceId(Input);
  if (validId(inId)) {
    // It's ok to set current default input device to default input device again.
    assert(AudioObjectUtils::SetDefaultDevice(inId, Input));
  }

  AudioObjectID outId = AudioObjectUtils::GetDefaultDeviceId(Output);
  if (validId(outId)) {
    // It's ok to set current default output device to default input device again.
    assert(AudioObjectUtils::SetDefaultDevice(outId, Output));
  }
}

void testSetDefaultDeviceInvalidScope()
{
  AudioObjectID inId = AudioObjectUtils::GetDefaultDeviceId(Input);
  if (validId(inId) && !AudioObjectUtils::IsInScope(inId, Output)) {
    // Surprisingly it's ok to set a non-output device to default output device.
    assert(AudioObjectUtils::SetDefaultDevice(inId, Output));
  }

  AudioObjectID outId = AudioObjectUtils::GetDefaultDeviceId(Output);
  if (validId(outId) && !AudioObjectUtils::IsInScope(outId, Input)) {
    // Surprisingly it's ok to set a non-input device to default intput device.
    assert(AudioObjectUtils::SetDefaultDevice(outId, Input));
  }
}

bool changeDefaultDevice(AudioObjectUtils::Scope aScope)
{
  vector<AudioObjectID> ids = AudioObjectUtils::GetDeviceIds(aScope);
  if (ids.size() < 2) { // No other choice!
    return false;
  }

  AudioObjectID currentId = AudioObjectUtils::GetDefaultDeviceId(aScope);
  // Get next available device.
  AudioObjectID newId;
  for (AudioObjectID id: ids) {
    if (id != currentId) {
      newId = id;
      break;
    }
  }

  return AudioObjectUtils::SetDefaultDevice(newId, aScope);
}

void testSetDefaultDeviceValidParameters()
{
  // It's ok to change the default input device if there are more than 1
  // input devices. Otherwise, it's failed to do that.
  bool moreThanOneInput = AudioObjectUtils::GetDeviceIds(Input).size() > 1;
  assert(moreThanOneInput == changeDefaultDevice(Input));

  // It's ok to change the default output device if there are more than 1
  // output devices. Otherwise, it's failed to do that.
  bool moreThanOneOutput = AudioObjectUtils::GetDeviceIds(Output).size() > 1;
  assert(moreThanOneOutput == changeDefaultDevice(Output));
}

void testSetDefaultDevice()
{
  testSetDefaultDeviceInvalidId();
  testSetDefaultDeviceSameDefaultDevice();
  testSetDefaultDeviceInvalidScope();
  testSetDefaultDeviceValidParameters();
}

int main()
{
  testGetDefaultDeviceId();
  testIsInScope();
  testGetDeviceName();
  testGetDeviceSource();
  testGetDeviceSourceName();
  testGetDeviceLabel();
  testGetAllDeviceIds();
  testSetDefaultDevice();
  return 0;
}