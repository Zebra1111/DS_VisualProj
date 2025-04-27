QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    avltree.cpp \
    bsttree.cpp \
    btree.cpp \
    chighlighter.cpp \
    graph.cpp \
    graphicsview.cpp \
    main.cpp \
    mainScene.cpp \
    queue.cpp \
    singlylist.cpp \
    stack.cpp \
    visualbtree.cpp \
    visualbubblesort.cpp \
    visualbucketsort.cpp \
    visualgraph.cpp \
    visualheap.cpp \
    visualqueue.cpp \
    visualquicksort.cpp \
    visualsearch.cpp \
    visualsinglylist.cpp \
    visualstack.cpp \
    visualtree.cpp

HEADERS += \
    avltree.h \
    binarytree.h \
    bsttree.h \
    btree.h \
    chighlighter.h \
    graph.h \
    graphicsview.h \
    mainScene.h \
    queue.h \
    singlylist.h \
    stack.h \
    visualbtree.h \
    visualbubblesort.h \
    visualbucketsort.h \
    visualgraph.h \
    visualheap.h \
    visualqueue.h \
    visualquicksort.h \
    visualsearch.h \
    visualsinglylist.h \
    visualstack.h \
    visualtree.h

FORMS += \
    mainScene.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
