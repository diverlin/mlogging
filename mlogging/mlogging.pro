TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

DEFINES += ENABLE_MLOGGER_DEBUG

SOURCES += \
        main.cpp \
        logging.cpp \

HEADERS += \
        logging.hpp
