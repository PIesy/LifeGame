TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_LFLAGS += -lGL -lGLEW -lGLU -lpthread -lSDL2

SOURCES += main.c \
    math_ext.c \
    optlib.c \
    core.c \
    renderer.c \
    render_queue.c \
    mesh_loader.c

HEADERS += \
    math_ext.h \
    optlib.h \
    core.h \
    core_defs.h \
    renderer.h \
    render_queue.h \
    render_defs.h \
    mesh_loader.h

OTHER_FILES += \
    VertexShader.glsl \
    FragmentShader.glsl \
    square.txt

