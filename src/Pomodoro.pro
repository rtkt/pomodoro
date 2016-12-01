#-------------------------------------------------
#
# Project created by QtCreator 2016-09-24T03:43:17
#
#-------------------------------------------------

QT       += core gui multimedia
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = pomodoro
TEMPLATE = app

TRANSLATIONS += translations/ru.ts

DESTDIR = build
OBJECTS_DIR = $${DESTDIR}/obj
MOC_DIR = $${DESTDIR}/moc
RCC_DIR = $${DESTDIR}/rcc
UI_DIR = $${DESTDIR}/ui

TRANSLATIONS_COMPILED = $$TRANSLATIONS
TRANSLATIONS_COMPILED ~= s/\.ts/.qm/g

isEmpty(PREFIX):PREFIX=/usr/local
isEmpty(APPDIR):APPDIR=$$PREFIX/share/applications
isEmpty(ICONDIR):ICONDIR=$$PREFIX/share/pixmaps
isEmpty(BINDIR):BINDIR=$$PREFIX/bin
isEmpty(FILESDIR):FILESDIR=$$PREFIX/share/$$TARGET
isEmpty(SOUNDFILE):SOUNDFILE=$$FILESDIR/bell.ogg

target.path = $$INSTROOT$$BINDIR
icon.path = $$INSTROOT$$ICONDIR
desktop.path = $$INSTROOT$$APPDIR
translations.path = $$INSTROOT$$FILESDIR/translations
sound.path = $$INSTROOT$$FILESDIR

icon.files = ../etc/logo/pomodoro.png
desktop.files = ../etc/pomodoro.desktop
sound.files = sound/bell.ogg

CONFIG(install_translations) {
    translations.files = $$TRANSLATIONS_COMPILED
    INSTALL += tranlations
    LANG_PATH += $$FILESDIR/translations
    CONFIG *= update_translations release_tranlations
}

CONFIG(begin_translations) {
    isEmpty(lupdate):lupdate=lupdate-qt5
    system($$lupdate -locations absolute $$_PRO_FILE_)
}

CONFIG(update_translations) {
    isEmpty(lupdate):lupdate=lupdate-qt5
    system($$lupdate -no-obsolete -locations none $$_PRO_FILE_)
}

CONFIG(release_translations) {
    isEmpty(lrelease):lrelease=lrelease-qt5
    system($$lrelease $$_PRO_FILE_)
}

INSTALLS += target sound icon desktop

DEFINES += "DEFAULT_SOUND=\\\"$$SOUNDFILE\\\"" \
            "LANG_PATH=\\\"$$LANG_PATH\\\""

SOURCES += main.cpp\
        win.cpp \
    timer.cpp \
    settings.cpp

HEADERS  += win.h \
    timer.h \
    settings.h

FORMS    += win.ui \
    settings.ui

RESOURCES += \
    img/icons.qrc
