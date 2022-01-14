QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++2a

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    codedialog.cpp \
    codegenerator.cpp \
    dfamachine.cpp \
    main.cpp \
    dialog.cpp \
    mfamachine.cpp \
    nfamachine.cpp \
    regularexpressionengine.cpp \
    regularexpressionparser.cpp

HEADERS += \
    codedialog.h \
    codegenerator.h \
    dfamachine.h \
    dialog.h \
    mfamachine.h \
    nfamachine.h \
    regularexpressionengine.h \
    regularexpressionparser.h

FORMS += \
    codedialog.ui \
    dialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
