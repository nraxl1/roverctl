## 5EID0 Project Orchestration Documentation

### Introduction

> This documentation contains some concepts used in other programming languages.
> Concepts that you should look up are written in ***bold italic***.

This document provides some details about the project structure and the orchestration of the code that will run on the rover. It would be safe to say that understanding the concepts detailed here is a prerequisite for working on the rover code. The documentation will be updated accordingly when new APIs are added.


### APIs Provided:
#### ```queue.h```
Basic implementation of the ***queue*** data structure and functions to manipulate and debug them. This API is not thread-safe and mostly exists as a backend for ```mutex_queue.h```.

#### ```mutex_queue.h```
Provides the data type ```mutex_queue_t``` which can be manipulated like a ```queue_t``` in a thread-safe way. This will be the only secure way of sharing state between threads for reasons detailed later.

### POSIX Threads Explanation:
Using the ```pthreads.h``` library on a POSIX compliant system provided by ```glibc``` we can spawn POSIX threads, creating new lines of execution. This allows the execution of multiple functions at once. If the amount of threads exceeds the physical threads supported by the topology of the CPU, preemptive multitasking is automatically used with some overhead. While cooperative multitasking waits for functions to yield control to give the other functions CPU time, preemptive multitasking forcibly interrupts the execution to give other threads CPU time. This prevents thread starvation if an IO function is overwhelmed and refuses to yield. Forced interruptions happen after the thread gets a predetermined amount of CPU time, called quantum. By default POSIX threads use the```SCHED_OTHER``` scheduler in Linux which supports the use of ***niceness*** values to tweak the relative CPU times of the threads and uses a typical quantum of 1-100ms. ```pthreads``` can use mutexes to share resources which prevents data races by making sure only 1 thread has access to a resource at a given moment.


### Mutex Explanation:
Before accessing or modifying a value in a mutex-protected data structure, you need to lock the ```lock``` field, then unlock it after use.

```
// Lock the mutex
pthread_mutex_lock(&mutex_queue->lock);
// Access the data here
// Unlock the mutex
pthread_mutex_unlock(&mutex_queue->lock);
```
When the mutex is already locked by another thread, the ```pthread_mutex_lock()``` function blocks execution until the thread that locked it unlocks it.
> [!WARNING]
 Attempting to call ```pthread_mutex_lock()``` on a mutex that is already locked by the same thread will result in a ***Deadlock*** which will block execution forever

Similarly, calling ```pthread_mutex_unlock()``` on an unlocked mutex will cause undefined behavior (commonly crashes or corrupts state).

> Note: this standard behavior for ```PTHREAD_MUTEX_NORMAL```, not all mutexes are the same

> [!WARNING]
A mutex-protected value being locked doesn't prevent it from being accessed. It is the user's responsibility to not access the data when the mutex is locked.

A pointer to a mutex-protected value can propagate in the thread and exceed its intended ***lifetime***, still being accessible after the mutex is locked. This is why its restricted to peek into a mutex-protected queue. When the only way to access a node is to dequeue it, the thread calling the dequeue operation becomes the sole ***owner*** of the data dequeued.
```
while(1){
void* value_ptr = mutex_dequeue(mutex_queue);
// value_ptr exists solely within this scope
// The underlying structure outliving value_ptr leads to a memory leak

free(value_ptr); // memory leak averted

} // value_ptr is destroyed at this point, there is no way of freeing the underlying memory

------ OR ------

void* value_ptr = mutex_dequeue(mutex_queue);
value_ptr = some_other_ptr; // value_ptr is also destroyed (overwritten)
```
 The owner ***assumes the responsibility of appropriately destroying the structure contained***.

If a thread crashes while a mutex is locked by it, the mutex becomes ***poisoned***, which C doesn't have a way of detecting.

### Systemd
Systemd is the init system used for most Linux distributions including PYNQ Linux.
> An init system is the first process that the kernel starts (PID 1). It is responsible for booting the system setting up the userspace.

Systemd can be used to create services that monitor and manage the system and fulfill tasks when certain conditions are met. This can be used to automatically trigger reboots, automatically start the rover when the PYNQ boots, handle system-wide errors, etc.

### UNIX Domain Sockets
UNIX sockets are websocket-like interprocess communication endpoints. They work extremely fast and are a primary method of communicating between processes. UDS is standardised in POSIX and very mature, making it widely available in different programming languages. Certain components of the code can be written in alternative programming languages and be integrated with the main codebase through UDS.