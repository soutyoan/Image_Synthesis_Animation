# if (mac OS X)
DESTDIR = ../viewer
# ENDIF
QT       += core gui opengl

macx {
  QMAKE_CXXFLAGS += -Wno-unknown-pragmas
} else {
  QMAKE_LFLAGS += -Wno-unknown-pragmas -fopenmp
}

### TARGET myViewer, compiling entire project
myViewer{

    TARGET = myViewer
    TEMPLATE = app
    SOURCES +=  \
                src/main.cpp \
                src/openglwindow.cpp \
                src/glshaderwindow.cpp \
                src/joint.cpp

    HEADERS  += \
                src/openglwindow.h \
                src/glshaderwindow.h \
                src/perlinNoise.h \
                src/joint.h \
                src/glshaderwindow-animation.h \
                src/openglwindow-animation.h

}

### TARGET Parser to test the parser
parser {
    TARGET = parser
    TEMPLATE = app
    SOURCES +=  \
                test/test_bvh_creation.cpp \
                src/joint.cpp

    HEADERS  += \
                src/joint.h \
}


# trimesh library for loading objects.
# Reference/source: http://gfx.cs.princeton.edu/proj/trimesh2/
INCLUDEPATH += ../trimesh2/include/

LIBS += -L../trimesh2/lib -ltrimesh
