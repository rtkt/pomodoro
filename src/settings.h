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
    bool autoWorking;
    QString filePath;
    bool onTop;
    QString lang;
    const QStringList audio_filetypes = {"*.mp3","*.ogg","*.wav","*.wma",
                                         "*.m4a","*.aac","*.ac3","*.ape",
                                         "*.flac","*.ra","*.mka"};
    void populateLangs();
    void setDesc(int val, QLabel *label);

public slots:
    void selectFile();
    void gotSettings(int work, int pause, int bigPause,
                     bool autoWorking, QString filePath,
                     bool onTop, QString lang);
    void accepted();

signals:
    void changedSettings(int work, int pause, int bigPause,
                         bool autoWorking, QString filePath,
                         bool onTop, QByteArray geom,
                         QString lang, bool save);
    void getSettings(bool apply);
    void checkFile(QString path);
};

#endif // SETTINGS_H
