QT += quick sql charts

SOURCES += \
        dbmanager.cpp \
        main.cpp \
        taskcontroller.cpp \
        taskfilter.cpp \
        taskmodel.cpp

resources.files = main.qml 
resources.prefix = /$${TARGET}
RESOURCES += resources \
    TaskManager.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    dbmanager.h \
    task.h \
    taskcontroller.h \
    taskfilter.h \
    taskmodel.h

DISTFILES += \
    AddTaskDialog.qml \
    Board.qml \
    BoardColumn.qml \
    DeleteTaskDialog.qml \
    EditWindow.qml \
    Footer.qml \
    MenuButton.qml \
    Statistics.qml \
    StatsBar.qml \
    TaskCard.qml \
    TopBar.qml
