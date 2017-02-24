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

    ui->fileBtn->setIcon(QIcon::fromTheme("document-open", QIcon(QString(ICONS_PATH) + "/document-open.png")));

    connect(ui->buttonBox, SIGNAL(accepted()), SLOT(accepted()));
    connect(ui->buttonBox, &QDialogButtonBox::rejected, [=]() {
        close();
    });
    connect(ui->volume, &QSlider::valueChanged, [=](int val) {
       setPercentsDesc(val, ui->volumeText);
    });
    connect(ui->fileBtn, &QPushButton::clicked, this, &Settings::selectFile);
    connect(this, SIGNAL(setAutoWorking(bool)), win, SIGNAL(setAutoWorking(bool)));
    connect(this, SIGNAL(setAutoZero(bool)), win, SIGNAL(setAutoZero(bool)));
    connect(this, SIGNAL(setBigPauseTime(int)), win, SIGNAL(setBigPauseTime(int)));
    connect(this, SIGNAL(setLang(QString)), win, SIGNAL(setLang(QString)));
    connect(this, SIGNAL(setOnTop(bool)), win, SIGNAL(setOnTop(bool)));
    connect(this, SIGNAL(setPath(QString)), win, SIGNAL(setPath(QString)));
    connect(this, SIGNAL(setPauseTime(int)), win, SIGNAL(setPauseTime(int)));
    connect(this, SIGNAL(setWorkTime(int)), win, SIGNAL(setWorkTime(int)));
    connect(this, SIGNAL(setVolume(int)), win, SIGNAL(setVolume(int)));

    populateLangs();
    setup();
}

Settings::~Settings()
{
    delete ui;
}

void Settings::accepted()
{
    QSettings *settings = win->newSettings();
    if(autoWorking != ui->autoStart->isChecked()) {
        emit setAutoWorking(ui->autoStart->isChecked());
        settings->setValue("autoWorking", ui->autoStart->isChecked());
    }
    if(autoZero != ui->autoZero->isChecked()) {
        emit setAutoZero(ui->autoZero->isChecked());
        settings->setValue("autoZero", ui->autoZero->isChecked());
    }
    if(bigPause != ui->bigPause->value()) {
        emit setBigPauseTime(ui->bigPause->value());
        settings->setValue("bigPauseTime", ui->bigPause->value());
    }
    if(filePath != ui->file->text()) {
        emit setPath(ui->file->text());
        settings->setValue("pathToSound", ui->file->text());
    }
    if(lang != ui->language->currentText()) {
        emit setLang(ui->language->currentText());
        settings->setValue("language", ui->language->currentText());
    }
    if(onTop != ui->onTop->isChecked()) {
        emit setOnTop(ui->onTop->isChecked());
        settings->setValue("alwaysOnTop", ui->onTop->isChecked());
    }
    if(pause != ui->pause->value()) {
        emit setPauseTime(ui->pause->value());
        settings->setValue("pauseTime", ui->pause->value());
    }
    if(work != ui->work->value()) {
        emit setWorkTime(ui->work->value());
        settings->setValue("workingTime", ui->work->value());
    }
    if(volume != ui->volume->value()) {
        emit setVolume(ui->volume->value());
        settings->setValue("volume", ui->volume->value());
    }
    delete settings;
    close();
}


void Settings::populateLangs()
{
    // open the language directory
    QDir root(LANG_PATH);
    // get files in the directory with .qm extension
    QFileInfoList flist;
    flist = root.entryInfoList({"*.qm"}, QDir::Files);
    // add the languages to the combo box
    ui->language->addItem("auto");
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

void Settings::setup()
{
    Win::options *st = win->getSettings();
    autoWorking = st->autoWorking;
    autoZero = st->autoZero;
    bigPause = st->bigPause;
    filePath = st->path;
    lang = st->language;
    onTop = st->onTop;
    pause = st->pause;
    work = st->work;
    volume = st->volume;
    delete st;

    ui->autoStart->setChecked(autoWorking);
    ui->autoZero->setChecked(autoZero);
    ui->bigPause->setValue(bigPause);
    ui->file->setText(filePath);
    ui->language->setCurrentText(lang);
    ui->onTop->setChecked(onTop);
    ui->pause->setValue(pause);
    ui->work->setValue(work);
    ui->volume->setValue(volume);

    setPercentsDesc(ui->volume->value(), ui->volumeText);
}
