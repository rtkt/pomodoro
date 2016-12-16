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

!win32 {
    isEmpty(PREFIX):PREFIX=/usr/local
    isEmpty(APPDIR):APPDIR=$$PREFIX/share/applications
    isEmpty(LOGODIR):LOGODIR=$$PREFIX/share/pixmaps
    isEmpty(ICONDIR):ICONDIR=$$PREFIX/share/pixmaps/$$TARGET
    isEmpty(BINDIR):BINDIR=$$PREFIX/bin
    isEmpty(FILESDIR):FILESDIR=$$PREFIX/share/$$TARGET
    isEmpty(SOUNDFILE):SOUNDFILE=$$FILESDIR/bell.ogg
}

win32 {
    isEmpty(PREFIX):PREFIX=.
    isEmpty(ICONDIR):ICONDIR=$$PREFIX/media
    isEmpty(BINDIR):BINDIR=$$PREFIX
    isEmpty(FILESDIR):FILESDIR=$$PREFIX/media
    isEmpty(SOUNDFILE):SOUNDFILE=$$FILESDIR/bell.ogg
    INSTALLS -= logo desktop
}

target.path = $$BINDIR
logo.path = $$LOGODIR
icons.path = $$ICONDIR
desktop.path = $$APPDIR
translations.path = $$FILESDIR/translations
sound.path = $$FILESDIR

logo.files = ../etc/icons/pomodoro.png
icons.files = ../etc/icons/*
desktop.files = ../etc/pomodoro.desktop
sound.files = ../etc/sound/bell.ogg

CONFIG(embed_translations) {
    # create translations resource file
    system("echo \'<RCC><qresource prefix=\"/\">\' > translations.qrc")
    for(translation, TRANSLATIONS_COMPILED):system("echo \'<file>$$translation</file>\' >> translations.qrc")
    system("echo \'</qresource></RCC>\'" >> translations.qrc)

    # add file to build
    RESOURCES += translations.qrc

    LANG_PATH += :/translations

    # make sure translations are updated and released
    CONFIG *= update_translations release_translations
}

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
#            "LANG_PATH=\\\"$$LANG_PATH\\\"" \
            "LANG_PATH=\\\"/usr/local/share/pomodoro/translations\\\"" \
            "ICONS_PATH=\\\"$$ICONDIR\\\""

SOURCES += main.cpp\
        win.cpp \
    timer.cpp \
    settings.cpp

HEADERS  += win.h \
    timer.h \
    settings.h

FORMS    += win.ui \
    settings.ui
