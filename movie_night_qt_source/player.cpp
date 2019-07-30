#include "player.h"

extern setting yyy;

Player::Player(QObject *parent)
    : QThread(parent)
{
    stop = true;
    ended = false;
}

bool Player::loadVideo(QString filename)
{
    capture.open(filename.toStdString());
    if (capture.isOpened())
    {
        frameRate = int(capture.get(CAP_PROP_FPS));
        return true;
    }
    else{
        return false;
    }
}

void Player::Play()
{
    if (!isRunning())
    {
        if (isStopped())
        {
            stop = false;
        }
        start(LowPriority);
    }
}

void Player::setFrameRate(int fps)
{
    frameRate = fps;
}

void Player::run()
{
    capture.read(frame);

    clock_t a;
    int delay = (1000/frameRate);

    while(!stop)
    {
        a = clock(); //measure time from here

        if (capture.read(frame))
        {
            emit processedImage(ASM::cvMatToQImage(frame));
        }
        else
        {
            stop = true;
            emit donePlaying();
            setCurrentFrame(0);
            capture.read(frame);

            emit processedImage(ASM::cvMatToQImage(frame));
            break;
        }
        this->msleep(delay-int(clock()-a)); //to here, get time it took to convert and emit new frame and subtract from time step to
                                            //get right amount of frames in a second (not really accurate still better than just "delay")
    }
}

Player::~Player()
{
    mutex.lock();
    stop = true;
    capture.release();
    condition.wakeOne();
    mutex.unlock();
    wait();
}

void Player::Stop()
{
    stop = true;
}

void Player::msleep(int ms)
{
    struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
    nanosleep(&ts, nullptr);
}

bool Player::isStopped() const
{
    return this->stop;
}

double Player::getCurrentFrame()
{
    return capture.get(CAP_PROP_POS_FRAMES);
}

double Player::getNumberOfFrames()
{
    return capture.get(CAP_PROP_FRAME_COUNT);
}

double Player::getFrameRate()
{
    return frameRate;
}

int Player::getOriginalFrameRate()
{
    return int(capture.get(CAP_PROP_FPS));
}

void Player::getNextFrame()
{
    if(capture.read(frame)){
        emit processedImage(ASM::cvMatToQImage(frame));
    }
}

void Player::getPreviousFrame()
{
    capture.set(CAP_PROP_POS_FRAMES, getCurrentFrame()-2);

    if(capture.read(frame))
    {
        emit processedImage(ASM::cvMatToQImage(frame));
    }
}

void Player::setCurrentFrame( int frameNumber )
{
    capture.set(CAP_PROP_POS_FRAMES, frameNumber);
}

Mat Player::get_frame()
{
    return frame;
}

void Player::forward_time(int seconds)
{
    stop = true;

    check_aggain: //awoid crashing when holding the forward/backwards time button, make sure player is stopped when forwarding
        if (!isRunning())
        {
            if (isStopped())
            {
                setCurrentFrame(int(getCurrentFrame() + getFrameRate()*seconds)-1);
                getNextFrame();
                return;
            }
        }
    goto check_aggain;
}

void Player::rewind_time(int seconds)
{
    stop = true;

    check_aggain:
        if (!isRunning())
        {
            if (isStopped())
            {
                setCurrentFrame(int(getCurrentFrame() - getFrameRate()*seconds)-1);
                getNextFrame();
                return;
            }
        }
    goto check_aggain;
}
