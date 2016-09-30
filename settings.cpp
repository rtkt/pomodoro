#include "settings.h"
#include "ui_settings.h"

#include "win.h"

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::settings),
    win(static_cast<Win*>(parent))
{
    ui->setupUi(this);
    setWindowModality(Qt::ApplicationModal);
    setAttribute(Qt::WA_DeleteOnClose);

    connect(win, SIGNAL(gotSettings(int,int,int,bool,QString,bool)),
            SLOT(gotSettings(int,int,int,bool,QString,bool)));
    connect(this, SIGNAL(changedSettings(int,int,int,bool,QString,bool,bool)),
            win, SIGNAL(newSettings(int,int,int,bool,QString,bool,bool)));
    connect(this, SIGNAL(getSettings(bool)), win, SLOT(getSettings(bool)));
    connect(ui->buttonBox, SIGNAL(accepted()), SLOT(accepted()));
    connect(ui->buttonBox, &QDialogButtonBox::rejected, [=]() {
        close();
    });
    connect(ui->work, &QSlider::valueChanged, [=](int time) {
        setDesc(time, ui->workText);
    });
    connect(ui->pause, &QSlider::valueChanged, [=](int time) {
        setDesc(time, ui->pauseText);
    });
    connect(ui->bigPause, &QSlider::valueChanged, [=](int time) {
        setDesc(time, ui->bigPauseText);
    });

    emit getSettings(false);
}

Settings::~Settings()
{
    delete ui;
}

void Settings::accepted()
{
    if(work != ui->work->value() || pause != ui->pause->value() ||
            bigPause != ui->bigPause->value() ||
            autoWorking != ui->autoStart->isChecked() ||
            filePath != ui->file->text() ||
            onTop != ui->onTop->isChecked()) {
        emit changedSettings(ui->work->value(), ui->pause->value(),
                             ui->bigPause->value(), ui->autoStart->isChecked(),
                             ui->file->text(), ui->onTop->isChecked(), true);
    }
    close();
}

void Settings::gotSettings(int work, int pause, int bigPause, bool autoWorking,
                           QString filePath, bool onTop)
{
    this->work = work;
    this->pause = pause;
    this->bigPause = bigPause;
    this->autoWorking = autoWorking;
    this->filePath = filePath;
    this->onTop = onTop;

    ui->work->setValue(work);
    ui->pause->setValue(pause);
    ui->bigPause->setValue(bigPause);
    ui->autoStart->setChecked(autoWorking);
    ui->file->setText(filePath);
    ui->onTop->setChecked(onTop);

    setDesc(ui->work->value(), ui->workText);
    setDesc(ui->pause->value(), ui->pauseText);
    setDesc(ui->bigPause->value(), ui->bigPauseText);
}

void Settings::setDesc(int val, QLabel *label)
{
    label->setText(QString::number(val) + " minutes");
}
