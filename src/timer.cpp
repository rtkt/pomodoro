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

void Timer::setBigPauseTime(int time)
{
    if(st != TIMER_IDLE)
        stop(true);
    else {
        count = 0;
        emit zeroCount();
    }
    bigPause = time;
}

void Timer::setPauseTime(int time)
{
    if(st != TIMER_IDLE)
        stop(true);
    else {
        count = 0;
        emit zeroCount();
    }
    pause = time;
}

void Timer::setWorkTime(int time)
{
    if(st != TIMER_IDLE)
        stop(true);
    else {
        count = 0;
        emit zeroCount();
    }
    work = time;
}

// Choose the mode and start
void Timer::start(bool pause)
{
    if(st != TIMER_IDLE) {
        emit error();
        return;
    }

    if(autoZero && relaxing) {
        killTimer(timerId);
        relaxing = false;
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

// Stop timer, start slack timer (the one that
// zeroes out pomodoros count after some time) if needed
void Timer::stop(bool autoStopped)
{
    killTimer(timerId);
    timerId = 0;
    minutes = 0;
    seconds = 0;
    if(st == TIMER_PAUSE && autoStopped && autoZero) {
        timerId = startTimer(pause * 30 * 1000);
        relaxing = true;
    } else if(st == TIMER_BIGPAUSE && autoStopped && autoZero) {
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
    if(relaxing && autoZero) {
        stop(false);
        relaxing = false;
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
