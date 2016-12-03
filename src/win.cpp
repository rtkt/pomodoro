#include "win.h"
#include "ui_win.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QMessageBox>
#include <QSettings>

Win::Win(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Win)
{
    ui->setupUi(this);
    player = new QMediaPlayer(this);
    player->setVolume(100);

    setWindowIcon(QIcon::fromTheme("pomodoro", QIcon(QString(ICONS_PATH) + "/pomodoro.png")));
    ui->closeBtn->setIcon(QIcon::fromTheme("window-close", QIcon(QString(ICONS_PATH) + "/button_close.png")));

//    connect(player, static_cast<void(QMediaPlayer::*)(QMediaPlayer::Error)>(&QMediaPlayer::error),
//          this, &Win::onPlayerError);
    connect(player, SIGNAL(error(QMediaPlayer::Error)), SLOT(onPlayerError(QMediaPlayer::Error)));
    connect(ui->Btn, SIGNAL(clicked()), &timer, SLOT(onClick()));
    connect(ui->closeBtn, &QPushButton::clicked, [=]() {
        close();
    });
    connect(this, SIGNAL(setLang(QString)), SLOT(onSetLang(QString)));
    connect(this, SIGNAL(setOnTop(bool)), SLOT(onSetOnTop(bool)));
    connect(this, SIGNAL(setPath(QString)), SLOT(onSetPath(QString)));
    connectTimer();

    createTrayIcon();

    setup();
}

Win::~Win()
{
    delete ui;
    delete trayIconMenu;
}

void Win::connectTimer()
{
    connect(&timer, SIGNAL(error()), SLOT(onError()));
    connect(&timer, SIGNAL(started(enum Timer::TIMER_STATE, int)),
            SLOT(onStart(enum Timer::TIMER_STATE,int)));
    connect(&timer, SIGNAL(stopped()), SLOT(onStop()));
    connect(&timer, SIGNAL(tick(int, int)), SLOT(onTick(int, int)));
    connect(&timer, SIGNAL(timeout(int)), SLOT(onTimeout(int)));
    connect(&timer, SIGNAL(zeroCount()), SLOT(onZeroCount()));
    connect(this, SIGNAL(setWorkTime(int)), &timer, SLOT(setWorkTime(int)));
    connect(this, SIGNAL(setPauseTime(int)), &timer, SLOT(setPauseTime(int)));
    connect(this, SIGNAL(setBigPauseTime(int)), &timer, SLOT(setBigPauseTime(int)));
    connect(this, SIGNAL(setAutoWorking(bool)), &timer, SLOT(setAutoWorking(bool)));
    connect(this, SIGNAL(setAutoZero(bool)), &timer, SLOT(setAutoZero(bool)));
}

void Win::createTrayIcon()
{
    trayIconMenu = new QMenu();
    exitAction = new QAction(tr("Exit"), (QObject*)trayIconMenu);
    settingsAction = new QAction(tr("Settings"), (QObject*)trayIconMenu);
    trayIconMenu->addAction(settingsAction);
    trayIconMenu->addAction(exitAction);
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(QIcon::fromTheme("pomodoro",
                                       QIcon(QString(ICONS_PATH) + "/pomodoro.png")));
    trayIcon->show();

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            SLOT(onIconActivation(QSystemTrayIcon::ActivationReason)));
    connect(settingsAction, &QAction::triggered, [=]() {
        Settings *settingsWin = new Settings(this);
        settingsWin->show();
    });
    connect(exitAction, &QAction::triggered, [=]() {
        QCoreApplication::exit();
    });
}

void Win::closeEvent(QCloseEvent *event)
{
    hide();
    event->ignore();
}


// Just get settings and emit a needed signal
struct Win::options* Win::getSettings()
{
    QSettings *settings = newSettings();
    struct options *ret = new struct options;
    ret->autoWorking = settings->value("autoWorking", false).toBool();
    ret->autoZero = settings->value("autoZero", true).toBool();
    ret->bigPause = settings->value("bigPauseTime", 15).toInt();
    ret->language = settings->value("language", "en").toString();
    ret->onTop = settings->value("alwaysOnTop", true).toBool();
    ret->path = settings->value("pathToSoundFile", QString(DEFAULT_SOUND)).toString();
    ret->pause = settings->value("pauseTime", 5).toInt();
    ret->work = settings->value("workingTime", 25).toInt();
    delete settings;
    return ret;
}

// Move the main window (only in window moving state)
void Win::mouseMoveEvent(QMouseEvent *event)
{
    if((event->buttons() && Qt::LeftButton) && moving) {
        move(x() + (event->globalX() - lastPos.x),
             y() + (event->globalY() - lastPos.y));
        lastPos.x = event->globalX();
        lastPos.y = event->globalY();
    }
}

