QT       += core gui charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = era_standalone_dashboard
TEMPLATE = app

INCLUDEPATH += ../../include

SOURCES += \
    main_standalone.cpp \
    standalone_dashboard.cpp \
    ../../src/resource/cpu/cpu_monitor.cpp \
    ../../src/resource/cpu/cpu_optimizer.cpp \
    ../../src/resource/memory/memory_monitor.cpp \
    ../../src/resource/memory/memory_optimizer.cpp \
    ../../src/resource/power/power_monitor.cpp \
    ../../src/resource/power/power_optimizer.cpp \
    ../../src/genetic/chromosome.cpp \
    ../../src/genetic/crossover.cpp \
    ../../src/genetic/mutation.cpp \
    ../../src/genetic/population.cpp \
    ../../src/genetic/fitness.cpp

HEADERS += \
    standalone_dashboard.h

# For Unix-like systems, ensure math library is linked
unix: LIBS += -lm

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
