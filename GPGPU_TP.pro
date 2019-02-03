QT       += core gui opengl testlib

myViewer{
    TARGET = myViewer
}

parser {
    TARGET = parser
}

tests {
    TARGET = tests
}

TEMPLATE = subdirs
SUBDIRS += trimesh2 viewer
CONFIG += ordered debug
