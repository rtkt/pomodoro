#ifndef WIN_H
#define WIN_H

#include <QMainWindow>
#include <QMediaPlayer>

#include "timer.h"

namespace Ui {
class Win;
}

class Win : public QMainWindow
{
    Q_OBJECT

    QString soundFile = "/home/rtkt/Misc/Packages/Fresh and Clean/stereo/dialog-information.ogg";

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

private:
    Ui::Win *ui;

    Timer timer;
};

#endif // WIN_H
