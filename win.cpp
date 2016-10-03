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

    setWindowIcon(QIcon(":/icons/tomato.png"));

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

void Win::beforeTimeout()
{
    setupPlayer(path);
}

void Win::checkFile(QString file)
{
    QFileInfo *fi = new QFileInfo(file);
    if(!fi->isDir() && fi->isReadable()) {
        emit fileIsPlayable(true);
    } else {
        emit fileIsPlayable(false);
    }
    delete fi;
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
    connect(&timer, SIGNAL(beforeTimeout()), SLOT(beforeTimeout()));
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
    trayIcon->setIcon(QIcon(":/icons/tomato.png"));
    trayIcon->show();

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            SLOT(onIconActivation(QSystemTrayIcon::ActivationReason)));
    connect(settingsAction, &QAction::triggered, [=]() {
        settingsWin = new Settings(this);
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

void Win::getSettings(bool apply)
{
    QSettings *settings = new QSettings("rtkt", "Pomodoro");
    int work = settings->value("workingTime", 25).toInt();
    int pause = settings->value("pauseTime", 5).toInt();
    int bigPause = settings->value("bigPauseTime", 15).toInt();
    bool autoWorking = settings->value("autoWorking", false).toBool();
    QString path = settings->value("pathToSoundFile",
                                   "/usr/share/sounds/pomodoro/bell.ogg").toString();
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

void Win::mouseMoveEvent(QMouseEvent *event)
{
    if((event->buttons() && Qt::LeftButton) && moving) {
        move(x() + (event->globalX() - lastPos.x),
             y() + (event->globalY() - lastPos.y));
        lastPos.x = event->globalX();
        lastPos.y = event->globalY();
    }
}

void Win::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        moving = true;
        lastPos.x = event->globalX();
        lastPos.y = event->globalY();
    }
}

void Win::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton && moving) {
        moving = false;
    }
}

void Win::onError()
{
    ui->Msg->setText(tr("Something went wrong..."));
    setupPlayer(path);
    player->play();
    delete player;
    player = nullptr;
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

void Win::onSetup(int work, int pause, int bigPause, bool autoWorking,
                  QString filePath, bool onTop, QByteArray geometry, bool save)
{
    if(save) {
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

    if(init) {
        if(onTop) {
            setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        } else {
            setWindowFlags(Qt::FramelessWindowHint);
        }
        init = false;
    }
    path = filePath;
    if(geometry != QByteArray()) {
        restoreGeometry(geometry);
    }
}

void Win::onStart(enum Timer::TIMER_STATE STATE, int minutes)
{
    ui->Btn->setText(tr("Stop"));
    ui->Time->setText(QString::number(minutes) + ":00");
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
}

void Win::onTick(int minutes, int seconds)
{
    ui->Time->setText(QString::number(minutes) + ":" +
                      QString::number(seconds));
}

void Win::onTimeout(int count)
{
    ui->Count->setText(QString::number(count) + tr(" pomodoros"));
    if(player == nullptr) {
        beforeTimeout();
    }
    player->play();
    delete player;
    player = nullptr;
}

void Win::onZeroCount()
{
    ui->Count->setText(QString::number(0) + tr(" pomodoros"));
}

void Win::setupPlayer(QString filePath)
{
    if(player == nullptr) {
        player = new QMediaPlayer(this);
        player->setVolume(100);
        connect(player, static_cast<void(QMediaPlayer::*)(QMediaPlayer::Error)>(&QMediaPlayer::error),
              [=](QMediaPlayer::Error error){
            if(error != QMediaPlayer::NoError) {
                QMessageBox::warning(this, tr("Error"),
                                     tr("Couldn't open audio file.\nPlease select correct audio file in the settings"),
                                     QMessageBox::Ok, QMessageBox::Ok);
            }
        });
    }
    player->setMedia(QUrl::fromLocalFile(filePath));
}
