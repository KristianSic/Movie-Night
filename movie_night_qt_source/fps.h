#ifndef FPS_H
#define FPS_H

#include <QDialog>

namespace Ui {
class FPS;
}

class FPS : public QDialog
{
    Q_OBJECT

public:

    void setData(double  fps,int originalFps);

    explicit FPS(QWidget *parent = nullptr);
    ~FPS();

private slots:
    void on_buttonBox_accepted();
    void on_reset_clicked();

private:
    int OriginalFramerate;
    Ui::FPS *ui;
};

#endif // FPS_H
