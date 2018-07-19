#include "AudioDeviceListener.h"
#include "AudioDeviceUtils.h"
#include <cassert>    // for assert
#include <iostream>   // for std::cout, std::endl
#include <pthread.h>  // for pthread_self()

using std::cout;
using std::endl;

string getDefaultDeviceName(bool aInput)
{
  AudioObjectID id = AudioDeviceUtils::GetDefaultDeviceId(aInput);
  return AudioDeviceUtils::GetDeviceName(id);
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

bool gDeviceChanged = false;
/* DeviceChangeCallback */
void OnDeviceChanged() {
  cout << "(thread " << pthread_self() << ") Device Changed!" << endl;
  gDeviceChanged = true;
}

int main()
{
  cout << "Run test on main thread: " << pthread_self() << endl;
  AudioDeviceListener adl(&OnDeviceChanged);

  assert(!gDeviceChanged);
  cout << "Changing default input device from " << getDefaultDeviceName(true) << endl;
  bool inputChanged = changeDefaultDevice(true);
  if (inputChanged) {
    while(!gDeviceChanged) {};
    gDeviceChanged = false;
    cout << "to " << getDefaultDeviceName(true) << endl;
  } else {
    cout << endl << "Unable to change!" << endl;
  }

  cout << "Try changing default output device from " << getDefaultDeviceName(false) << endl;
  assert(!gDeviceChanged);
  bool outputChanged = changeDefaultDevice(false);
  if (outputChanged) {
    while(!gDeviceChanged) {};
    cout << "to " << getDefaultDeviceName(false) << endl;
  } else {
    cout << endl << "Unable to change!" << endl;
  }

  return 0;
}