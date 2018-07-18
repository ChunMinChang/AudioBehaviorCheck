#include "AudioDeviceUtils.h"
#include <iostream>

using std::cout;
using std::endl;

void printDevicesID(vector<AudioObjectID> ids)
{
 for (AudioObjectID id: ids) {
   cout << id << " : " << AudioDeviceUtils::GetDeviceName(id) << endl;
   if (AudioDeviceUtils::IsInput(id)) {
     cout << "\tinput" << endl;
   }
   if (AudioDeviceUtils::IsOutput(id)) {
     cout << "\toutput" << endl;
   }
 }
 cout << endl;
}

int main()
{
  vector<AudioObjectID> ids = AudioDeviceUtils::GetAllDeviceIDs();
  printDevicesID(ids);

  AudioObjectID defaultInput = AudioDeviceUtils::GetDefaultDeviceID(true);
  cout << "default input: " << defaultInput << endl;

  AudioObjectID defaultOutput = AudioDeviceUtils::GetDefaultDeviceID(false);
  cout << "default output: " << defaultOutput << endl;

  return 0;
}