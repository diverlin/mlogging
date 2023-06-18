# mlogging

This code requires C++17 standard and has been tested on MS Visual Studio 2019 and QtCreator with mingw 11.2 that supports C++17.

- The Logging class utilizes the variadic template feature, providing a convenient way to pass multiple arguments of different simple types.
- When a new location pattern is received as the first argument, the Logging class creates a new thread. The thread then enters a sleep state using a condition variable.
- A condition variable is used to put the thread into sleep mode, preventing continuous race conditions, avoiding the need for constant mutex lock/unlock overhead.
- When a new log request is received, the Logging wakes up all sleeping threads. Only the thread with the required pattern proceeds to process the message queue, while the other threads go back to sleep.
- It is possible to change the root location for logging during runtime. Note that the thread is not recreated in this case since the thread depends solely on the location pattern (relative path).
- The Logging class follows the singleton pattern to prevent issues where multiple Logging instances with the same location pattern could exist. This ensures that only a single worker thread is associated with a specific location pattern.
