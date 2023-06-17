#include <logging.hpp>
#include <currentdatetimeutc.hpp>

#include <iostream>

int main() {
    CurrentDateTimeUTC dt;

    std::cout << dt.dateStr() << std::endl;
    std::cout << dt.timeStr() << std::endl;
    std::cout << dt.dateTimeStr() << std::endl;

    custom::Logging logging;

    // Wake up thread with index 1
    logging.log("msg1", "cat1");
    logging.log("msg2", "cat2");
    logging.log("msg3", "cat3");
    logging.log("msg4", "cat1");
    logging.log("msg5", "cat1");
    logging.log("msg6", "cat1");
    logging.log("msg7", "cat1");

    // Continue other tasks
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    return 0;
}
