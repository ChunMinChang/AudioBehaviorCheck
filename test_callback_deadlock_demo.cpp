// Deadlock
//
// In CoreAudio, the ouput callback will hold a mutex shared with AudioUnit,
// denoted by mutex_AU. Thus, if the callback requests another mutex M held
// by another thread, without releasing mutex_AU, then it will cause a
// deadlock when another thread holding the mutex M requests to use AudioUnit.
//
// The following figure illustrates the deadlock described above:
//
//    (Thread A)     holds
//  data_callback <---------- mutex within AudioUnit (mutex_AU)
//      |                            ^
//      |                            |
//      | request                    | request
//      |                            |
//      v           holds            |
//   mutex M -------------------> Thread B
//
// This program demonstrates the case above.

#include "AudioStream.h"  // AudioStream
#include <assert.h>       // assert
#include <chrono>         // std::chrono::seconds
#include <iostream>       // std::cout, std::endl
#include <mutex>          // std::mutex
#include <thread>         // std::thread, std::this_thread

const double kFequency = 44100.0;
const unsigned int kChannels = 2;

bool CALLBACK_STARTED = false;
bool CALLBACK_ENDED = false;
bool TASK_STARTED = false;
bool TASK_ENDED = false;

std::mutex MUTEX;

void another_callback(void* aBuffer, unsigned long aFrames) {
  // The callback thread holds a mutex shared with AudioUnit.
  std::thread::id id = std::this_thread::get_id();
  std::cout << "[" << id << "] " << "On another callback thread: " << id << std::endl;
}

void create_audio_stream()
{
  TASK_STARTED = true;
  std::lock_guard<std::mutex> lock(MUTEX);

  // The callback thread holds a mutex shared with AudioUnit.
  std::thread::id id = std::this_thread::get_id();
  std::cout << "[" << id << "] " << "On task thread: " << id << std::endl;

  while (!CALLBACK_STARTED) {
    std::cout << "[" << id << "] " << "Wait until callback starts ..." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(10)); // sleep for 10 ms.
  }

  std::cout << "[" << id << "] " << "Create another audio stream." << std::endl;
  AudioStream as(AudioStream::F32LE, kChannels, kFequency, another_callback);
  std::cout << "[" << id << "] " << "Another audio stream is created!" << std::endl;

  TASK_ENDED = true;
}

// The output callback fired from audio rendering mechanism, which is on
// out-of-main thread.
void callback(void* aBuffer, unsigned long aFrames)
{
  CALLBACK_STARTED = true;

  // The callback thread holds a mutex shared with AudioUnit.
  std::thread::id id = std::this_thread::get_id();
  std::cout << "[" << id << "] " << "On callback thread: " << id << std::endl;

  while (!TASK_STARTED) {
    std::cout << "[" << id << "] " << "Wait to create another audio stream ..." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1)); // sleep for 1 ms.
  }

  std::cout << "[" << id << "] " << "Wait until another audio stream is created ..." << std::endl;
  std::lock_guard<std::mutex> lock(MUTEX);

  CALLBACK_ENDED = true;
}

int main()
{
  AudioStream as(AudioStream::F32LE, kChannels, kFequency, callback);
  as.Start();
  std::thread task(create_audio_stream); // Spawn new thread that calls create_audio_stream().
  task.join(); // Pauses until task finishes.
  as.Stop();
  while (!CALLBACK_ENDED || !TASK_ENDED) {
    std::cout << "Wait until callback and task are called ..." << std::endl;
  }
  return 0;
}
