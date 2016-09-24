#include "win.h"
#include "ui_win.h"

Win::Win(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Win)
{
    ui->setupUi(this);

    connect(&timer, SIGNAL(error()), this, SLOT(onError()), Qt::AutoConnection);
    connect(&timer, SIGNAL(started(enum Timer::TIMER_STATE, int)), SLOT(onStart(enum Timer::TIMER_STATE,int)));
    connect(&timer, SIGNAL(stopped()), SLOT(onStop()));
    connect(&timer, SIGNAL(tick(int, int)), SLOT(onTick(int, int)));
    connect(&timer, SIGNAL(timeout(int)), SLOT(onTimeout(int)));
    connect(&timer, SIGNAL(zeroCount()), SLOT(onZeroCount()));
    connect(ui->Btn, SIGNAL(clicked()), &timer, SLOT(onClick()));
}

Win::~Win()
{
    delete ui;
}

void Win::onError()
{
    ui->Msg->setText(tr("Something went wrong..."));
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
}

void Win::onZeroCount()
{
    onTimeout(0);
}
