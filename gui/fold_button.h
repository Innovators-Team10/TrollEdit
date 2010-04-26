#ifndef FOLD_BUTTON_H
#define FOLD_BUTTON_H

#include <QGraphicsPixmapItem>
#include <QtGui>

class Block;
class BlockGroup;

class FoldButton : public QGraphicsPixmapItem
{
public:
    FoldButton(Block *parentBlock, BlockGroup *parentGruoup);

    enum { Type = UserType + 10 };
    void updatePos();

protected:
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

    int type() const {return Type;}

private:
    Block *myBlock;
    QPixmap plus;
    QPixmap minus;
};

#endif // FOLD_BUTTON_H
