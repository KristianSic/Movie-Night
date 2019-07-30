#ifndef SETTING_H
#define SETTING_H

#include <QSize>
#include <QString>

class setting
{
public:
    setting();
    setting(bool theme,bool autogen,bool showFrame,int step_1,int step_2,int step_3,int cols, int rows, QString color);


    setting get_setting();

    bool get_theme();
    bool get_autogen();
    bool get_showFrame();

    int get_step_1();
    int get_step_2();
    int get_step_3();

    int get_cols();
    int get_rows();

    QString get_color();


private:

    bool theme;
    bool autogen;
    bool showFrame;

    int step_1 = 5;
    int step_2 = 10;
    int step_3 = 25;

    int cols = 3;
    int rows = 3;

    QString color;

};

#endif // SETTING_H
