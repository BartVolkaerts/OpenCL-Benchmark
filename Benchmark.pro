TARGET = Benchmark

QT += opengl

LIBS += -lOpenCL -levent -lqwt -lm
INCLUDEPATH += /usr/include/qwt

OBJECTS_DIR = ./build
MOC_DIR = ./build
RCC_DIR = ./build
UI_DIR = ./build

HEADERS +=\
    main_window.h\
    environment.h\
    base_benchmark.h\
    flops/flops_benchmark.h\
    flops/flops_main_widget.h\
    flops/flops_config_widget.h\
    readwrite/read_write_benchmark.h\

SOURCES +=\
    main.cpp\
    main_window.cpp\
    environment.cpp\
    base_benchmark.cpp\
    flops/flops_benchmark.cpp\
    flops/flops_main_widget.cpp\
    flops/flops_config_widget.cpp\
    readwrite/read_write_benchmark.cpp\

FORMS += main_window.ui\
    flops/flops_main_widget.ui\
    flops/flops_config_widget.ui\
