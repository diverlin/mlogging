#include "logging.hpp"

int main() {
    custom::Logging& logging = custom::Logging::instance();

    logging.log("orders/cat1", "message1", 1, 2.0f, "yes");
    logging.log("orders_extra/cat2", "message2");
    logging.log("orders_extra/cat3", "message3");
    logging.log("orders/cat1", "message4", "proof");
    logging.log("orders/cat1", "message5", 1, 1);
    logging.log("orders/cat1", "message6");
    logging.log("orders/cat1", "message7");

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

#ifdef WIN32
    logging.setRootPath("C:/workspace/tmp/new");
#else
    logging.setRootPath("/workspace/tmp/new");
#endif
    logging.log("orders/cat1", "message8");
    logging.log("orders/cat1", "message9");
    logging.log("orders/cat1", "message10");
    logging.log("orders/cat2", "message11");
    logging.log("orders/cat3", "message12");

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    return 0;
}
