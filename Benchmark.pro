TARGET = Benchmark

QT += opengl

#DEFINES += GL_SHARING_EXTENSION="cl_khr_gl_sharing"

unix:LIBS += -lOpenCL -levent -lqwt -lm -lopencv_core -lopencv_highgui -lassimp -fopenmp
unix:INCLUDEPATH += /usr/include/qwt /usr/include/assimp
unix:DEFINES += GL_GLEXT_PROTOTYPES

OBJECTS_DIR = ./build
MOC_DIR = ./build
RCC_DIR = ./build
UI_DIR = ./build

QMAKE_CXXFLAGS += -fopenmp

HEADERS +=\
    main_window.h\
    environment.h\
    base_benchmark.h\
    flops/flops_benchmark.h\
    flops/flops_main_widget.h\
    flops/flops_config_widget.h\
    flops/flops_stat_viewer.h\
    readwrite/glwidget.h\
    readwrite/videosource.h\
    galaxy/galaxy_main_widget.h\
    galaxy/galaxy.h\
    galaxy/camera.h\
    readwrite/readwrite.h \
    readwrite/readwriteconfigwidget.h\
    mandelbrot/mandelbrot.h\
    mandelbrot/mandelbrot_main_widget.h\
    mandelbrot/mandelbrot_config_widget.h\
    raytracing/raytracing.h\
    raytracing/raytracing_main_widget.h\
    io_throughput/io_throughput.h\
    io_throughput/io_throughput_main_widget.h\
    io_throughput/io_throughput_config_widget.h\

SOURCES +=\
    main.cpp\
    main_window.cpp\
    environment.cpp\
    base_benchmark.cpp\
    flops/flops_benchmark.cpp\
    flops/flops_main_widget.cpp\
    flops/flops_config_widget.cpp\
    flops/flops_stat_viewer.cpp\
    readwrite/glwidget.cpp\
    readwrite/videosource.cpp\
    galaxy/galaxy_main_widget.cpp\
    galaxy/galaxy.cpp\
    galaxy/camera.cpp\
    readwrite/readwrite.cpp \
    readwrite/readwriteconfigwidget.cpp\
    mandelbrot/mandelbrot.cpp\
    mandelbrot/mandelbrot_main_widget.cpp\
    mandelbrot/mandelbrot_config_widget.cpp\
    raytracing/raytracing.cpp\
    raytracing/raytracing_main_widget.cpp\
    io_throughput/io_throughput.cpp\
    io_throughput/io_throughput_main_widget.cpp\
    io_throughput/io_throughput_config_widget.cpp\

FORMS += main_window.ui\
    flops/flops_main_widget.ui\
    flops/flops_config_widget.ui\
    flops/flops_stat_viewer.ui\
    readwrite/readwriteconfigwidget.ui\
    mandelbrot/mandelbrot_config_widget.ui\
    io_throughput/io_throughput_main_widget.ui\
    io_throughput/io_throughput_config_widget.ui\

