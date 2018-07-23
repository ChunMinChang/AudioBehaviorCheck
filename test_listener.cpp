#include "AudioDeviceListener.h"
#include "AudioObjectUtils.h"
#include <cassert>    // for assert
#include <iostream>   // for std::cout, std::endl
#include <pthread.h>  // for pthread_self()
#include <unistd.h>   // for sleep()

using std::cout;
using std::endl;

AudioObjectUtils::Scope Input = AudioObjectUtils::Input;
AudioObjectUtils::Scope Output = AudioObjectUtils::Output;

string getDefaultDeviceName(AudioObjectUtils::Scope aScope)
{
  AudioObjectID id = AudioObjectUtils::GetDefaultDeviceId(aScope);
  return AudioObjectUtils::GetDeviceName(id);
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

bool gDeviceChanged = false;
/* DeviceChangeCallback */
void OnDeviceChanged() {
  cout << "(thread " << pthread_self() << ") Device Changed!" << endl;
  gDeviceChanged = true;
}

void testChangeDefaultDevice(AudioObjectUtils::Scope aScope)
{
  gDeviceChanged = false; // Clear test flag.

  cout << "Default " << (aScope == Input ? "intput" : "output")
       << " device: " << getDefaultDeviceName(aScope) << endl;
  bool willBeChanged = changeDefaultDevice(aScope);
  if (willBeChanged) {
    while(!gDeviceChanged) {
      // Force to context-switch by sleeping for 100 millisecond.
      usleep(100000);
    };
    cout << "is changed to " << getDefaultDeviceName(aScope) << endl;
  } else {
    cout << endl << "is unable to be changed!" << endl;
  }
}

int main()
{
  cout << "Run test on main thread: " << pthread_self() << endl;
  AudioDeviceListener adl(&OnDeviceChanged);

  testChangeDefaultDevice(Input);
  testChangeDefaultDevice(Output);

  return 0;
}