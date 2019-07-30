#include "setting.h"

setting::setting(bool Theme, bool Autogen, bool ShowFrame, int Step_1, int Step_2, int Step_3,int Cols, int Rows,QString Color)
{
    this->theme = Theme;
    this->autogen = Autogen;
    this->showFrame = ShowFrame;

    this->step_1 = Step_1;
    this->step_2 = Step_2;
    this->step_3 = Step_3;

    this->cols = Cols;
    this->rows = Rows;

    this->color = Color;
}

setting::setting(){}

bool setting::get_theme()
{
    return this->theme;
}

bool setting::get_autogen()
{
    return this->autogen;
}

bool setting::get_showFrame()
{
    return  this->showFrame;
}

int setting::get_step_1()
{
    return this->step_1;
}

int setting::get_step_2()
{
    return this->step_2;
}

int setting::get_step_3()
{
    return  this->step_3;
}

int setting::get_cols()
{
    return this->cols;
}

int setting::get_rows()
{
    return  this->rows;
}

QString setting::get_color()
{
    return this->color;
}

setting setting::get_setting()
{
    return *this;
}
