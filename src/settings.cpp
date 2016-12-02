#include "settings.h"
#include "ui_settings.h"

#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

#include "win.h"

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::settings),
    win(static_cast<Win*>(parent))
{
    ui->setupUi(this);

//    Set this windows modal
    setWindowModality(Qt::ApplicationModal);

//    Delete this window on closing
    setAttribute(Qt::WA_DeleteOnClose);

    ui->fileBtn->setIcon(QIcon::fromTheme("document-open", QIcon(QString(ICONS_PATH) + "/file_open.png")));

    connect(win, SIGNAL(gotSettings(int,int,int,bool,QString,bool,QString)),
            SLOT(gotSettings(int,int,int,bool,QString,bool,QString)));
    connect(this, SIGNAL(changedSettings(int,int,int,bool,QString,bool,QByteArray,QString,bool)),
            win, SIGNAL(newSettings(int,int,int,bool,QString,bool,QByteArray,QString,bool)));
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
    connect(ui->fileBtn, &QPushButton::clicked, this, &Settings::selectFile);

    populateLangs();
    emit getSettings(false);
}

Settings::~Settings()
{
    delete ui;
}

// Do we need to save settings before closing?
void Settings::accepted()
{
    if(work != ui->work->value() || pause != ui->pause->value() ||
            bigPause != ui->bigPause->value() ||
            autoWorking != ui->autoStart->isChecked() ||
            filePath != ui->file->text() ||
            onTop != ui->onTop->isChecked() ||
            lang != ui->language->currentText()) {
        emit changedSettings(ui->work->value(), ui->pause->value(),
                             ui->bigPause->value(), ui->autoStart->isChecked(),
                             ui->file->text(), ui->onTop->isChecked(), QByteArray(),
                             ui->language->currentText(), true);
    }
    close();
}


// We got signal 'gotSettings()' and we need to see what's in...
void Settings::gotSettings(int work, int pause, int bigPause, bool autoWorking,
                           QString filePath, bool onTop, QString lang)
{
    this->work = work;
    this->pause = pause;
    this->bigPause = bigPause;
    this->autoWorking = autoWorking;
    this->filePath = filePath;
    this->onTop = onTop;
    this->lang = lang;

    ui->work->setValue(work);
    ui->pause->setValue(pause);
    ui->bigPause->setValue(bigPause);
    ui->autoStart->setChecked(autoWorking);
    ui->file->setText(filePath);
    ui->onTop->setChecked(onTop);
    ui->language->setCurrentText(lang);

    setDesc(ui->work->value(), ui->workText);
    setDesc(ui->pause->value(), ui->pauseText);
    setDesc(ui->bigPause->value(), ui->bigPauseText);
}

void Settings::populateLangs()
{
    // open the language directory
    QDir root(LANG_PATH);
    // get files in the directory with .qm extension
    QFileInfoList flist;
    flist = root.entryInfoList({"*.qm"}, QDir::Files);
    // add the languages to the combo box
    ui->language->addItem("en");
    for(auto &i : flist)
    {
        QString lang = i.fileName();
        lang.chop(3); // -  .qm
        ui->language->addItem(lang);
    }
}

// Setup and open a file chooser
void Settings::selectFile()
{
    QString newPath = QFileDialog::getOpenFileName(this,
                                                   tr("Select an audio file"),
                                                   QDir::homePath(),
                                                   audio_filetypes.join(" "));
    if(newPath != QString()) {
        ui->file->setText(newPath);
    }
}

void Settings::setDesc(int val, QLabel *label)
{
    label->setText(QString::number(val) + tr(" minute(s)"));
}
