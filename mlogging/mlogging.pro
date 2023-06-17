TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

DEFINES += ENABLE_MLOGGER_DEBUG

SOURCES += \
        main.cpp \
        logging.cpp \
        currentdatetimeutc.cpp \
        fsutils.cpp \

HEADERS += \
        logging.hpp \
        currentdatetimeutc.hpp \
        fsutils.hpp \
