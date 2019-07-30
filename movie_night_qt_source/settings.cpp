#include "settings.h"
#include "ui_settings.h"

extern setting yyy;

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);

    set_setting();

    this->setWindowFlags ( Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::MSWindowsFixedSizeDialogHint);
    this->setWindowTitle("Settings");

    connect(ui->color_label, SIGNAL(clicked(QPoint&)), this, SLOT(mouse_pressed()));

    ui->color_label->coords.setX(0);
    ui->color_label->coords.setY(0);
}

Settings::~Settings()
{
    delete ui;
}

void Settings::mouse_pressed()
{
    QColor color = QColorDialog::getColor(yyy.get_color(), this );

    if(color.isValid())
    {
        QPixmap pixmap(1,1);
        pixmap.fill(QColor(color.name()));
        ui->color_label->setPixmap(pixmap);
    }
}

void Settings::set_setting()
{
    ui->theme->setChecked(yyy.get_theme());
    ui->autogen->setChecked(yyy.get_autogen());
    ui->showFrame->setChecked(yyy.get_showFrame());

    ui->step_1->setText(QString::number(yyy.get_step_1()));
    ui->step_2->setText(QString::number(yyy.get_step_2()));
    ui->step_3->setText(QString::number(yyy.get_step_3()));

    ui->cols->setText(QString::number(yyy.get_cols()));
    ui->rows->setText(QString::number(yyy.get_rows()));

    QPixmap pixmap(1,1);
    pixmap.fill(QColor(yyy.get_color()));
    ui->color_label->setPixmap(pixmap);
}

void Settings::on_buttonBox_accepted()
{
    QMessageBox msgBox;
    msgBox.setText("Restart to apply?.");
    msgBox.setInformativeText("Do you want to applly your changes?");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    int ret = msgBox.exec();

    bool ready_save = false;

    switch (ret)
    {
    case QMessageBox::Save:   //save and restart

        ready_save = true;
        break;
    case QMessageBox::Discard: //reset settings
        set_setting();
        break;
    case QMessageBox::Cancel:  //close modal
        // Cancel was clicked
        break;
    default:
        // should never be reached
        break;
    }

    if(ready_save) //user wants to save changed data
    {
        QImage image(ui->color_label->pixmap()->toImage());
        QColor color(image.pixel(0, 0));

        yyy = setting(
                    ui->theme->isChecked(),
                    ui->autogen->isChecked(),
                    ui->showFrame->isChecked(),

                    ui->step_1->text().toInt(),
                    ui->step_2->text().toInt(),
                    ui->step_3->text().toInt(),

                    ui->cols->text().toInt(),
                    ui->rows->text().toInt(),

                    color.name()
                    );

        QFile file("settings.dat");
        file.open(QIODevice::WriteOnly);
        QDataStream out(&file);   // we will serialize the data into the file

        out << QString("dark_theme");
        out << ui->theme->isChecked();

        out << QString("autogen");
        out << ui->autogen->isChecked();

        out << QString("show_frame_nuber_on_tile");
        out << ui->showFrame->isChecked();

        out << QString("arrow_keys_time_step");
        out << ui->step_1->text().toInt();

        out << QString("arrow_keys_plus_shift_time_step");
        out << ui->step_2->text().toInt();

        out << QString("arrow_keys_plus_ctrl_time_step");
        out << ui->step_3->text().toInt();

        out << QString("column_count");
        out << ui->cols->text().toInt();

        out << QString("row_count");
        out << ui->rows->text().toInt();

        out << QString("placeholder_color");
        out << color.name();

        file.close();
        Settings::close();

        //quit and restart the app to apply the changes
        qApp->quit();
        QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
    }
}

void Settings::on_buttonBox_rejected()
{
    Settings::close();
}
