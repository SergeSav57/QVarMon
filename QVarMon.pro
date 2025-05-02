QT      += core gui
QT      += network
QT      += printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    IXXATCmd.cpp \
    LoggingCategories.cpp \
    comboboxdelegate.cpp \
    dbgtimer.cpp \
    dlgabout.cpp \
    dlgaddaddress.cpp \
    dlgbineditor.cpp \
    dlgconfig.cpp \
    dlghowtouse.cpp \
    dlgixxat.cpp \
    dlgselectcfg.cpp \
    dlgtablearray.cpp \
    dlgtablework.cpp \
    dlguart.cpp \
    dlgvaraddr.cpp \
    dlgvarsel.cpp \
    main.cpp \
    mainwindow.cpp \
    mylocalserver.cpp \
    mystyles.cpp \
    tblselmodel.cpp \
    tblworkmodel.cpp

HEADERS += \
    IXXAT/baltype.h \
    IXXAT/cantype.h \
    IXXAT/lintype.h \
    IXXAT/stdtype.h \
    IXXAT/vciapi.h \
    IXXAT/vcierr.h \
    IXXAT/vcisdk.h \
    IXXAT/vcitype.h \
    IXXAT/vciver.h \
    IXXATCmd.h \
    LoggingCategories.h \
    comboboxdelegate.h \
    common.h \
    crc32.h \
    dbgtimer.h \
    dlgabout.h \
    dlgaddaddress.h \
    dlgbineditor.h \
    dlgconfig.h \
    dlghowtouse.h \
    dlgixxat.h \
    dlgselectcfg.h \
    dlgtablearray.h \
    dlgtablework.h \
    dlguart.h \
    dlgvaraddr.h \
    dlgvarsel.h \
    mainwindow.h \
    mycolors.h \
    mylocalserver.h \
    mystyles.h \
    tblselmodel.h \
    tblworkmodel.h

FORMS += \
    dlgabout.ui \
    dlgaddaddress.ui \
    dlgbineditor.ui \
    dlgconfig.ui \
    dlghowtouse.ui \
    dlgixxat.ui \
    dlgselectcfg.ui \
    dlgtablearray.ui \
    dlgtablework.ui \
    dlguart.ui \
    dlgvaraddr.ui \
    dlgvarsel.ui \
    mainwindow.ui

# игнорировать файлы при сборке
#SOURCES -= tblallmodel.cpp \
#           dlgcreatevarfile.cpp

#HEADERS -= tblallmodel.h \
#           dlgcreatevarfile.h

#FORMS   -= dlgcreatevarfile.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

LIBS += -lSetupAPI

RESOURCES += \
    resource.qrc

DISTFILES += \
    QVarMon.pro.user

RC_ICONS = $$PWD/res/displayChip.ico
