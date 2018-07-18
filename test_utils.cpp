#include "AudioDeviceUtils.h"
#include <iostream>

using std::cout;
using std::endl;

void printDevicesID(vector<AudioObjectID> ids)
{
 for (AudioObjectID id: ids) {
   cout << id << endl;
 }
 cout << endl;
}

void printDevicesName(vector<string> names)
{
 for (string name: names) {
   cout << name << endl;
 }
 cout << endl;
}

int main()
{
  vector<AudioObjectID> ids = AudioDeviceUtils::GetAllDeviceIDs();
  printDevicesID(ids);

  vector<string> names = AudioDeviceUtils::GetAllDeviceNames();
  printDevicesName(names);

  AudioObjectID defaultInput = AudioDeviceUtils::GetDefaultDeviceID(true);
  cout << "default input: " << defaultInput << endl;

  AudioObjectID defaultOutput = AudioDeviceUtils::GetDefaultDeviceID(false);
  cout << "default output: " << defaultOutput << endl;

  return 0;
}