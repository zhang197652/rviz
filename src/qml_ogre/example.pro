CONFIG += qt
QT += qml quick
TEMPLATE = app
TARGET = qml_ogre

INCLUDEPATH += ./ \
            += += /usr/include/OGRE \
            += /usr/include/OGRE/RenderSystems/GL

//
LIBS += /usr/lib/x86_64-linux-gnu/libboost_system.so
LIBS += /usr/lib/x86_64-linux-gnu/libOgreMain.so
LIBS += /usr/lib/x86_64-linux-gnu/OGRE-1.9.0/libRenderSystem_GL.so

# -lboost_date_time -lboost_thread

UI_DIR = ./.ui
OBJECTS_DIR = ./.obj
MOC_DIR = ./.moc

SOURCES += main.cpp \
    cameranodeobject.cpp \
    exampleapp.cpp \
    ogreitem.cpp \
    ogrenode.cpp \
    ogrecamerawrapper.cpp \
    ogreengine.cpp

HEADERS += cameranodeobject.h \
    exampleapp.h \
    ogreitem.h \
    ogrenode.h \
    ogrecamerawrapper.h \
    ogreengine.h

OTHER_FILES += \
    resources/example.qml

RESOURCES += resources/resources.qrc

# Copy all resources to build folder
Resources.path = $$OUT_PWD/resources
Resources.files = resources/*.zip

# Copy all config files to build folder
Config.path = $$OUT_PWD
Config.files = config/*

# make install
INSTALLS += Resources Config
