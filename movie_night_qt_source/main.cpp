#include "mainwindow.h"
#include "setting.h"

#include <QApplication>
#include <QFile>
#include <QDebug>
#include <QFileInfo>

setting yyy; //global settings object

int main(int argc, char *argv[])
{
    QFileInfo check_file("settings.dat");

    if (check_file.exists() && check_file.isFile()) //check if settings file exists
    {
        QFile file("settings.dat");
        file.open(QIODevice::ReadOnly); //open it
        QDataStream in(&file);

        QString theme,autogen,showFrame,step_1,step_2,step_3,cols,rows,color;

        bool theme_val,autogen_val,showFrame_val;

        int step_1_val,step_2_val,step_3_val;

        int cols_val,rows_val;

        QString color_val;

        //read all the parameters from file
        in >> theme     >> theme_val
           >> autogen   >> autogen_val
           >> showFrame >> showFrame_val
           >> step_1    >> step_1_val
           >> step_2    >> step_2_val
           >> step_3    >> step_3_val
           >> cols      >> cols_val
           >> rows      >> rows_val
           >> color     >> color_val;

        file.close();

        //set the settings object
        yyy = setting(theme_val,autogen_val,showFrame_val,step_1_val,step_2_val,step_3_val,cols_val,rows_val,color_val);

    }
    else //doesnt exist
    {
        yyy = setting(true,true,false,5,10,25,3,3,QString("#8E2DC5")); //create default values
    }

    QCoreApplication::setApplicationName("GRID generator");
    QCoreApplication::setAttribute(Qt::AA_X11InitThreads);

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    a.setStyle("fusion");
    a.setWindowIcon(QIcon(":/resources/assets/film-roll.svg"));

    if(yyy.get_theme()) //change ui color scheme if dark theme
    {
        QPalette p;
        p = qApp->palette();
        p.setColor(QPalette::Window, QColor(53,53,53));
        p.setColor(QPalette::WindowText, QColor(255,255,255));
        p.setColor(QPalette::Button, QColor(53,53,53));
        p.setColor(QPalette::Highlight, QColor(142,45,197));
        p.setColor(QPalette::ButtonText, QColor(255,255,255));
        qApp->setPalette(p);
    }

    return a.exec();
}
