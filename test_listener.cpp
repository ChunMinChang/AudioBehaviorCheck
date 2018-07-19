#include "AudioDeviceListener.h"
#include "AudioDeviceUtils.h"
#include <cassert>    // for assert
#include <iostream>   // for std::cout, std::endl
#include <pthread.h>  // for pthread_self()

using std::cout;
using std::endl;

AudioDeviceUtils::Scope Input = AudioDeviceUtils::Input;
AudioDeviceUtils::Scope Output = AudioDeviceUtils::Output;

string getDefaultDeviceName(AudioDeviceUtils::Scope aScope)
{
  AudioObjectID id = AudioDeviceUtils::GetDefaultDeviceId(aScope);
  return AudioDeviceUtils::GetDeviceName(id);
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
  cout << "Changing default input device from " << getDefaultDeviceName(Input) << endl;
  bool inputChanged = changeDefaultDevice(Input);
  if (inputChanged) {
    while(!gDeviceChanged) {};
    gDeviceChanged = false;
    cout << "to " << getDefaultDeviceName(Input) << endl;
  } else {
    cout << endl << "Unable to change!" << endl;
  }

  cout << "Try changing default output device from " << getDefaultDeviceName(Output) << endl;
  assert(!gDeviceChanged);
  bool outputChanged = changeDefaultDevice(Output);
  if (outputChanged) {
    while(!gDeviceChanged) {};
    cout << "to " << getDefaultDeviceName(Output) << endl;
  } else {
    cout << endl << "Unable to change!" << endl;
  }

  return 0;
}