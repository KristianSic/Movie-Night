#ifndef FRAMEPICKER_H
#define FRAMEPICKER_H

#include <QDialog>
#include <QtCore/QTimer>
#include <QFileInfo>
#include <QElapsedTimer>
#include <QMutex>
#include <QClipboard>
#include <qfiledialog.h>
#include <QDebug>
#include <QTime>
#include <QMessageBox>
#include <QShortcut>
#include <QStandardPaths>
#include <QMovie>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

#include "aspectratiopixmaplabel.h"
#include "player.h"
#include "setting.h"
#include "asmOpenCV.h"
#include "fps.h"

namespace Ui {
class Framepicker;
}


class Framepicker : public QDialog
{
    Q_OBJECT

public:
    QMutex mutex;
    QString main_filename;
    QFileInfo main_name;
    QPoint image_index;
    QSize first_frame_size;
    int curr_position = 0;

    void setData(QString str,QPoint pt,int frame_num,QSize size);
    void play_fade_animation(int durration);
    void on_close();
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);

    explicit Framepicker(QWidget *parent = nullptr);
    ~Framepicker();

private slots:

    QString getFormattedTime(int timeInSeconds);

    void updatePlayerUI(QImage img);

    void done_playing();

    void on_play_clicked();

    void on_prev_frame_clicked();

    void on_next_frame_clicked();

    void on_timeline_sliderMoved(int position);

    void on_timeline_sliderPressed();

    void on_timeline_sliderReleased();

    void on_FPS_clicked();

    void on_clear_clicked();

    void on_load_clicked();

    void on_copy_clicked();

    void on_save_clicked();

    void on_cancel_clicked();

    void on_pick_clicked();

signals:
    void updateUI1Signal(const QPixmap pix,const QPoint pt,const int frame_num);
    void framepicker_closed();
private:

    bool was_runnning = false;

    int m_slider_val = 0;

    int slider_state = 0;

    QPixmap* play_icon;
    QPixmap* pause_icon;

    Ui::Framepicker *ui;
    AspectRatioPixmapLabel *label;
    AspectRatioPixmapLabel *overlay;
    QGraphicsOpacityEffect *eff;

    Player* myPlayer;

};

#endif // FRAMEPICKER_H
