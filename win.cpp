#include "win.h"
#include "ui_win.h"

#include <QCoreApplication>

Win::Win(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Win)
{
    ui->setupUi(this);

    setWindowIcon(QIcon(":/icon/tomato.png"));

    player = new QMediaPlayer(this);
    player->setMedia(QUrl::fromLocalFile(soundFile));
    player->setVolume(100);

    trayIconMenu = new QMenu();
    exitAction = new QAction(tr("Exit"), (QObject*)trayIconMenu);
    trayIconMenu->addAction(exitAction);
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(QIcon(":/icon/tomato.png"));
    trayIcon->show();

    connect(&timer, SIGNAL(error()), SLOT(onError()));
    connect(&timer, SIGNAL(started(enum Timer::TIMER_STATE, int)), SLOT(onStart(enum Timer::TIMER_STATE,int)));
    connect(&timer, SIGNAL(stopped()), SLOT(onStop()));
    connect(&timer, SIGNAL(tick(int, int)), SLOT(onTick(int, int)));
    connect(&timer, SIGNAL(timeout(int)), SLOT(onTimeout(int)));
    connect(&timer, SIGNAL(zeroCount()), SLOT(onZeroCount()));
    connect(ui->Btn, SIGNAL(clicked()), &timer, SLOT(onClick()));
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), SLOT(onIconActivation(QSystemTrayIcon::ActivationReason)));
    connect(exitAction, &QAction::triggered, [=]() {
        QCoreApplication::exit();
    });
}

Win::~Win()
{
    delete ui;
    delete trayIconMenu;
}

void Win::closeEvent(QCloseEvent *event)
{
    if(!isModalOpened) {
        hide();
    }
    event->ignore();
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
