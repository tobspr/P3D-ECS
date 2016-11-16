
#include "network_mutex.h"
#include <mutex>
#include <iostream>
#include <thread>

std::mutex global_networking_mutex;

/*
  Right now, a mutex produces a deadlock. It will only be required later on in the
  C+ side.
*/

NetworkMutexHolder::NetworkMutexHolder() {
    //global_networking_mutex.lock();
}

NetworkMutexHolder::~NetworkMutexHolder() {
    //global_networking_mutex.unlock();
}
