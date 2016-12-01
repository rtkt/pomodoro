#ifndef WIN_H
#define WIN_H

#include <QAction>
#include <QCloseEvent>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QMenu>
#include <QMouseEvent>
#include <QSystemTrayIcon>

#include "settings.h"
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

public slots:
    void onTick(int minutes, int seconds);
    void onTimeout(int count);
    void onStart(enum Timer::TIMER_STATE STATE, int minutes);
    void onStop();
    void onZeroCount();
    void onError();
    void onSetup(int work, int pause, int bigPause, bool autoWorking,
                 QString filePath, bool onTop, QByteArray geometry, bool save);
    void getSettings(bool apply);

private slots:
    void onIconActivation(QSystemTrayIcon::ActivationReason r);
    void onPlayerError(QMediaPlayer::Error error);

signals:
    void newSettings(int work, int pause, int bigPause, bool autoWorking,
                     QString filePath, bool onTop, QByteArray geometry, bool save);
    void gotSettings(int work, int pause, int bigPause, bool autoWorking,
                     QString filePath, bool onTop);

private:
    Ui::Win *ui;

    Timer timer;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QAction *exitAction;
    QAction *settingsAction;
    QMediaPlayer *player = nullptr;
    QString path;
    bool moving = false;
    bool init = true;
//    QString lang;
    struct MinutesLeft {
        int num;
        QString str;
    } minutesLeft;

    struct Point {
        int x;
        int y;
    } lastPos;

    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

    void connectTimer();
    void createTrayIcon();
    void saveSettings(int work, int pause, int bigPause, bool autoWorking,
                      QString filePath, bool onTop, QByteArray geometry);
    void setup();

};

#endif // WIN_H
