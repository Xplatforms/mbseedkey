QT += quick
CONFIG += c++latest


RC_ICONS="Resources/icon.ico"
RC_LANG="en"

VERSION=1.0.1.00

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
TARGET = MBSeedKey
DESTDIR = build/release

SOURCES += \
        ecuseedkeydll.cpp \
        exutils.cpp \
        main.cpp

HEADERS += \
    ecuseedkeydll.h \
    exutils.h

RESOURCES += qml.qrc

TRANSLATIONS += \
    mbseedkey_en.ts


win32 {
    CONFIG += embed_manifest_exe
    QMAKE_LFLAGS += /LARGEADDRESSAWARE
#    QMAKE_LFLAGS_WINDOWS += /MANIFESTUAC:"level='requireAdministrator'"
}

DEPLOY_TARGET = $$shell_quote($$shell_path($$DESTDIR"/"$$TARGET".exe"))
WINDEPLOY_CMD = $$shell_quote($$shell_path($$[QT_INSTALL_BINS]$$QMAKE_DIR_SEP"windeployqt.exe"))
win32: QMAKE_POST_LINK = $$WINDEPLOY_CMD --compiler-runtime --verbose 2 --release --angle --qmldir $$shell_quote($$shell_path($$_PRO_FILE_PWD_)) $$DEPLOY_TARGET
