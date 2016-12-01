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

    setWindowIcon(QIcon::fromTheme("pomodoro", QIcon(QString(ICONS_PATH) + "/pomodoro.png")));
    ui->closeBtn->setIcon(QIcon::fromTheme("window-close", QIcon(QString(ICONS_PATH) + "/button_close.png")));

    player = new QMediaPlayer(this);
    player->setVolume(100);

    connect(player, static_cast<void(QMediaPlayer::*)(QMediaPlayer::Error)>(&QMediaPlayer::error),
          this, &Win::onPlayerError);
    connect(ui->Btn, SIGNAL(clicked()), &timer, SLOT(onClick()));
    connect(this, SIGNAL(newSettings(int, int, int, bool, QString, bool, QByteArray, bool)),
            SLOT(onSetup(int, int, int, bool, QString, bool, QByteArray, bool)));
    connect(ui->closeBtn, &QPushButton::clicked, [=]() {
        close();
    });

    createTrayIcon();
    connectTimer();
    getSettings(true);
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
    connect(this, SIGNAL(newSettings(int, int, int, bool, QString, bool, QByteArray, bool)),
            &timer, SLOT(onSetup(int, int, int, bool, QString, bool, QByteArray, bool)));
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
        QSettings s("rtkt", "Pomodoro");
        s.setValue("windowGeometry", saveGeometry());
        QCoreApplication::exit();
    });
}

void Win::closeEvent(QCloseEvent *event)
{
    hide();
    event->ignore();
}


// Just get settings and emit a needed signal
void Win::getSettings(bool apply)
{
    QSettings *settings = new QSettings("rtkt", "Pomodoro");
    int work = settings->value("workingTime", 25).toInt();
    int pause = settings->value("pauseTime", 5).toInt();
    int bigPause = settings->value("bigPauseTime", 15).toInt();
    bool autoWorking = settings->value("autoWorking", false).toBool();
    QString path = settings->value("pathToSoundFile",
                                   QString(DEFAULT_SOUND)).toString();
    QByteArray geometry = settings->value("windowGeometry").toByteArray();
//    QString lang = settings->value("language", "en-US").toString();
    bool onTop = settings->value("alwaysOnTop", true).toBool();
    if(apply) {
        emit newSettings(work, pause, bigPause,
                         autoWorking, path, onTop, geometry, false);
    } else {
        emit gotSettings(work, pause, bigPause,
                         autoWorking, path, onTop);
    }
    delete settings;
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

void Win::saveSettings(int work, int pause, int bigPause, bool autoWorking, QString filePath,
                       bool onTop)
{
    QSettings *settings = new QSettings("rtkt", "Pomodoro", this);
    settings->setValue("workingTime", work);
    settings->setValue("pauseTime", pause);
    settings->setValue("bigPauseTime", bigPause);
    settings->setValue("autoWorking", autoWorking);
    settings->setValue("pathToSoundFile", filePath);
//        settings->setValue("language");
    settings->setValue("alwaysOnTop", onTop);
    delete settings;
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
    QString messageEnding = "\nSelect another file?";
    switch(error) {
    case QMediaPlayer::ResourceError:
        res = QMessageBox::critical(this, tr("Error"), tr("Alarm file couldn't be opened."
                                                          + messageEnding.toLocal8Bit()),
                                        QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        break;
    case QMediaPlayer::FormatError:
        res = QMessageBox::critical(this, tr("Error"), tr("Something is wrong with the format of the alarm file."
                                                          + messageEnding.toLocal8Bit()),
                                    QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        break;
    case QMediaPlayer::AccessDeniedError:
        res = QMessageBox::critical(this, tr("Error"), tr("There're no appropriate permissions to play an alarm file."
                                                          + messageEnding.toLocal8Bit()),
                                    QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        break;
    case QMediaPlayer::NoError:
        return;
    default:
        res = QMessageBox::critical(this, tr("Error"), tr("Something is wrong with the alarm file."
                                                          + messageEnding.toLocal8Bit()),
                                    QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        break;
    }

    if(res == QMessageBox::Yes) {
        Settings *settings = new Settings(this);
        settings->show();
        settings->selectFile();
    }
}

void Win::onSetup(int work, int pause, int bigPause, bool autoWorking,
                  QString filePath, bool onTop, QByteArray geometry, bool save)
{
    if(save) {
        saveSettings(work, pause, bigPause, autoWorking, filePath, onTop);
    }

    if(init) {
        if(geometry != QByteArray()) {
            restoreGeometry(geometry);
        }
        if(onTop) {
            setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        } else {
            setWindowFlags(Qt::FramelessWindowHint);
        }
        init = false;
    }
    player->setMedia(QUrl::fromLocalFile(filePath));
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
        ui->Msg->setText(tr("Now work"));
        break;
    case Timer::TIMER_PAUSE:
        ui->Msg->setText(tr("Rest a bit (small break)"));
        break;
    case Timer::TIMER_BIGPAUSE:
        ui->Msg->setText(tr("Now rest (big pause)"));
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
