#ifndef PLAYER_H
#define PLAYER_H

#include <QMutex>
#include <QThread>
#include <QImage>
#include <QWaitCondition>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QDebug>
#include <vector>
#include <QTime>

#include "asmOpenCV.h"
#include "setting.h"

using namespace cv;
class Player : public QThread
{    Q_OBJECT
 private:

    bool locked;
    bool stop;
    bool ended;
    QMutex mutex;
    QMutex mutex2;
    QWaitCondition condition;
    Mat frame;
    int frameRate;
    VideoCapture capture;
    Mat RGBframe;
    //QImage img;
 signals:
 //Signal to output frame to be displayed
      void processedImage(const QImage &image);
      void donePlaying();
 protected:
     void run();
     void msleep(int ms);
 public:
    //Constructor
    Player(QObject *parent = 0);
    //Destructor
    ~Player();
    //Load a video from memory
    bool loadVideo(QString filename);
    //Play the video
    void Play();
    //Stop the video
    void Stop();
    //check if the player has been stopped
    bool isStopped() const;

    void setCurrentFrame( int frameNumber);

    double getFrameRate();
    void setFrameRate(int framerate);
    double getCurrentFrame();
    double getNumberOfFrames();
    int getOriginalFrameRate();

    void getNextFrame();
    void getPreviousFrame();

    void forward_time(int seconds);
    void rewind_time(int seconds);

     Mat get_frame();
    //std::vector<QImage> trail;
    // int trail_size = 0;
    // int trail_limit = 10;
};
#endif // PLAYER_H
