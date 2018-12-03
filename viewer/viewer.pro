# if (mac OS X)
DESTDIR = ../viewer
# ENDIF
QT       += core gui opengl

TARGET = myViewer
TEMPLATE = app

macx {
  QMAKE_CXXFLAGS += -Wno-unknown-pragmas
} else {
  QMAKE_LFLAGS += -Wno-unknown-pragmas -fopenmp
}

SOURCES +=  \
            src/bvhnode.cpp \
            src/trimesh_bvh.cpp \
            src/main.cpp \
            src/openglwindow.cpp \
            src/glshaderwindow.cpp \

HEADERS  += \
            src/bvhnode.h \
            src/trimesh_bvh.h\
            src/openglwindow.h \
            src/glshaderwindow.h \
            src/perlinNoise.h \

# trimesh library for loading objects.
# Reference/source: http://gfx.cs.princeton.edu/proj/trimesh2/
INCLUDEPATH += ../trimesh2/include/

LIBS += -L../trimesh2/lib -ltrimesh
