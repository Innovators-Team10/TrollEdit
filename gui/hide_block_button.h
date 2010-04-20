#ifndef HIDEBLOCKBUTTON_H
#define HIDEBLOCKBUTTON_H

#include <QGraphicsPixmapItem>
#include <QtGui>
class Block;

class HideBlockButton : public QGraphicsPixmapItem
{
public:
    HideBlockButton(Block *parent);
    enum { Type = UserType + 10 };

    void updatePos();

protected:
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

    int type() const;

private:
    Block *myBlock;
    QPixmap plus;
    QPixmap minus;
    bool focus;
};

#endif // HIDEBLOCKBUTTON_H