// Enter window moving state...
void Win::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        moving = true;
        lastPos.x = event->globalX();
        lastPos.y = event->globalY();
    }
}

// Quit window moving state
void Win::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton && moving) {
        moving = false;
    }
}

void Win::onError()
{
    ui->Msg->setText(tr("Something went wrong..."));
    player->play();
    minutesLeft.num = 0;
    minutesLeft.str = "00";
}

void Win::onIconActivation(QSystemTrayIcon::ActivationReason r)
{
    switch(r)
    {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        if(!isHidden()) {
            close();
        } else {
            show();
        }
        break;
    case QSystemTrayIcon::Context:
        trayIconMenu->setVisible(true);
        break;
     default:
        break;
    }
}

// Something is wrong with player... (connected to player's Error())
void Win::onPlayerError(QMediaPlayer::Error error)
{
    int res;
    switch(error) {
    case QMediaPlayer::ResourceError:
        res = QMessageBox::critical(this, tr("Error"),
                                    tr("Alarm file couldn't be opened.\nSelect another file?"),
                                        QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        break;
    case QMediaPlayer::FormatError:
        res = QMessageBox::critical(this, tr("Error"),
                                    tr("Something is wrong with the format of the alarm file.\nSelect another file?"),
                                    QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        break;
    case QMediaPlayer::AccessDeniedError:
        res = QMessageBox::critical(this, tr("Error"),
                                    tr("There're no appropriate permissions to play an alarm file.\nSelect another file?"),
                                    QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        break;
    case QMediaPlayer::NoError:
        return;
    default:
        res = QMessageBox::critical(this, tr("Error"),
                                    tr("Something is wrong with the alarm file.\nSelect another file?"),
                                    QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        break;
    }

    if(res == QMessageBox::Yes) {
        Settings *settings = new Settings(this);
        settings->show();
        settings->selectFile();
    }
}

void Win::onSetLang(QString lang)
{
    if(currentLang != "en") {
        qApp->removeTranslator(&translator);
    }
    if(lang != "en") {
        translator.load(lang, LANG_PATH);
        qApp->installTranslator(&translator);
    }
    ui->retranslateUi(this);
    exitAction->disconnect();
    settingsAction->disconnect();
    trayIcon->disconnect();
    delete exitAction;
    delete settingsAction;
    delete trayIconMenu;
    delete trayIcon;
    createTrayIcon();
    currentLang = lang;
}

void Win::onStart(enum Timer::TIMER_STATE STATE, int minutes)
{
    ui->Btn->setText(tr("Stop"));
    this->minutesLeft.num = minutes;
    minutes < 10 ? this->minutesLeft.str = "0" + QString::number(minutes) :
            this->minutesLeft.str = QString::number(minutes);
    ui->Time->setText(this->minutesLeft.str + ":00");
    switch(STATE) {
    case Timer::TIMER_WORKING:
        ui->Msg->setText(tr("Time to work"));
        break;
    case Timer::TIMER_PAUSE:
        ui->Msg->setText(tr("Break"));
        break;
    case Timer::TIMER_BIGPAUSE:
        ui->Msg->setText(tr("Big break"));
        break;
    default:
        ui->Msg->setText(tr("Something went wrong..."));
    }
}

void Win::onStop()
{
    ui->Btn->setText(tr("Start"));
    ui->Msg->setText(tr("Stopped"));
    ui->Time->setText("00:00");
    minutesLeft.num = 0;
    minutesLeft.str = "00";
}

void Win::onTick(int minutes, int seconds)
{
    QString sec;
    if(this->minutesLeft.num != minutes) {
        this->minutesLeft.num = minutes;
        minutes < 10 ? this->minutesLeft.str = "0" + QString::number(minutes) :
                this->minutesLeft.str = QString::number(minutes);
    }
    if(seconds < 10) {
        sec = "0" + QString::number(seconds);
    } else {
        sec = QString::number(seconds);
    }
    ui->Time->setText(minutesLeft.str + ":" + sec);
}

void Win::onTimeout(int count)
{
    ui->Count->setText(QString::number(count) + tr(" pomodoro(s)"));
    player->play();
    minutesLeft.num = 0;
    minutesLeft.str = "00";
}

void Win::onZeroCount()
{
    ui->Count->setText(QString::number(0) + tr(" pomodoro(s)"));
}

void Win::setup()
{
    options *st = getSettings();
    emit setAutoWorking(st->autoWorking);
    emit setAutoZero(st->autoZero);
    emit setBigPauseTime(st->bigPause);
    emit setLang(st->language);
    emit setOnTop(st->onTop);
    emit setPath(st->path);
    emit setPauseTime(st->pause);
    emit setWorkTime(st->work);
    delete st;
}
