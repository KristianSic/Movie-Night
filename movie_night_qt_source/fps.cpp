#include "fps.h"
#include "ui_fps.h"

void FPS::setData(double  fps,int originalFps){
    ui->fps_text->setText(QString::number(fps));
    OriginalFramerate = originalFps;
}

FPS::FPS(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FPS)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
}

FPS::~FPS()
{
    delete ui;
}

void FPS::on_buttonBox_accepted()
{
    bool ok;
    QDialog::done(int(ui->fps_text->text().toDouble(&ok)));
}

void FPS::on_reset_clicked()
{
    ui->fps_text->setText(QString::number(OriginalFramerate));
}
