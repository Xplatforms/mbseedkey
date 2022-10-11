QT -= quick
CONFIG += c++latest
CONFIG += console

GIT_LATEST_TAG=$$system(git describe --always --abbrev=0)
SW_VERSION=\\\"$$GIT_LATEST_TAG\\\"
DEFINES += SW_VERSION=$$(SW_VERSION)

VERSION_MAJOR=$$section(GIT_LATEST_TAG, ., 0, 0)
VERSION_MINOR=$$section(GIT_LATEST_TAG, ., 1, 1)
VERSION_PATCH=$$section(GIT_LATEST_TAG, ., 2, 2)

RC_ICONS="Resources/icon.ico"
RC_LANG="en"

VERSION=$$GIT_LATEST_TAG
VERSION_PE_HEADER = $$VERSION_MAJOR'.'$$VERSION_MINOR

message("VER: $$VERSION" )

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
TARGET = mbseedkey_cli
DESTDIR = build/release

SOURCES += \
        ecuseedkeydll.cpp \
        exutils.cpp \
        main_cli.cpp

HEADERS += \
    ecuseedkeydll.h \
    exutils.h

#RESOURCES += qml.qrc
TRANSLATIONS += mbseedkey_en.ts


win32 {
    CONFIG += embed_manifest_exe
    QMAKE_LFLAGS += /LARGEADDRESSAWARE
#    QMAKE_LFLAGS_WINDOWS += /MANIFESTUAC:"level='requireAdministrator'"
}

DEPLOY_TARGET = $$shell_quote($$shell_path($$DESTDIR"/"$$TARGET".exe"))
WINDEPLOY_CMD = $$shell_quote($$shell_path($$[QT_INSTALL_BINS]$$QMAKE_DIR_SEP"windeployqt.exe"))
win32: QMAKE_POST_LINK = $$WINDEPLOY_CMD --compiler-runtime --verbose 2 --release $$DEPLOY_TARGET
