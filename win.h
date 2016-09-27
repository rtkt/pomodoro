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
    void onSetup(int work, int pause, int bigPause, bool autoWorking, QString filePath, bool onTop);

private slots:
    void onIconActivation(QSystemTrayIcon::ActivationReason r);
    void getSettings(bool apply);

signals:
    void newSettings(int work, int pause, int bigPause, bool autoWorking, QString filePath, bool onTop);
    void gotSettings(int work, int pause, int bigPause, bool autoWorking, QString filePath, bool onTop);

private:
    Ui::Win *ui;

    Timer timer;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QAction *exitAction;
    QPoint *lastPos;
    QSettings *settings;
    QMediaPlayer *player;
    bool isModalOpened = false;
    bool moving = false;

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
