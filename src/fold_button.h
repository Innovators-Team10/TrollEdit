/**
 * fold_button.h
 *  ---------------------------------------------------------------------------
 * Contains the declaration of class FoldButton and it's funtions and identifiers
 *
 */

#ifndef FOLD_BUTTON_H
#define FOLD_BUTTON_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QtGui>

class Block;
class BlockGroup;

class FoldButton : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    FoldButton(Block *parentBlock);

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
    QString foldText; //! custom text showing when myBlock is folded
    QPixmap plus;
    QPixmap minus;
    friend class Block;
};

#endif // FOLD_BUTTON_H
