#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPixmap>
#include <QtDebug>
#include <QPushButton>
#include <QSize>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QFileDialog>
#include <QStandardPaths>
#include <QThread>
#include <QtConcurrent/QtConcurrent>
#include <QPainter>
#include <QResizeEvent>
#include <QStyle>
#include <QGuiApplication>
#include <QScreen>

#include <cstdlib>
#include <omp.h>

#include "framepicker.h"
#include "setting.h"
#include "aspectratiopixmaplabel.h"
#include "settings.h"
#include "opencv2/highgui.hpp"
#include "asmOpenCV.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    bool eventFilter(QObject *, QEvent*);
    void fix_aspect_ratio();

    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void mouse_pressed(QPoint& coords);
    void on_button_settings_clicked();
    void on_ratio_centre_clicked();
    void on_clear_all_clicked();
    void on_export_grid_clicked();

    void on_open_file_clicked();

public slots:

    void update_frame( const QPixmap pix, const QPoint coords,const int frame_num);// set new text in mainwindow form
    void framepicker_delete();

private:
    std::vector<std::vector<AspectRatioPixmapLabel*>> buttons;

    QString filename;
    Framepicker *frame;
    cv::Mat first_frame;
    std::vector<std::vector<QPixmap>>mats;
    std::vector<std::vector<int>>mats_frames;
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
