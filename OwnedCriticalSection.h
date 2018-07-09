// Reference: https://github.com/kinetiknz/cubeb/blob/master/src/cubeb_utils_unix.h

#ifndef OWNEDCRITICALSECTION_H
#define OWNEDCRITICALSECTION_H

#include <pthread.h>

/* This wraps a critical section to track the owner in ERRORCHECK mode. */
class OwnedCriticalSection
{
public:
  enum Mode
  {
    NORMAL,
    ERRORCHECK
  };

  OwnedCriticalSection(Mode aMode = NORMAL)
    : mMode(aMode)
  {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, mMode == NORMAL ? PTHREAD_MUTEX_NORMAL :
                                                       PTHREAD_MUTEX_ERRORCHECK);
    assert(!pthread_mutex_init(&mMutex, &attr));
    pthread_mutexattr_destroy(&attr);
  }

  ~OwnedCriticalSection()
  {
    assert(!pthread_mutex_destroy(&mMutex));
  }

  void lock()
  {
    assert(!pthread_mutex_lock(&mMutex));
  }

  void unlock()
  {
    assert(!pthread_mutex_unlock(&mMutex));
  }

  void assertCurrentThreadOwns()
  {
    assert(mMode == ERRORCHECK);
    // EDEADLK: A deadlock condition was detected or the current thread
    // already owns the mutex.
    assert(pthread_mutex_lock(&mMutex) == EDEADLK);
  }

private:
  pthread_mutex_t mMutex;
  Mode mMode;

  // Disallow copy and assignment because pthread_mutex_t cannot be copied.
  OwnedCriticalSection(const OwnedCriticalSection&);
  OwnedCriticalSection& operator=(const OwnedCriticalSection&);
};

#endif /* OWNEDCRITICALSECTION_H */
