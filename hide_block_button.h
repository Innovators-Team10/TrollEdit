#ifndef HIDEBLOCKBUTTON_H
#define HIDEBLOCKBUTTON_H

#include <QGraphicsPixmapItem>
#include <QtGui>

class Block;

class HideBlockButton : public QGraphicsPixmapItem
{
public:
    HideBlockButton(QGraphicsItem *parent = 0);
    enum { Type = UserType + 10 };

protected:
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    int type() const;

private:
//    Block *myBlock;
 //   bool firstTimeActivated;
};

#endif // HIDEBLOCKBUTTON_H
