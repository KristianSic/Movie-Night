#include "framepicker.h"
#include "ui_framepicker.h"

extern setting yyy;

void Framepicker::setData(QString str,QPoint coords,int frame_num,QSize size) //get data from mainwindow
{
    main_filename = str;
    main_name = main_filename; //save fileinfo
    image_index = coords;
    first_frame_size = size;

    if (!main_filename.isEmpty()){
        if (!myPlayer->loadVideo(main_filename.toStdString().data())) //if theres a problem opening the video file
        {
            QMessageBox* msgBox = new QMessageBox();
            msgBox->setText("The selected video could not be opened!");
            msgBox->setAttribute(Qt::WA_DeleteOnClose);
            msgBox->exec();
        }
        else{
            ui->timeline->setValue(0);
            label->clear();
            curr_position = 0;
            ui->curr_time->setText("00:00");
            this->setWindowTitle(main_name.fileName());
            ui->timeline->setMaximum(int(myPlayer->getNumberOfFrames()));
            ui->max_time->setText( getFormattedTime( int(myPlayer->getNumberOfFrames())/int(myPlayer->getFrameRate())));
            ui->max_frame->setText(" / "+QString::number(myPlayer->getNumberOfFrames()));
            myPlayer->setCurrentFrame(frame_num-1);
            myPlayer->getNextFrame();
        }
    }
}

Framepicker::Framepicker(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Framepicker)
{

    myPlayer = new Player();

    QObject::connect(myPlayer, SIGNAL(processedImage(QImage)),this, SLOT(updatePlayerUI(QImage))); //get frame from player object
    QObject::connect(myPlayer, SIGNAL(donePlaying()),this, SLOT(done_playing())); //signal when the movie ends

    ui->setupUi(this);

    auto L = static_cast<QVBoxLayout *>(layout()); //create a layout

    label = new AspectRatioPixmapLabel(); //main laberl for showing video
    label->setStyleSheet("QLabel { background-color : black}");
    label->setAlignment(Qt::AlignCenter);
    L->insertWidget(0,label);

    overlay =  new AspectRatioPixmapLabel(label); //overlay image for pausing playing etc...
    overlay->setAlignment(Qt::AlignCenter);
    overlay->setStyleSheet("background-color: rgba(0,0,0,0%)");

    eff = new QGraphicsOpacityEffect(this); //effect for using fade out

    ui->max_frame->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    ui->curr_frame->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    play_icon = new QPixmap(":/resources/assets/play.png");
    pause_icon = new QPixmap(":/resources/assets/pause.png");

}


void Framepicker::done_playing()
{
    ui->play->setText(tr("Play"));
}

void Framepicker::updatePlayerUI(QImage img) //get new frame from myPlayer
{
    if (!img.isNull())
    {
        label->setPixmap(QPixmap::fromImage(img));
        ui->timeline->setValue(int(myPlayer->getCurrentFrame()));
        ui->curr_time->setText( getFormattedTime( int(myPlayer->getCurrentFrame())/int(myPlayer->getFrameRate())));
        ui->curr_frame->setText(QString::number(int(myPlayer->getCurrentFrame())));
        curr_position = int(myPlayer->getCurrentFrame());
    }
}

QString Framepicker::getFormattedTime(int timeInSeconds) //get formated time from seconds
{
    int seconds = int((timeInSeconds) % 60 );
    int minutes = int(((timeInSeconds / 60) % 60));
    int hours   = int(((timeInSeconds / (60*60)) % 24));
    QTime t(hours, minutes, seconds);
    if (hours == 0 ){
        return t.toString("mm:ss");
    }
    else{
        return t.toString("h:mm:ss");
    }
}

Framepicker::~Framepicker()
{
    delete play_icon;
    delete pause_icon;
    delete eff;
    delete label; //overlay is label child, gets get deleted here
    delete myPlayer;
    delete ui;
}

void Framepicker::on_play_clicked() //play click
{
    double w_h = label->height()*0.50;
    //center the play/pause image indicator
    overlay->setGeometry(int(double(label->width()/double(2))-w_h/2),int(double(label->height()/double(2))-w_h/2),int(w_h),int(w_h));

    if (myPlayer->isStopped())
    {
        overlay->setPixmap(*play_icon);
        myPlayer->Play();
        ui->play->setText(tr("Pause"));
        was_runnning = true;
    }
    else
    {
        overlay->setPixmap(*pause_icon);
        myPlayer->Stop();
        ui->play->setText(tr("Play"));
        was_runnning = false;
    }

    play_fade_animation(2000);
}

void Framepicker::play_fade_animation(int durration)
{
    overlay->setGraphicsEffect(eff);
    QPropertyAnimation *a = new QPropertyAnimation(eff,"opacity");
    a->setDuration(durration);
    a->setStartValue(1);
    a->setEndValue(0);
    a->setEasingCurve(QEasingCurve::OutBack);
    a->start(QPropertyAnimation::DeleteWhenStopped); //deletes itself when done
}
void Framepicker::on_prev_frame_clicked()
{
    myPlayer->Stop();
    myPlayer->getPreviousFrame();
    ui->play->setText(tr("Play"));
}

void Framepicker::on_next_frame_clicked()
{
    myPlayer->Stop();
    myPlayer->getNextFrame();
    ui->play->setText(tr("Play"));
}

void Framepicker::on_timeline_sliderMoved(int position) //when moving the slider
{
    slider_state += 1;

    if(!myPlayer->isStopped())
    {
        m_slider_val = position;
    }

    ui->curr_frame->setText(QString::number(position));
    ui->curr_time->setText( getFormattedTime( int(position)/int(myPlayer->getFrameRate())));
}

