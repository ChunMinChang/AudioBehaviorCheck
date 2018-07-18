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
  vector<AudioObjectID> ids = AudioDeviceUtils::GetAllDeviceIds();
  printDevicesID(ids);

  AudioObjectID defaultInput = AudioDeviceUtils::GetDefaultDeviceId(true);
  cout << "default input: " << defaultInput << endl;

  AudioObjectID defaultOutput = AudioDeviceUtils::GetDefaultDeviceId(false);
  cout << "default output: " << defaultOutput << endl;

  cout << endl;

  vector<AudioObjectID> ins = AudioDeviceUtils::GetDeviceIds(true);
  printDevicesID(ins);

  vector<AudioObjectID> outs = AudioDeviceUtils::GetDeviceIds(false);
  printDevicesID(outs);

  return 0;
}