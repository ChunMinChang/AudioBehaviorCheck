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
  vector<AudioObjectID> outs = AudioDeviceUtils::GetOutputDevicesID();
  printDevicesID(outs);
  vector<string> outNames = AudioDeviceUtils::GetDevicesName(outs);
  printDevicesName(outNames);

  vector<AudioObjectID> ins = AudioDeviceUtils::GetInputDevicesID();
  printDevicesID(ins);
  vector<string> inNames = AudioDeviceUtils::GetDevicesName(ins);
  printDevicesName(inNames);

  return 0;
}