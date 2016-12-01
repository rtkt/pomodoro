#include "timer.h"

void Timer::onClick()
{
    if(st == TIMER_IDLE) {
        start();
    } else {
        stop(false);
    }
}

void Timer::onSetup(int work, int pause, int bigPause,
                    bool autoWorking, QString filePath,
                    bool onTop, QByteArray geometry, bool save)
{
    if(this->work != work || this->pause != pause || this->bigPause != bigPause) {
        if(st != TIMER_IDLE) {
            stop(false);
        }
        this->work = work;
        this->pause = pause;
        this->bigPause = bigPause;
    }
    if(this->autoWorking != autoWorking) {
        this->autoWorking = autoWorking;
    }

    (void)filePath;
    (void)onTop;
    (void)save;
    (void)geometry;
//    (void)lang;
}

void Timer::start(bool pause)
{
    if(st != TIMER_IDLE) {
        emit error();
        return;
    }

    if(pause == false) {
        st = TIMER_WORKING;
        minutes = work;
    } else {
        if(count % 4 == 0 && count != 0) {
            st = TIMER_BIGPAUSE;
            minutes = bigPause;
        } else {
            st = TIMER_PAUSE;
            minutes = this->pause;
        }
    }
    seconds = 0;
    timerId = startTimer(1000);
    emit started(st, minutes);
}

void Timer::stop(bool autoStopped)
{
    killTimer(timerId);
    timerId = 0;
    minutes = 0;
    seconds = 0;
    st = TIMER_IDLE;
    if(!autoStopped) {
        count = 0;
        emit zeroCount();
    }
    emit stopped();
}

void Timer::timerEvent(QTimerEvent *event)
{
    event->accept();
    if(minutes == 0 && seconds == 1) {
        enum TIMER_STATE prevState = st;
        seconds--;
        if(prevState == TIMER_WORKING) {
            count++;
        }
        emit timeout(count);
        stop();
        if(prevState == TIMER_WORKING) {
            start(true);
        } else if(autoWorking == true) {
            start();
        }
        return;
//    } else if(minutes == 0 && seconds == 5) {
//        emit beforeTimeout();
//        seconds--;
    } else if(seconds == 0) {
        seconds = 59;
        minutes--;
    } else {
        seconds--;
    }
    emit tick(minutes, seconds);
}