void Framepicker::on_timeline_sliderPressed() //press anywhere on the timeline
{
    myPlayer->Stop();
    m_slider_val = ui->timeline->sliderPosition();

}

void Framepicker::on_timeline_sliderReleased()
{
    if(slider_state > 2) //change came from holding and releasing the slider
    {
        myPlayer->setCurrentFrame(ui->timeline->sliderPosition()-1);
        myPlayer->getNextFrame();
    }
    else //change came from clicking on the timeline
    {
        myPlayer->setCurrentFrame(m_slider_val-1);
        myPlayer->getNextFrame();
    }

    if(was_runnning){ //resume playing if the video was playing before
        myPlayer->Play();
    }

    slider_state = 0; //reset state
}

void Framepicker::on_close() //cancel/close modal
{
    Framepicker::close();
    emit framepicker_closed();
}

void Framepicker::keyReleaseEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Right || e->key() == Qt::Key_Left )
    {
        if(was_runnning)
        {
            myPlayer->Play();
            ui->play->setText(tr("Pause"));
        }
    }
}

void Framepicker::keyPressEvent(QKeyEvent *e)
{
    if(myPlayer->isRunning())
    {
        was_runnning = true;
    }
    else
    {
        was_runnning = false;
    }
    //control + right arrow key
    if ((e->key() == Qt::Key_Right)  && (e->modifiers().testFlag(Qt::ControlModifier)))
    {
        int ammount = yyy.get_step_3();
        myPlayer->forward_time(ammount);
    }
    //control + left arrow key
    if ((e->key() == Qt::Key_Left)  && (e->modifiers().testFlag(Qt::ControlModifier)))
    {
        int ammount = yyy.get_step_3();
        myPlayer->rewind_time(ammount);
    }
    //shift + right arrow key
    if ((e->key() == Qt::Key_Right)  && (e->modifiers().testFlag(Qt::ShiftModifier)))
    {
        int ammount = yyy.get_step_2();
        myPlayer->forward_time(ammount);
    }
    //shift + left arrow key
    if ((e->key() == Qt::Key_Left)  && (e->modifiers().testFlag(Qt::ShiftModifier)))
    {
        int ammount = yyy.get_step_2();
        myPlayer->rewind_time(ammount);
    }
    //right arrow key
    if(e->key() == Qt::Key_Right && !(e->modifiers().testFlag(Qt::ControlModifier)) && !(e->modifiers().testFlag(Qt::ShiftModifier)))
    {
        int ammount = yyy.get_step_1();
        myPlayer->forward_time(ammount);
    }
    //left arrow key
    if(e->key() == Qt::Key_Left && !(e->modifiers().testFlag(Qt::ControlModifier)) &&  !(e->modifiers().testFlag(Qt::ShiftModifier)))
    {
        int ammount = yyy.get_step_1();
        myPlayer->rewind_time(ammount);

    }
    //backwards 1 frame
    if(e->key() == Qt::Key_Period)
    {
        on_next_frame_clicked();

    }
    //forward 1 frame
    if(e->key() == Qt::Key_Comma )
    {
        on_prev_frame_clicked();
    }
    //play/pause
    if(e->key() == Qt::Key_Space)
    {
        on_play_clicked();
    }
}

void Framepicker::on_FPS_clicked()
{
    bool temp = was_runnning;

    if(myPlayer->isRunning())
    {
        on_play_clicked();
    }

    FPS* fps = new FPS(this);
    fps->setModal(true);

    fps->setData(myPlayer->getFrameRate(),myPlayer->getOriginalFrameRate());
    fps->setAttribute(Qt::WA_DeleteOnClose);

    int ret = fps->exec();

    if(ret > 0) // if OK was clicked
    {
        myPlayer->setFrameRate(ret); //set new framerate
    }
    if(temp) // resume playing if the video was playing before
    {
        on_play_clicked();
    }
}

void Framepicker::on_clear_clicked()
{
    QPixmap pixmap(first_frame_size.width(),first_frame_size.height());
    pixmap.fill(QColor(yyy.get_color()));
    emit updateUI1Signal(pixmap,image_index,-1); //send placeholder image
    on_close();
}

void Framepicker::on_load_clicked() //load picture from disk
{

    QString f_name = QFileDialog::getOpenFileName(
                this,
                tr("Choose file"),
                QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
                "All files(*.png);;PNG(*.png)"
    );

    if(f_name.isEmpty())
    {
        return;
    }

    QPixmap img(f_name);

    emit updateUI1Signal(img,image_index,-1);
    on_close();
}

void Framepicker::on_copy_clicked() //copy current frame to clipboard
{
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setPixmap(ASM::cvMatToQPixmap(myPlayer->get_frame()));
    delete &clipboard;
}

void Framepicker::on_save_clicked() //save current frame to disk
{
    QString f_name = QFileDialog::getSaveFileName(
                this,
                tr("Save frame"),
                QDir::currentPath(),
                tr(" (*.png)")
    );

    if(!f_name.isNull())
    {
        QFile file(f_name);

        file.open(QIODevice::WriteOnly);
        ASM::cvMatToQPixmap(myPlayer->get_frame()).save(&file, "PNG");

        QMessageBox* msgBox = new QMessageBox();
        msgBox->setText("Photo saved.");
        msgBox->setAttribute(Qt::WA_DeleteOnClose);
        msgBox->setStyleSheet("QLabel{min-width:500 px; font-size: 24px;} QPushButton{ width:500px; font-size: 15px; }");
        msgBox->exec();
    }
}

void Framepicker::on_cancel_clicked()
{
    on_close();
}

void Framepicker::on_pick_clicked() //pick current frame
{
    Mat frame = myPlayer->get_frame();
    emit updateUI1Signal( ASM::cvMatToQPixmap(frame),image_index,curr_position);
    on_close();
}
