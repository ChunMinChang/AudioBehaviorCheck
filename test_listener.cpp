#include "AudioDeviceListener.h"
#include "AudioDeviceUtils.h"
#include <cassert>    // for assert
#include <iostream>   // for std::cout, std::endl
#include <pthread.h>  // for pthread_self()
#include <unistd.h>   // for sleep()

using std::cout;
using std::endl;

AudioDeviceUtils::Scope Input = AudioDeviceUtils::Input;
AudioDeviceUtils::Scope Output = AudioDeviceUtils::Output;

string getDefaultDeviceName(AudioDeviceUtils::Scope aScope)
{
  AudioObjectID id = AudioDeviceUtils::GetDefaultDeviceId(aScope);
  return AudioDeviceUtils::GetDeviceName(id);
}

bool changeDefaultDevice(AudioDeviceUtils::Scope aScope)
{
  vector<AudioObjectID> ids = AudioDeviceUtils::GetDeviceIds(aScope);
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
    while(!gDeviceChanged) {
      // Force to context-switch by sleeping for 100 millisecond.
      usleep(100000);
    };
    gDeviceChanged = false;
    cout << "to " << getDefaultDeviceName(Input) << endl;
  } else {
    cout << endl << "Unable to change!" << endl;
  }

  cout << "Try changing default output device from " << getDefaultDeviceName(Output) << endl;
  assert(!gDeviceChanged);
  bool outputChanged = changeDefaultDevice(Output);
  if (outputChanged) {
    while(!gDeviceChanged) {
      // Force to context-switch by sleeping for 100 millisecond.
      usleep(100000);
    };
    cout << "to " << getDefaultDeviceName(Output) << endl;
  } else {
    cout << endl << "Unable to change!" << endl;
  }

  return 0;
}