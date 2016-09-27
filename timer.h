#ifndef TIMER_H
#define TIMER_H

#include <QObject>
#include <QTimerEvent>

class Timer : public QObject
{
    Q_OBJECT

    int work = 1;
    int pause = 5;
    int bigPause = 15;
    bool autoWorking = false;
    int count = 0;
    int minutes;
    int seconds;
    int timerId;


    void timerEvent(QTimerEvent *event);
    void start(bool pause = false);
    void stop(bool autoStopped = true);

public:
    enum TIMER_STATE {TIMER_IDLE, TIMER_WORKING, TIMER_PAUSE, TIMER_BIGPAUSE};

public slots:
    void onClick();
    void onSetup(int work, int pause, int bigPause, bool autoWorking, QString filePath, bool onTop);

signals:
    void tick(int minutes, int seconds);
    void timeout(int count);
    void started(enum Timer::TIMER_STATE STATE, int minutes);
    void stopped();
    void zeroCount();
    void error();

private:
    enum TIMER_STATE st = TIMER_IDLE;
};

#endif // TIMER_H
