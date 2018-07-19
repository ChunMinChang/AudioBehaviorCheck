#include "AudioDeviceUtils.h"

#include <cassert>
#include <iostream>

using std::cout;
using std::endl;

bool validId(AudioObjectID id)
{
  return id != kAudioObjectUnknown;
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

void printDeviceInfo(AudioObjectID id)
{
  string name = AudioDeviceUtils::GetDeviceName(id);
  cout << "Device " << id << " : " << name << " >";
  if (AudioDeviceUtils::IsInput(id)) {
    cout << " input";
  }
  if (AudioDeviceUtils::IsOutput(id)) {
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

int main()
{
  testGetDefaultDeviceId();
  testGetDeviceName();
  testIsInput();
  testIsOutput();
  testGetAllDeviceIds();
  return 0;
}