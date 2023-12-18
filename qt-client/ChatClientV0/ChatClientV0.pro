QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    additemdialog.cpp \
    currentuserinfo.cpp \
    logindialog.cpp \
    main.cpp \
    mainwindow.cpp \
    networkthread.cpp

HEADERS += \
    additemdialog.h \
    currentuserinfo.h \
    group.h \
    groupuser.h \
    json.h \
    logindialog.h \
    mainwindow.h \
    msgcontent.h \
    networkthread.h \
    publicmsg.h \
    user.h

FORMS += \
    additemdialog.ui \
    logindialog.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
