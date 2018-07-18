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

string getDefaultDeviceName(bool isInput)
{
  AudioObjectID id = AudioDeviceUtils::GetDefaultDeviceId(isInput);
  return AudioDeviceUtils::GetDeviceName(id);
}

void changeDefaultDevice(bool isInput) {
  cout << "Change default " << (isInput ? "input" : "output")
       << " device from : " << getDefaultDeviceName(isInput) << " to : "
       << (AudioDeviceUtils::ChangeDefaultDevice(isInput)
             ? getDefaultDeviceName(isInput)
             : " Fail to change!") << endl;
}

int main()
{
  vector<AudioObjectID> ids = AudioDeviceUtils::GetAllDeviceIds();
  printDevicesID(ids);

  vector<AudioObjectID> ins = AudioDeviceUtils::GetDeviceIds(true);
  printDevicesID(ins);

  vector<AudioObjectID> outs = AudioDeviceUtils::GetDeviceIds(false);
  printDevicesID(outs);

  changeDefaultDevice(true);
  changeDefaultDevice(false);

  return 0;
}