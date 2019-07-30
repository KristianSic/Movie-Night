#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QColorDialog>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QProcess>
#include <QMessageBox>

#include "setting.h"

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:

    void set_setting();
    explicit Settings(QWidget *parent = nullptr);
    ~Settings();

private slots:
    void mouse_pressed();
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::Settings *ui;
};

#endif // SETTINGS_H
