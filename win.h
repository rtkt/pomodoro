#ifndef WIN_H
#define WIN_H

#include <QAction>
#include <QCloseEvent>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QMenu>
#include <QMouseEvent>
#include <QPoint>
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
    QMenu *trayIconMenu;
    QAction *exitAction;
    QPoint *lastPos;
    bool isModalOpened = false;
    bool moving = false;

    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
};

#endif // WIN_H
