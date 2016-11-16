#pragma once
#ifndef ENET_NETWORK_MUTEX_H
#define ENET_NETWORK_MUTEX_H

#ifndef INTERROGATE

struct NetworkMutexHolder {
  NetworkMutexHolder();
  ~NetworkMutexHolder();
};

#define NETWORK_THREAD_SAFE NetworkMutexHolder _lock_;

#endif
#endif
