TARGET = Benchmark

LIBS += -lOpenCL -levent

OBJECTS_DIR = ./build
MOC_DIR = ./build
RCC_DIR = ./build
UI_DIR = ./build

HEADERS +=\
    main_window.h\
    environment.h\

SOURCES +=\
    main.cpp\
    main_window.cpp\
    environment.cpp\

FORMS += main_window.ui\
