#include "AudioDeviceUtils.h"
#include <cassert>  // for assert
#include <iostream> // for std::cout, std::endl

using std::cout;
using std::endl;

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
  AudioObjectID inId = AudioDeviceUtils::GetDefaultDeviceId(true);
  assert(validId(inId));
  AudioObjectID outId = AudioDeviceUtils::GetDefaultDeviceId(false);
  assert(validId(outId));
}

void testGetDeviceName()
{
  // Return empty string if it's a inavlid id.
  string unknown = AudioDeviceUtils::GetDeviceName(kAudioObjectUnknown);
  assert(unknown.empty());

  // Return an non-empty string if it's a valid id.
  AudioObjectID id = AudioDeviceUtils::GetDefaultDeviceId(false);
  assert(validId(id) == !AudioDeviceUtils::GetDeviceName(id).empty());
}

void testIsInput()
{
  // Return false for an invalid id.
  assert(!AudioDeviceUtils::IsInput(kAudioObjectUnknown));

  // Return true if we have a valid id.
  AudioObjectID id = AudioDeviceUtils::GetDefaultDeviceId(true);
  assert(validId(id) == AudioDeviceUtils::IsInput(id));
}

void testIsOutput()
{
  // Return false for an invalid id.
  assert(!AudioDeviceUtils::IsOutput(kAudioObjectUnknown));

  // Return true if we have a valid id.
  AudioObjectID id = AudioDeviceUtils::GetDefaultDeviceId(false);
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

vector<AudioObjectID> getDeviceIds(bool aInput) {
  vector<AudioObjectID> ids;

  vector<AudioObjectID> all = AudioDeviceUtils::GetAllDeviceIds();
  for (AudioObjectID id : all) {
    if (aInput) {
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

bool changeDefaultDevice(bool aInput)
{
  vector<AudioObjectID> ids = getDeviceIds(aInput);
  if (ids.size() < 2) { // No other choice!
    return false;
  }

  AudioObjectID currentId = AudioDeviceUtils::GetDefaultDeviceId(aInput);
  // Get next available device.
  AudioObjectID newId;
  for (AudioObjectID id: ids) {
    if (id != currentId) {
      newId = id;
      break;
    }
  }

  return AudioDeviceUtils::SetDefaultDevice(newId, aInput);
}

void testSetDefaultDevice()
{
  // Surprisingly it's ok to set default input device to a unknown device
  // in apple's API. The system do nothing in this case.
  assert(AudioDeviceUtils::SetDefaultDevice(kAudioObjectUnknown, true));

  // Surprisingly it's ok to set default output device to a unknown device
  // in apple's API. The system do nothing in this case.
  assert(AudioDeviceUtils::SetDefaultDevice(kAudioObjectUnknown, false));

  AudioObjectID inId = AudioDeviceUtils::GetDefaultDeviceId(true);
  if (validId(inId)) {
    // It's ok to set current default input device to default input device again.
    assert(AudioDeviceUtils::SetDefaultDevice(inId, true));
    if (!AudioDeviceUtils::IsOutput(inId)) {
      // Surprisingly it's ok to set default output device to a non-output device!
      assert(AudioDeviceUtils::SetDefaultDevice(inId, false));
    }
  }

  AudioObjectID outId = AudioDeviceUtils::GetDefaultDeviceId(false);
  if (validId(outId)) {
    // It's ok to set current default output device to default input device again.
    assert(AudioDeviceUtils::SetDefaultDevice(outId, false));
    if (!AudioDeviceUtils::IsInput(outId)) {
      // Surprisingly it's ok to set default intput device to a non-input device!
      assert(AudioDeviceUtils::SetDefaultDevice(outId, true));
    }
  }

  // It's ok to change the default input device if there are more than 1
  // input devices. Otherwise, it's failed to do that.
  assert((getDeviceIds(true).size() > 1) == changeDefaultDevice(true));

  // It's ok to change the default output device if there are more than 1
  // output devices. Otherwise, it's failed to do that.
  assert((getDeviceIds(false).size() > 1) == changeDefaultDevice(false));
}

int main()
{
  testGetDefaultDeviceId();
  testGetDeviceName();
  testIsInput();
  testIsOutput();
  testSetDefaultDevice();
  testGetAllDeviceIds();
  return 0;
}