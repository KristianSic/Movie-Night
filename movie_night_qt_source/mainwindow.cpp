#include "mainwindow.h"
#include "ui_mainwindow.h"

extern setting yyy;

MainWindow::MainWindow(QWidget *parent) : //window constructor, set up stuff here
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle("Movie night");
    this->setWindowIcon(QIcon(":/resources/assets/film-roll.svg"));

    //disable buttons until movie is loaded
    ui->clear_all->setEnabled(false);
    ui->ratio_centre->setEnabled(false);
    ui->export_grid->setEnabled(false);

    QImage image(QPixmap(":/resources/assets/cog.png").toImage());

    if(yyy.get_theme()) //invert settings icon if theme is dark
    {
        image.invertPixels();
    }

    QIcon icon;
    icon.addPixmap(QPixmap::fromImage(image));
    ui->button_settings->setIcon(icon);

    QPixmap placeholder_pix(1,1);
    placeholder_pix.fill(QColor(yyy.get_color()));

    int width = yyy.get_cols();
    int height = yyy.get_rows();

    for(int i=0;i<height;i++) //creates a vector of vectors of AspectRatioPixmapLabels
    {
        std::vector<AspectRatioPixmapLabel*> temp_labels;

        for(int j=0;j<width;j++)
        {
            AspectRatioPixmapLabel* temp_label = new AspectRatioPixmapLabel();
            temp_label->setScaledContents( true );
            temp_label->setPixmap(placeholder_pix);
            temp_label->setAlignment(Qt::AlignCenter);
            temp_label->coords.setX(i);
            temp_label->coords.setY(j);
            temp_labels.push_back(temp_label);
        }
        buttons.push_back(temp_labels); //labels that act like buttons :)
    }

    for(uint i=0;i<uint(height);i++) //insert labels to grid and connect "clicked" signals
    {
        for(uint j=0;j<uint(width);j++)
        {
            ui->grid_insert->addWidget(buttons[i][j], int(i), int(j));
            connect(buttons[i][j], SIGNAL(clicked(QPoint&)), this, SLOT(mouse_pressed(QPoint&)));
        }
    }

    ui->FilenameLineEdit->installEventFilter(this); //set event filter for filename edit box (for onclicked)
    ui->FilenameLineEdit->setReadOnly(true);

    for(int i = 0; i < height ; i++) //fill the vectors for images and image numbers with placeholders
    {
        std::vector<QPixmap> temp_mats;
        std::vector<int> temp_frames;

        for(int j = 0; j< width; j++)
        {
            temp_mats.push_back(placeholder_pix);
            temp_frames.push_back(0);
        }
        mats.push_back(temp_mats);
        mats_frames.push_back(temp_frames);
    }

    //resize the window to 75% of the users screen width and height
    QRect desktopRect(QApplication::desktop()->availableGeometry(this));
    QWidget::resize(int( double(desktopRect.width()) * 0.75),int( double(desktopRect.height()) * 0.75));

    //center the window
    QWidget::setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            QWidget::size(),
            QGuiApplication::primaryScreen()->availableGeometry()
        )
    );
}

MainWindow::~MainWindow() //destructor
{
    for (auto p : buttons) //clear all dynamic labels from vectors
    {
        for (auto q : p)
        {
            delete q;
        }
        p.clear();
    }

    buttons.clear();
    mats.clear();
    mats_frames.clear();

    delete ui;
}

void MainWindow::fix_aspect_ratio() //resize the window based on the loaded movies aspect ratio + grid size
{
    QRect desktopRect = QApplication::desktop()->availableGeometry(this); //users screen dimension
    QRect geometry = ui->grid_insert->geometry(); //label grid dimensions

    int w_offset = this->width()-geometry.width(); // window width - photo container width
    int h_offset = this->height()-geometry.height(); // window height - photo container height

    double screen_window_ratio = double(desktopRect.width())/double(desktopRect.height());
    double aspect_ratio = (double(first_frame.cols) * yyy.get_cols()) / (double(first_frame.rows)  * yyy.get_rows()); //movie aspect ratio when acounting grid size

    double widget_window_ratio = double(this->width())/double(this->height()); //aspect ratio of the whole window

    if(widget_window_ratio<=screen_window_ratio)  //longer height wise based on ratio -> set height ,adjust width
    {
        double perfect_height = this->height()-h_offset;
        QWidget::resize(int((perfect_height*aspect_ratio)+w_offset),int(perfect_height+h_offset));
    }
    else //longer width wise based on ratio -> set width ,adjust height
    {
        double perfect_width = this->width()-w_offset;
        QWidget::resize(int(perfect_width+w_offset),int((perfect_width/aspect_ratio)+h_offset));
    }

    //center window
    QWidget::setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            QWidget::size(),
            QGuiApplication::primaryScreen()->availableGeometry()
        )
    );
}

