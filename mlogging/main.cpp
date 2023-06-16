#include <logging.hpp>
#include <chrono>
#include <iostream>

int main() {
    custom::Logging logging;

    // Put threads to sleep

    // Wake up thread with index 1
    logging.log("msg1", "cat1");
    logging.log("msg2", "cat2");
    logging.log("msg3", "cat3");
    logging.log("msg4", "cat1");
    logging.log("msg5", "cat1");
    logging.log("msg6", "cat1");
    logging.log("msg7", "cat1");

    std::cout << "1 main thread=" << std::this_thread::get_id() << std::endl;

    // Continue other tasks
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    std::cout << "2 main thread=" << std::this_thread::get_id() << std::endl;

    return 0;
}