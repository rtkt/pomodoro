#include "win.h"
#include "ui_win.h"

#include <QCoreApplication>

Win::Win(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Win)
{
    ui->setupUi(this);

    setWindowIcon(QIcon(":/icons/tomato.png"));

    player = new QMediaPlayer(this);
    player->setVolume(100);
    settings = new QSettings("rtkt", "Pomodoro");

    connect(ui->Btn, SIGNAL(clicked()), &timer, SLOT(onClick()));
    connect(this, SIGNAL(newSettings(int, int, int, bool, QString, bool)),
            SLOT(onSetup(int,int,int,bool,QString,bool)));
    connect(ui->closeBtn, &QPushButton::clicked, [=]() {
        if(!isModalOpened) {
            hide();
        }
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
    connect(this, SIGNAL(newSettings(int, int, int, bool, QString, bool)),
            &timer, SLOT(onSetup(int, int, int, bool, QString, bool)));
}

void Win::createTrayIcon()
{
    trayIconMenu = new QMenu();
    exitAction = new QAction(tr("Exit"), (QObject*)trayIconMenu);
    trayIconMenu->addAction(exitAction);
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(QIcon(":/icons/tomato.png"));
    trayIcon->show();

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), SLOT(onIconActivation(QSystemTrayIcon::ActivationReason)));
    connect(exitAction, &QAction::triggered, [=]() {
        QCoreApplication::exit();
    });
}

void Win::closeEvent(QCloseEvent *event)
{
    if(!isModalOpened) {
        hide();
    }
    event->ignore();
}

void Win::getSettings(bool apply)
{
    int work = settings->value("workingTime", 25).toInt();
    int pause = settings->value("pauseTime", 5).toInt();
    int bigPause = settings->value("bigPauseTime", 25).toInt();
    bool autoWorking = settings->value("autoWorking", false).toBool();
    QString path = settings->value("pathToSound", "/usr/share/sounds/pomodoro/bell.ogg").toString();
    bool onTop = settings->value("alwaysOnTop", true).toBool();
    if(apply) {
        emit newSettings(work, pause, bigPause, autoWorking, path, onTop);
    } else {
        emit gotSettings(work, pause, bigPause, autoWorking, path, onTop);
    }
}

void Win::mouseMoveEvent(QMouseEvent *event)
{
    if((event->buttons() && Qt::LeftButton) && moving) {
        move(x() + (event->globalX() - lastPos->x()), y() + (event->globalY() - lastPos->y()));
        lastPos->setX(event->globalX());
        lastPos->setY(event->globalY());
    }
}

void Win::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        moving = true;
        lastPos = new QPoint();
        lastPos->setX(event->globalX());
        lastPos->setY(event->globalY());
    }
}

void Win::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton && moving) {
        moving = false;
        delete lastPos;
    }
}

void Win::onError()
{
    ui->Msg->setText(tr("Something went wrong..."));
    player->play();
}

void Win::onIconActivation(QSystemTrayIcon::ActivationReason r)
{
    switch(r)
    {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        if(!isHidden()) {
            hide();
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

void Win::onSetup(int work, int pause, int bigPause, bool autoWorking, QString filePath, bool onTop)
{
    if(onTop) {
        setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    } else {
        setWindowFlags(Qt::FramelessWindowHint);
    }
    player->setMedia(QUrl::fromLocalFile(filePath));

    (void)work;
    (void)pause;
    (void)bigPause;
    (void)autoWorking;
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
    ui->Time->setText(QString::number(minutes) + ":" + QString::number(seconds));
}

void Win::onTimeout(int count)
{
    ui->Count->setText(QString::number(count) + tr(" pomodoros"));
    player->play();
}

void Win::onZeroCount()
{
    ui->Count->setText(QString::number(0) + tr(" pomodoros"));
}
