#ifndef WIN_H
#define WIN_H

#include <QAction>
#include <QCloseEvent>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QMenu>
#include <QMouseEvent>
#include <QPoint>
#include <QSettings>
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
                 QString filePath, bool onTop, bool save);
    void getSettings(bool apply);

private slots:
    void onIconActivation(QSystemTrayIcon::ActivationReason r);

signals:
    void newSettings(int work, int pause, int bigPause, bool autoWorking,
                     QString filePath, bool onTop, bool save);
    void gotSettings(int work, int pause, int bigPause, bool autoWorking,
                     QString filePath, bool onTop);

private:
    Ui::Win *ui;

    Timer timer;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QAction *exitAction;
    QAction *settingsAction;
    QPoint *lastPos;
    QSettings *settings;
    QMediaPlayer *player;
    Settings *settingsWin;
    bool moving = false;
    bool init = true;
//    QString lang;

    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

    void connectTimer();
    void createTrayIcon();
    void setup();
    void setupPlayer(QString filePath);
};

#endif // WIN_H
