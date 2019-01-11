QT       += core gui opengl

myViewer{
    TARGET = myViewer
}

parser {
    TARGET = parser
}

TEMPLATE = subdirs
SUBDIRS += trimesh2 viewer
CONFIG += ordered debug
