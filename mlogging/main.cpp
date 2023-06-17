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
    logging.log("message1", "orders/cat1");
    logging.log("message2", "orders_extra/cat2");
    logging.log("message3", "orders_extra/cat3");
    logging.log("message4", "orders/cat1");
    logging.log("message5", "orders/cat1");
    logging.log("message6", "orders/cat1");
    logging.log("message7", "orders/cat1");

    // Continue other tasks
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    return 0;
}
