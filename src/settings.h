#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QLabel>

namespace Ui {
class settings;
}

class Win;

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = 0);
    ~Settings();

private:
    Ui::settings *ui;

    Win *win;
    int work;
    int pause;
    int bigPause;
    int volume;
    QString filePath;
    QString lang;
    bool autoWorking;
    bool autoZero;
    bool onTop;
    const QStringList audio_filetypes = {"*.mp3","*.ogg","*.wav","*.wma",
                                         "*.m4a","*.aac","*.ac3","*.ape",
                                         "*.flac","*.ra","*.mka"};
    void populateLangs();
    void setup();
    inline void setMinutesDesc(int val, QLabel *label) {label->setText(QString::number(val) + tr(" minute(s)"));}
    inline void setPercentsDesc(int val, QLabel *label) {label->setText(QString::number(val) + "%");}

public slots:
    void selectFile();
    void accepted();

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
};

#endif // SETTINGS_H
