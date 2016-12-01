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

target.path = $$BINDIR
logo.path = $$ICONDIR
icons.path = $$ICONDIR/$$TARGET
desktop.path = $$APPDIR
translations.path = $$FILESDIR/translations
sound.path = $$FILESDIR

logo.files = ../etc/icons/pomodoro.png
icons.files = ../etc/icons/*
desktop.files = ../etc/pomodoro.desktop
sound.files = ../etc/sound/bell.ogg

CONFIG(install_translations) {
    translations.files = $$TRANSLATIONS_COMPILED
    INSTALLS += translations
    LANG_PATH = $$FILESDIR/translations
    CONFIG *= update_translations release_tranlations
}

CONFIG(begin_translations) {
    isEmpty(lupdate):lupdate=lupdate
    system($$lupdate -locations absolute $$_PRO_FILE_)
}

CONFIG(update_translations) {
    isEmpty(lupdate):lupdate=lupdate
    system($$lupdate -no-obsolete -locations none $$_PRO_FILE_)
}

CONFIG(release_translations) {
    isEmpty(lrelease):lrelease=lrelease
    system($$lrelease $$_PRO_FILE_)
}

INSTALLS += target sound logo desktop icons

DEFINES += "DEFAULT_SOUND=\\\"$$SOUNDFILE\\\"" \
            "LANG_PATH=\\\"$$LANG_PATH\\\"" \
            "ICONS_PATH=\\\"$$ICONDIR/$$TARGET\\\""

SOURCES += main.cpp\
        win.cpp \
    timer.cpp \
    settings.cpp

HEADERS  += win.h \
    timer.h \
    settings.h

FORMS    += win.ui \
    settings.ui

RESOURCES +=
