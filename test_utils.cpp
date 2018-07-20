#include "AudioDeviceUtils.h"
#include <cassert>  // for assert
#include <iostream> // for std::cout, std::endl

using std::cout;
using std::endl;

AudioDeviceUtils::Scope Input = AudioDeviceUtils::Input;
AudioDeviceUtils::Scope Output = AudioDeviceUtils::Output;

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
  AudioObjectID inId = AudioDeviceUtils::GetDefaultDeviceId(Input);
  assert(validId(inId));
  AudioObjectID outId = AudioDeviceUtils::GetDefaultDeviceId(Output);
  assert(validId(outId));
}

void testGetDeviceName()
{
  // Return empty string if it's a inavlid id.
  string unknown = AudioDeviceUtils::GetDeviceName(kAudioObjectUnknown);
  assert(unknown.empty());

  // Return an non-empty string if it's a valid id.
  AudioObjectID id = AudioDeviceUtils::GetDefaultDeviceId(Output);
  assert(validId(id) == !AudioDeviceUtils::GetDeviceName(id).empty());
}

void testGetDeviceSourceInvalidId()
{
  UInt32 data = 0;

  data = AudioDeviceUtils::GetDeviceSource(kAudioObjectUnknown, Input);
  assert(!data);

  data = AudioDeviceUtils::GetDeviceSource(kAudioObjectUnknown, Output);
  assert(!data);
}

void testGetDeviceSourceInvalidScope()
{
  UInt32 data = 0;

  AudioObjectID inId = AudioDeviceUtils::GetDefaultDeviceId(Input);
  if (validId(inId) && !AudioDeviceUtils::IsOutput(inId)) {
    data = AudioDeviceUtils::GetDeviceSource(inId, Output);
    assert(!data);
  }

  AudioObjectID outId = AudioDeviceUtils::GetDefaultDeviceId(Output);
  if (validId(outId) && !AudioDeviceUtils::IsInput(outId)) {
    data = AudioDeviceUtils::GetDeviceSource(outId, Input);
    assert(!data);
  }
}

