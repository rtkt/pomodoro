#include "timer.h"

// Recieved on main (start/stop) button click
void Timer::onClick()
{
    if(st == TIMER_IDLE) {
        start();
    } else {
        stop(false);
    }
}

// Apply settings
void Timer::onSetup(int work, int pause, int bigPause,
                    bool autoWorking, QString filePath,
                    bool onTop, QByteArray geometry, QString lang, bool save)
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
    (void)lang;
}

// Choose the mode and start
void Timer::start(bool pause)
{
    if(st != TIMER_IDLE) {
        emit error();
        return;
    }

    killTimer(timerId);
    relaxing = false;

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

// Stop timer, start slack timer (the one that
// zeroes out pomodoros count after some time) if needed
void Timer::stop(bool autoStopped)
{
    killTimer(timerId);
    timerId = 0;
    minutes = 0;
    seconds = 0;
    if(st == TIMER_PAUSE && autoStopped) {
        timerId = startTimer(pause * 30 * 1000);
        relaxing = true;
    } else if(st == TIMER_BIGPAUSE && autoStopped) {
        timerId = startTimer(bigPause * 30 * 1000);
        relaxing = true;
    }
    st = TIMER_IDLE;
    if(!autoStopped) {
        count = 0;
        emit zeroCount();
    }
    emit stopped();
}

// That's a tick function
void Timer::timerEvent(QTimerEvent *event)
{
    if(relaxing) {
        stop(false);
        killTimer(timerId);
        return;
    }

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
    } else if(seconds == 0) {
        seconds = 59;
        minutes--;
    } else {
        seconds--;
    }

    if(minutes < 0 || seconds < 0 || seconds >= 60) {
        stop(false);
        emit error();
        return;
    }

    emit tick(minutes, seconds);
}
