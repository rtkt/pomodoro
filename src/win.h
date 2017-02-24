#ifndef WIN_H
#define WIN_H

#include <QAction>
#include <QCloseEvent>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QMenu>
#include <QMouseEvent>
#include <QSettings>
#include <QSystemTrayIcon>
#include <QTranslator>

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

    struct options {
        bool autoWorking;
        bool autoZero;
        int bigPause;
        QString language;
        bool onTop;
        QString path;
        int pause;
        int work;
        int volume;
    };

    inline QSettings* newSettings() {return new QSettings("rtkt", "pomodoro");}
    QTranslator translator;
    QTranslator qtTranslator;

    struct Win::options* getSettings();

public slots:
    void onTick(int minutes, int seconds);
    void onTimeout(int count);
    void onStart(enum Timer::TIMER_STATE STATE, int minutes);
    void onStop();
    void onZeroCount();
    void onError();

private slots:
    void onIconActivation(QSystemTrayIcon::ActivationReason r);
    void onPlayerError(QMediaPlayer::Error error);
    void onSetLang(QString lang);
    void onSetOnTop(bool val);
    inline void onSetPath(QString path) {
//        if(player->media() != QMediaContent()) player->setMedia(QMediaContent());
        player->setMedia(QUrl::fromLocalFile(path));
    }
    inline void onSetVolume(int volume) {player->setVolume(volume);}

signals:
    void setAutoWorking(bool val);
    void setAutoZero(bool val);
    void setBigPauseTime(int time);
    void setLang(QString lang);
    void setOnTop(bool val);
    void setPath(QString path);
    void setPauseTime(int time);
    void setWorkTime(int time);
    void setVolume(int volume);

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
    QString currentLang = "en";
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
    void setup();

};

#endif // WIN_H
