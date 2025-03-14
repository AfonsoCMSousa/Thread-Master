# Thread-Master
A high-level thread management library for C that abstracts the complexity of working with pthreads. It allows easy creation, assignment, and management of worker threads.

## Introduction
Hey, I'm Afonso, a passionate developer who loves low-level C/C++ and even some assembly!  

I originally created this library as a proof-of-concept, but it quickly evolved into something much bigger.  
If you're interested in learning how it works, check out the [GUIDE.md](https://github.com/AfonsoCMSousa/Thread-Master/blob/master/GUIDE.md) and the `main.c` file, along with all the headers in the `include/` directory.

Of course! Feel free to create a new [issue](https://github.com/AfonsoCMSousa/Thread-Master/issues) if you ever find any when using the library.

---

## Features
- **Thread Pool Management** – Efficiently manages a pool of worker threads.
- **Dynamic Task Assignment** – Assign tasks to available threads dynamically.
- **Thread Safety** – Built-in mutex handling for safe multithreading.
- **Lightweight & Fast** – Optimized for minimal overhead.
- **Cross-Platform Compatibility** – Works on Linux and Windows (via pthreads).

---

## Getting Started

### Requirements
- **C Compiler** (GCC, Clang, MSVC)
- **CMake** (optional, for easier builds)
- **pthreads** (POSIX thread library)

### Installation
Clone the repository:
```sh
git clone https://github.com/AfonsoCMSousa/Thread-Master.git
cd Thread-Master
```

Or compiple manually:
```sh
gcc -o my_program src/main.c include/threadlib/threadlib.c -pthread
```

Check the [releases](https://github.com/AfonsoCMSousa/Thread-Master/releases/latest) for a detailed instalation guide and some libraries (.a) (.dll) or even (.lib).

## All Available Functions

### thread_master_init

```c
void thread_master_init(int max_threads, void *(*__thread_worker__)(void *), void *custom_params);
```
Initializes the thread master with a specified number of threads and a worker function.

- `max_threads`: The maximum number of threads to be used.
- `__thread_worker__`: The function pointer to the worker function that each thread will execute.
- `custom_params`: Custom parameters to be passed to the worker function.

### thread_mutex_init

```c
void thread_mutex_init(pthread_mutex_t *mutex);
```
Initializes a mutex.

- `mutex`: Pointer to the mutex to be initialized.

### thread_mutex_destroy

```c
void thread_mutex_destroy(pthread_mutex_t *mutex);
```
Destroys a mutex.

- `mutex`: Pointer to the mutex to be destroyed.

### thread_master_free

```c
void thread_master_free();
```
Frees resources allocated by the thread master.

### thread_master_get_status

```c
void thread_master_get_status();
```
Retrieves the status of the thread master.

### thread_master_assign_new_job

```c
void thread_master_assign_new_job(void *(*__thread_worker__)(void *), void *custom_params);
```
Assigns a new job to the thread workers.

- `__thread_worker__`: The function pointer to the worker function that each thread will execute.
- `custom_params`: Custom parameters to be passed to the worker function.

## File Structure

- `src/main.c`: Contains the main function and the thread worker example of use.
- `include/threadlib/threadlib.h`: Header file for the thread library.
- `include/threadlib/threadlib.c`: Implementation of the thread library.

## A big thanks to:
- Professor **Marco Ferreira**
- Professor **Patrício Domingues**

## Licence:
This content falls under [Apache License Version 2.0](https://github.com/AfonsoCMSousa/Thread-Master/blob/master/LICENSE)

## Contact
Have questions or suggestions? Feel free to reach out!
- 📧 Email: <afonsoclerigomendessousa@gmail.com>
- 🐙 GitHub: AfonsoCMSousa