void testGetDeviceSourceValidParameters()
{
  UInt32 data = 0;

  AudioObjectID inId = AudioDeviceUtils::GetDefaultDeviceId(Input);
  if (validId(inId)) {
    data = AudioDeviceUtils::GetDeviceSource(inId, Input);
    cout << "input source data: " << data << endl;
    // Some USB headset(e.g., Plantronics .Audio 628) fails to get its source.
  }

  data = 0;

  AudioObjectID outId = AudioDeviceUtils::GetDefaultDeviceId(Output);
  if (validId(outId)) {
    data = AudioDeviceUtils::GetDeviceSource(outId, Output);
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

  name = AudioDeviceUtils::GetDeviceSourceName(kAudioObjectUnknown, Input, 0);
  assert(name.empty());

  name = AudioDeviceUtils::GetDeviceSourceName(kAudioObjectUnknown, Output, 0);
  assert(name.empty());
}

// TODO: What if data is valid?
void testGetDeviceSourceNameInvalidScope()
{
  string name;

  AudioObjectID inId = AudioDeviceUtils::GetDefaultDeviceId(Input);
  if (validId(inId) && !AudioDeviceUtils::IsOutput(inId)) {
    name = AudioDeviceUtils::GetDeviceSourceName(inId, Output, 0);
    assert(name.empty());
  }

  AudioObjectID outId = AudioDeviceUtils::GetDefaultDeviceId(Output);
  if (validId(outId) && !AudioDeviceUtils::IsInput(outId)) {
    name = AudioDeviceUtils::GetDeviceSourceName(outId, Input, 0);
    assert(name.empty());
  }
}

void testGetDeviceSourceNameInvalidData()
{
  string name;

  AudioObjectID inId = AudioDeviceUtils::GetDefaultDeviceId(Input);
  if (validId(inId)) {
    name = AudioDeviceUtils::GetDeviceSourceName(inId, Input, 0);
    assert(name.empty());
  }

  AudioObjectID outId = AudioDeviceUtils::GetDefaultDeviceId(Output);
  if (validId(outId)) {
    name = AudioDeviceUtils::GetDeviceSourceName(outId, Output, 0);
    assert(name.empty());
  }
}

void testGetDeviceSourceNameValidParameters()
{
  UInt32 data = 0;
  string name;

  AudioObjectID inId = AudioDeviceUtils::GetDefaultDeviceId(Input);
  if (validId(inId)) {
    data = AudioDeviceUtils::GetDeviceSource(inId, Input);
    name = AudioDeviceUtils::GetDeviceSourceName(inId, Input, data);
    assert(!!data == !name.empty());
    cout << "input source data: " << data << ", name: " << name << endl;
    // Some USB headset(e.g., Plantronics .Audio 628) fails to get its source.
  }

  name.clear();
  data = 0;

  AudioObjectID outId = AudioDeviceUtils::GetDefaultDeviceId(Output);
  if (validId(outId)) {
    data = AudioDeviceUtils::GetDeviceSource(outId, Output);
    name = AudioDeviceUtils::GetDeviceSourceName(outId, Output, data);
    assert(!!data == !name.empty());
    cout << "input source data: " << data << ", name: " << name << endl;
    // Some USB headset(e.g., Plantronics .Audio 628) fails to get its source.
  }
}

void testGetDeviceSourceName()
{
  testGetDeviceSourceInvalidId();
  testGetDeviceSourceNameInvalidScope();
  testGetDeviceSourceNameInvalidData();
  testGetDeviceSourceNameValidParameters();
}

void testIsInput()
{
  // Return false for an invalid id.
  assert(!AudioDeviceUtils::IsInput(kAudioObjectUnknown));

  // Return true if we have a valid id.
  AudioObjectID id = AudioDeviceUtils::GetDefaultDeviceId(Input);
  assert(validId(id) == AudioDeviceUtils::IsInput(id));
}

void testIsOutput()
{
  // Return false for an invalid id.
  assert(!AudioDeviceUtils::IsOutput(kAudioObjectUnknown));

  // Return true if we have a valid id.
  AudioObjectID id = AudioDeviceUtils::GetDefaultDeviceId(Output);
  assert(validId(id) == AudioDeviceUtils::IsOutput(id));
}

void printDeviceInfo(AudioObjectID aId)
{
  string name = AudioDeviceUtils::GetDeviceName(aId);
  cout << "Device " << aId << " : " << name << " >";
  if (AudioDeviceUtils::IsInput(aId)) {
    cout << " input";
  }
  if (AudioDeviceUtils::IsOutput(aId)) {
    cout << " output";
  }
  cout << endl;
}

void testGetAllDeviceIds()
{
  vector<AudioObjectID> ids = AudioDeviceUtils::GetAllDeviceIds();
  for (AudioObjectID id : ids) {
    printDeviceInfo(id);
  }
}

vector<AudioObjectID> getDeviceIds(AudioDeviceUtils::Scope aScope) {
  vector<AudioObjectID> ids;

  vector<AudioObjectID> all = AudioDeviceUtils::GetAllDeviceIds();
  for (AudioObjectID id : all) {
    if (aScope == Input) {
      if (AudioDeviceUtils::IsInput(id)) {
        ids.push_back(id);
      }
    } else {
      if (AudioDeviceUtils::IsOutput(id)) {
        ids.push_back(id);
      }
    }
  }

  return ids;
}

bool changeDefaultDevice(AudioDeviceUtils::Scope aScope)
{
  vector<AudioObjectID> ids = getDeviceIds(aScope);
  if (ids.size() < 2) { // No other choice!
    return false;
  }

  AudioObjectID currentId = AudioDeviceUtils::GetDefaultDeviceId(aScope);
  // Get next available device.
  AudioObjectID newId;
  for (AudioObjectID id: ids) {
    if (id != currentId) {
      newId = id;
      break;
    }
  }

  return AudioDeviceUtils::SetDefaultDevice(newId, aScope);
}

void testSetDefaultDevice()
{
  // Surprisingly it's ok to set default input device to a unknown device
  // in apple's API. The system do nothing in this case.
  assert(AudioDeviceUtils::SetDefaultDevice(kAudioObjectUnknown, Input));

  // Surprisingly it's ok to set default output device to a unknown device
  // in apple's API. The system do nothing in this case.
  assert(AudioDeviceUtils::SetDefaultDevice(kAudioObjectUnknown, Output));

  AudioObjectID inId = AudioDeviceUtils::GetDefaultDeviceId(Input);
  if (validId(inId)) {
    // It's ok to set current default input device to default input device again.
    assert(AudioDeviceUtils::SetDefaultDevice(inId, Input));
    if (!AudioDeviceUtils::IsOutput(inId)) {
      // Surprisingly it's ok to set default output device to a non-output device!
      assert(AudioDeviceUtils::SetDefaultDevice(inId, Output));
    }
  }

  AudioObjectID outId = AudioDeviceUtils::GetDefaultDeviceId(Output);
  if (validId(outId)) {
    // It's ok to set current default output device to default input device again.
    assert(AudioDeviceUtils::SetDefaultDevice(outId, Output));
    if (!AudioDeviceUtils::IsInput(outId)) {
      // Surprisingly it's ok to set default intput device to a non-input device!
      assert(AudioDeviceUtils::SetDefaultDevice(outId, Input));
    }
  }

  // It's ok to change the default input device if there are more than 1
  // input devices. Otherwise, it's failed to do that.
  assert((getDeviceIds(Input).size() > 1) == changeDefaultDevice(Input));

  // It's ok to change the default output device if there are more than 1
  // output devices. Otherwise, it's failed to do that.
  assert((getDeviceIds(Output).size() > 1) == changeDefaultDevice(Output));
}

int main()
{
  testGetDefaultDeviceId();
  testGetDeviceName();
  testGetDeviceSource();
  testGetDeviceSourceName();
  testIsInput();
  testIsOutput();
  testSetDefaultDevice();
  testGetAllDeviceIds();
  return 0;
}