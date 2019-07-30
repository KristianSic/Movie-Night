#ifndef ASPECTRATIOPIXMAPLABEL_H
#define ASPECTRATIOPIXMAPLABEL_H

#include <QLabel>
#include <QPixmap>
#include <QResizeEvent>
#include <QMouseEvent>

class AspectRatioPixmapLabel : public QLabel
{
    Q_OBJECT
public:
    explicit AspectRatioPixmapLabel(QWidget *parent = 0);
    virtual int heightForWidth( int width ) const;
    virtual QSize sizeHint() const;
    QPixmap scaledPixmap() const;

    QPoint coords;

public slots:
    void setPixmap ( const QPixmap & );
    void resizeEvent(QResizeEvent *);

protected:
    void mousePressEvent(QMouseEvent* event);

signals:
    void clicked(QPoint& coords);
private:
    QPixmap pix;


};


#endif // ASPECTRATIOPIXMAPLABEL_H