void MainWindow::on_button_settings_clicked() //open settings window
{
    Settings* settings = new Settings(this);
    settings->setModal(true);
    settings->setAttribute(Qt::WA_DeleteOnClose);
    settings->exec();
}

void MainWindow::mouse_pressed(QPoint& coords) //fires when clicking on label
{
    if(filename.isEmpty()){ //if no movie selected open file dialog
        on_open_file_clicked();
        return;
    }

    //open framepicker
    frame = new Framepicker(this);
    frame->setModal(true);
    frame->setWindowFlags(Qt::Dialog | Qt::Desktop); //no close button
    //call first to set some initial data
    frame->setData(filename,coords, mats_frames[uint(coords.x())][uint(coords.y())],QSize(first_frame.cols,first_frame.rows));
    frame->setAttribute(Qt::WA_DeleteOnClose);
    //connect signals for picking frame and when the framepicker window closes
    connect( frame, SIGNAL(updateUI1Signal(const QPixmap,const QPoint,const int)), this, SLOT(update_frame(const QPixmap,const QPoint,const int)));
    connect( frame, SIGNAL(framepicker_closed()), this, SLOT(framepicker_delete()));
    frame->open();
}

bool MainWindow::eventFilter(QObject *object, QEvent *event) //event filter
{
    if(object == ui->FilenameLineEdit && event->type() == QEvent::MouseButtonPress) //if the event is mouse click on line edit open file dialog
    {
        on_open_file_clicked();
    }

    return QMainWindow::eventFilter(object, event);
}

void MainWindow::update_frame(const QPixmap pix,const QPoint coords,const int frame_num) //gets picked frame from framepicker
{
    uint x = uint(coords.x());
    uint y = uint(coords.y());

    mats[x][y] = pix; //for exporting image
    buttons[x][y]->clearMask();

    if(frame_num > 0 ) //if a "real" frame was sent
    {
        buttons[x][y]->setScaledContents( false ); //no stretch on image

        if(yyy.get_showFrame()) //if "show frames" is active
        {
            QImage image(pix.toImage().convertToFormat(QImage::Format_ARGB32));
            QPainter p(&image);
            p.setPen(QPen(Qt::red));
            p.setFont(QFont("Times", 40, QFont::Bold));
            p.drawText(image.rect(), " " + QString::number(frame_num));
            buttons[x][y]->setPixmap(QPixmap::fromImage(image));
        }
        else
        {
            buttons[x][y]->setPixmap(pix);
        }
        mats_frames[x][y] = frame_num;
    }
    else // eighter picture was loaded from disk or this frame was "cleared" (sends placeholder image)
    {
        buttons[x][y]->setScaledContents( true ); //image stretches on resize
        buttons[x][y]->setPixmap(pix);
        mats_frames[x][y] = 0;
    }
}

void tile(const std::vector<Mat> &src, Mat &dst, int grid_x, int grid_y) //tile grid images in one image
{
    // patch size
    int width  = dst.cols/grid_x;
    int height = dst.rows/grid_y;
    // iterate through grid
    uint k = 0;
    for(int i = 0; i < grid_y; i++) {
        for(int j = 0; j < grid_x; j++) {
            Mat s = src[k++];
            resize(s,s,Size(width,height));
            s.copyTo(dst(Rect(j*width,i*height,width,height)));
        }
    }
}

bool save_file(Mat res, QString filename) //save  image to disk
{
    QFile file(filename);
    file.open(QIODevice::WriteOnly);
    ASM::cvMatToQPixmap(res).save(&file, "PNG");

    return true;
}

void MainWindow::on_ratio_centre_clicked()
{
    fix_aspect_ratio();
}

void MainWindow::framepicker_delete(){

    delete frame;
}

void MainWindow::on_clear_all_clicked() //clear all images from grid
{
    QPixmap pixmap(1,1);
    pixmap.fill(QColor(yyy.get_color()));

    for(uint i=0;i<uint(yyy.get_rows());i++) //set all grid images to placeholders
    {
        for(uint j=0;j<uint(yyy.get_cols());j++)
        {
            buttons[i][j]->setScaledContents(true);
            buttons[i][j]->setPixmap(pixmap);
            mats[uint(i)][uint(j)] = pixmap;
            mats_frames[uint(i)][uint(j)] = 0;
        }
    }
}

