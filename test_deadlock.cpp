// Deadlock
//
// In CoreAudio, the ouput callback will holds a mutex shared with AudioUnit
// (hereinafter mutex_AU). Thus, if the callback requests another mutex M held
// by another thread, without releasing mutex_AU, then it will cause a
// deadlock when another thread holding the mutex M requests to use AudioUnit.
//
// The following figure illustrates the deadlock described above:
//
//    (Thread A)     holds
//  data_callback <---------- mutext_AudioUnit(mutex_AU)
//      |                            ^
//      |                            |
//      | request                    | request
//      |                            |
//      v           holds            |
//   mutex_M -------------------> Thread B
#include "AudioStream.h"          // for AudioStream
#include "OwnedCriticalSection.h" // for OwnedCriticalSection
#include "utils.h"                // for LOG
#include <assert.h>               // for assert
#include <pthread.h>              // for pthread
#include <signal.h>               // for signal
#include <unistd.h>               // for sleep, usleep

// The signal alias for calling our thread killer.
#define CALL_THREAD_KILLER SIGUSR1

const double kFequency = 44100.0;
const unsigned int kChannels = 2;

// If we apply ERRORCHECK mode, then we can't unlock a mutex locked by a
// different thread.
// OwnedCriticalSection gMutex(OwnedCriticalSection::Mode::ERRORCHECK);
OwnedCriticalSection gMutex;
using locker = std::lock_guard<OwnedCriticalSection>;

// Indicating whether the test is passed.
bool gPass = false;

// Indicating whether the data callback is fired.
bool gCalled = false;

// Indicating whether the data callback is running.
bool gCalling = false;

// Indicating whether the assigned task is done.
bool gTaskDone = false;

// Indicating whether our pending task thread is killed by ourselves.
bool gKilled = false;

void killer(int aSignal)
{
  assert(aSignal == CALL_THREAD_KILLER);
  LOG("pending task thread is killed!\n");
  gKilled = true;
}

uint64_t getThreadId(pthread_t aThread = NULL)
{
  uint64_t tid;
  // tid will be current thread id if aThread is null.
  pthread_threadid_np(aThread, &tid);
  return tid;
}

// The output callback fired from audio rendering mechanism, which is on
// out-of-main thread.
void callback(void* aBuffer, unsigned long aFrames)
{
  // The callback thread holds a mutex shared with AudioUnit.

  gCalling = true;

  uint64_t id = getThreadId();
  !gCalled && LOG("Output callback is on thread %llu, holding mutex_AU\n", id);
  gCalled = true;

  if (!gTaskDone) {
    // Force to switch threads by sleeping 10 ms. Notice that anything over
    // 10ms would produce a glitch. It's intended for testing deadlock,
    // so we ignore the fault here.
    LOG("[%llu] Force to switch threads\n", id);
    usleep(10000);
  }

  LOG("[%llu] Try getting another mutex: gMutex...\n", id);
  locker guard(gMutex);

  LOG("[%llu] Got mutex finally!\n", id);

  gCalling = false;
}

void* task(void*)
{
  // Hold the mutex.
  locker guard(gMutex);

  uint64_t id = getThreadId();
  LOG("Task thread: %llu, holding gMutex, is created\n", id);

  while(!gCalling) {
    LOG("[%llu] waiting for output callback before running task\n", id);
    usleep(1000); // Force to switch threads by sleeping 1 ms.
  }

  // Creating another AudioUnit when we already had one will cause a deadlock!
  LOG("[%llu] Try creating another AudioUnit (getting mutex_AU)...\n", id);
  AudioStream as(AudioStream::F32LE, kChannels, kFequency, callback);

  LOG("[%llu] Another AudioUnit is created!\n", id);
  gTaskDone = true;

  return NULL;
}

// We provide one possible solution here:
// void* task(void*)
// {
//   uint64_t id = getThreadId();
//   LOG("Task thread: %llu is created\n", id);
//
//   while(!gCalling) {
//     LOG("[%llu] waiting for output callback before running task\n", id);
//     usleep(1000); // Force to switch threads by sleeping 1 ms.
//   }
//
//   // Creating another AudioUnit when we already had one will cause a deadlock!
//   LOG("[%llu] Try creating another AudioUnit (getting mutex_AU)...\n", id);
//   AudioStream as(AudioStream::Format::F32LE, kFequency, kChannels, callback);
//
//   LOG("[%llu] Another AudioUnit is created!\n", id);
//
//   // Hold the mutex.
//   LOG("[%llu] Try getting another mutex: gMutex...\n", id);
//   locker guard(gMutex);
//
//   LOG("[%llu] Got mutex finally!\n", id);
//
//   gTaskDone = true;
//
//   return NULL;
// }

void* watchdog(void* aSubject)
{
  uint64_t id = getThreadId();

  pthread_t subject = *((pthread_t *) aSubject);
  uint64_t sid = getThreadId(subject);

  LOG("Monitor thread %llu on thread %llu\n", sid, id);

  unsigned int sec = 1;
  LOG("[%llu] sleep %d seconds before checking task for thread %llu\n", id, sec, sid);
  sleep(sec); // Force to switch threads.

  if (!gTaskDone) {
    LOG("[%llu] Kill the task thread %llu!\n", id, sid);
    assert(!pthread_kill(subject, CALL_THREAD_KILLER));
    assert(!pthread_detach(subject));
    // The mutex held by the killed thread(subject) won't be released,
    // so we need unlock it manually. Notice that we can't unlock a mutex held
    // by other thread in OwnedCriticalSection::Mode::ERRORCHECK mode of gMutex.
    gMutex.unlock();
  }

  LOG("\n[%llu] Task is %sdone\n\n", id, gTaskDone ? "": "NOT ");
  gPass = gTaskDone;

  return NULL;
}

int main()
{
  AudioStream as(AudioStream::F32LE, kChannels, kFequency, callback);

  // Install signal handler.
  signal(CALL_THREAD_KILLER, killer);

  pthread_t subject, detector;
  pthread_create(&subject, NULL, task, NULL);
  pthread_create(&detector, NULL, watchdog, (void *) &subject);

  as.Start();

  pthread_join(subject, NULL);
  pthread_join(detector, NULL);

  as.Stop();

  // If the callback is never fired, then the task must not be processed.
  // No need to keep checking in this case.
  assert(gCalled && "Callback should be fired!");

  // The task thread might keep running after the deadlock is freed, so we use
  // gPass instead of gTaskDone.
  assert(gPass && "Deadlock detected!");

  // False gPass implies there is a deadlock detected, so we need to kill the
  // pending task thread to free the deadlock and set gKilled to true.
  // True gPass means there is no deadlock and no need to kill any thread.
  assert(gPass != gKilled && "Killer is out of control!");

  return 0;
}
