#-------------------------------------------------
#
# Project created by QtCreator 2014-09-13T19:50:27
#
#-------------------------------------------------

QT += core gui widgets network

TARGET = Netvisix
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp\
    GUI/MainWindow.cpp \
    GUI/VisibleHost.cpp \
    GUI/VisibleBase.cpp \
    NetView.cpp \
    Net/NetEvent.cpp \
    Net/NetEventManager.cpp \
    Net/Host.cpp \
    Net/NetStatistic.cpp \
    Net/PacketHandler.cpp \
    GUI/VisiblePacket.cpp \
    GUI/StatusbarDisplay.cpp \
    Net/NetUtil.cpp \
    GUI/HostInfoPopup.cpp \
    GUI/VisibleLink.cpp \
    GUI/StatisticPopup.cpp

HEADERS += GUI/MainWindow.h \
    GUI/VisibleHost.h \
    GUI/VisibleBase.h \
    NetView.h \
    Net/NetEvent.h \
    Net/NetEventManager.h \
    Net/Host.h \
    Net/NetStatistic.h \
    Net/PacketHandler.h \
    GUI/VisiblePacket.h \
    Net/IPreparedNetEventListener.h \
    GUI/StatusbarDisplay.h \
    Net/NetUtil.h \
    GUI/HostInfoPopup.h \
    GUI/VisibleLink.h \
    GUI/StatisticPopup.h \
    GUI/Strings.h

FORMS += MainWindow.ui \
    GUI/HostInfoPopup.ui \
    GUI/StatisticPopup.ui

RESOURCES += \
    app.qrc


unix:!macx: INCLUDEPATH += $$PWD/../libtins/include
unix:!macx: DEPENDPATH += $$PWD/../libtins/include

unix:!macx: LIBS += -L$$PWD/../libtins/ -ltins
unix:!macx: PRE_TARGETDEPS += $$PWD/../libtins/libtins.a

unix:!macx: LIBS += -lpcap


win32: DEFINES += WIN32_LEAN_AND_MEAN

win32: LIBS += -LE:/Build/WpdPack/Lib/ -lwpcap -lws2_32 -liphlpapi
win32: INCLUDEPATH += E:/Build/WpdPack/Include

win32: LIBS += -LE:/Build/libtins/lib/ -ltins
win32: INCLUDEPATH += E:/Build/libtins/include
