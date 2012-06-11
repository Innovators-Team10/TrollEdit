/**
 * close_button.h
 *  ---------------------------------------------------------------------------
 * Contains the declaration of class CloseButton and it's funtions and identifiers
 *
 */

#ifndef CLOSE_BUTTON_H
#define CLOSE_BUTTON_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QtGui>

class Block;
class BlockGroup;

class CloseButton : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    CloseButton(Block *parentBlock);

    enum { Type = UserType + 10 };
    void updatePos();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    int type() const {return Type;}

private:
    Block *myBlock;
    QPixmap close;
    friend class Block;
};

#endif // CLOSE_BUTTON_H
