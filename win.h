#ifndef WIN_H
#define WIN_H

#include <QAction>
#include <QCloseEvent>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QMenu>
#include <QSystemTrayIcon>

#include "timer.h"

namespace Ui {
class Win;
}

class Win : public QMainWindow
{
    Q_OBJECT

public:
    explicit Win(QWidget *parent = 0);
    ~Win();

    QMediaPlayer *player;

public slots:
    void onTick(int minutes, int seconds);
    void onTimeout(int count);
    void onStart(enum Timer::TIMER_STATE STATE, int minutes);
    void onStop();
    void onZeroCount();
    void onError();

private slots:
    void onIconActivation(QSystemTrayIcon::ActivationReason r);

private:
    Ui::Win *ui;

    Timer timer;
    QString soundFile = "/home/rtkt/src/Pomodoro/media/bell.ogg";
    QSystemTrayIcon *trayIcon;
    bool isModalOpened = false;
    QMenu *trayIconMenu;
    QAction *exitAction;

    void closeEvent(QCloseEvent *event);
};

#endif // WIN_H
