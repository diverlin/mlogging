#include <logging.hpp>

int main() {
    custom::Logging logging("/workspace/tmp");

    logging.log("message1", "orders/cat1");
    logging.log("message2", "orders_extra/cat2");
    logging.log("message3", "orders_extra/cat3");
    logging.log("message4", "orders/cat1");
    logging.log("message5", "orders/cat1");
    logging.log("message6", "orders/cat1");
    logging.log("message7", "orders/cat1");

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    logging.setRootPath("/workspace/tmp/new");
    logging.log("message8", "orders/cat1");

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    return 0;
}