void MainWindow::on_export_grid_clicked() //export the grid
{
    QString f_name = QFileDialog::getSaveFileName(
                this,
                tr("Save prhotogrid"),
                QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
                tr(" (*.png)"));

    if( !f_name.isNull())
    {
        int cols = yyy.get_cols();
        int rows = yyy.get_rows();

        std::vector<Mat> imgs;

        for(int i=0;i<rows;i++)
        {
            for(int j=0;j<cols;j++)
            {
                imgs.push_back(ASM::QPixmapToCvMat( mats[uint(i)][uint(j)], false ));
            }
        }

        Mat res = Mat(first_frame.rows * rows,first_frame.cols * cols,CV_8UC3); //prepare output dimensions , gets writen on by tile function
        tile(imgs,res,cols,rows);

        QFuture<bool> future = QtConcurrent::run( save_file,res,f_name );  // new thread

        if(future.result()) //notify user when the image is completely saved
        {
            QMessageBox* msgBox = new QMessageBox;
            msgBox->setText("Saved.");
            msgBox->setAttribute(Qt::WA_DeleteOnClose);
            msgBox->setStyleSheet("QLabel{min-width:500 px; font-size: 24px;} QPushButton{ width:500px; font-size: 15px; }");
            msgBox->exec();
        }
    }
}

void MainWindow::on_open_file_clicked() //open file dialog
{
    QString temp_filename = QFileDialog::getOpenFileName(
                this,
                tr("Choose file"),
                QStandardPaths::writableLocation(QStandardPaths::DownloadLocation),
                "All files(*.mkv *.mp4 *.avi *.webm);;MKV(*.mkv);;mp4(*.mp4);;avi(*.avi);;webm(*.webm)"
    );

    if(temp_filename.isEmpty())
    {
        return;
    }

    filename = temp_filename;

    //upon succesfull load enable buttons and set filename text
    ui->clear_all->setEnabled(true);
    ui->ratio_centre->setEnabled(true);
    ui->export_grid->setEnabled(true);
    ui->FilenameLineEdit->setText(filename);

    cv::VideoCapture capp(filename.toStdString()); //create a temporary VC object for loading frames

    capp.read(first_frame); //read first frame to get dimensions

    fix_aspect_ratio(); //prepare window size

    if(yyy.get_autogen()){ //if autogen images is on

        cv::Mat imgg;
        const int num_req_frames = yyy.get_cols() * yyy.get_rows();
        int total_frames = int(capp.get(CAP_PROP_FRAME_COUNT));
        double magic_num = 0.0028; //time in percentage when a lot of movies/shows have intro cards
        int step = (total_frames-int(double(total_frames)*(magic_num*2)))/num_req_frames;

        if(capp.isOpened()){

            int counter = 0;
            int frm = int(double(total_frames)*magic_num); //offset to avoid black screen
            std::vector<int>frms;

            for(int N = 0;N<yyy.get_rows();N++) //load frame numbers into a vector
            {
                for(int M = 0;M<yyy.get_cols();M++)
                {
                    frms.push_back(frm);
                    frm+=step;
                }
            }

            int middle = yyy.get_cols()/2;
            int middle_index = middle * yyy.get_cols() + middle;

            //switch first frame with the one in the middle
            frms[0] = frms[uint(middle_index)];
            frms[uint(middle_index)] = int(double(total_frames)*magic_num);

            for(int N = 0;N<yyy.get_rows();N++) //get actual frames from the movie
            {
                for(int M = 0;M<yyy.get_cols();M++)
                {
                    capp.set(CAP_PROP_POS_FRAMES, frms[uint(counter)]);
                    capp.read(imgg);
                    update_frame(ASM::cvMatToQPixmap(imgg),QPoint(N,M),frms[uint(counter)]); //set the grid image
                    counter++;
                    qApp->processEvents(); //give QT time to actually show the images one by one
                }
            }
            /*
            //sometimes causes half loaded frames, not very stable but "works"
            //dont forget to include theese in the .pro file
            //LIBS += -fopenmp
            //QMAKE_CXXFLAGS += -fopenmp

            std::size_t idx;
            std::size_t idx2;

            #pragma omp parallel default(shared) private(idx, idx2)
            {
              cv::VideoCapture kapp(filename.toStdString());
              for(std::size_t idx=0;idx<yyy.get_rows();idx++)
              {
                #pragma omp for
                for(std::size_t idx2=0;idx2<yyy.get_cols();idx2++)
                {
                    kapp.set(CAP_PROP_POS_FRAMES, frms[(idx*yyy.get_rows())+idx2]);
                    kapp.read(imgg);
                    update_frame(ASM::cvMatToQPixmap(imgg),QPoint(idx,idx2),frms[uint((idx*yyy.get_rows())+idx2)]);
                    qApp->processEvents();
                    qDebug()<<"From thread: "<<omp_get_thread_num()<<" idx: "<<idx<<" idx2: "<<idx2;
                }
              }
            }*/
        }
    }
}
