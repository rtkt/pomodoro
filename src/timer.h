#ifndef TIMER_H
#define TIMER_H

#include <QObject>
#include <QTimerEvent>

class Timer : public QObject
{
    Q_OBJECT

    int work;
    int pause;
    int bigPause;
    bool autoWorking;
    bool relaxing = false; // Are we relaxing after pause?
    bool autoZero;
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
    inline void setWorkTime(int time) {stop(true); work = time;}
    inline void setPauseTime(int time) {stop(true); pause = time;}
    inline void setBigPauseTime(int time) {stop(true); bigPause = time;}
    inline void setAutoWorking(bool val) {autoWorking = val;}
    inline void setAutoZero(bool val) {autoZero = val;}

//    void onSetup(int work, int pause, int bigPause, bool autoWorking,
//                 QString filePath, bool onTop, QByteArray geometry, QString lang, bool autoZero, bool save);

signals:
    void tick(int minutes, int seconds);
    void timeout(int count);
    void started(enum Timer::TIMER_STATE STATE, int minutes);
//    void beforeTimeout();
    void stopped();
    void zeroCount();
    void error();

private:
    enum TIMER_STATE st = TIMER_IDLE;
};

#endif // TIMER_H
