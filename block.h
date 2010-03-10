#ifndef BLOCK_H
#define BLOCK_H

#include <QGraphicsRectItem>
#include <QWidget>
#include <QtGui>
#include "hide_block_button.h"
#include "document_scene.h"

class Block : public QObject, public QGraphicsRectItem
{
    Q_OBJECT

public:
    Block(QGraphicsScene *parentScene = 0, Block *parentBlock = 0);

    enum { Type = UserType + 1 };
    int id;

    void setFolded(bool folded);
    bool isFolded();
    Block *parentBlock();

    QPainterPath shape() const;
    int type() const;
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QList<Block*> blocklist_cast(QList<QGraphicsItem*> list);

    void setChanged();

signals:
    void lostFocus(Block *block);

protected:
    void updateLayout();

    void focusOutEvent(QFocusEvent *event);
    QVariant itemChange(GraphicsItemChange change, const QVariant & value);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);

    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event);


private:
    bool folded;    // true when block is folded
    bool pressed;   // true while mouse is pressed
    bool changed;   // changed after last updateLayout() call

    HideBlockButton *hideButton;
    qreal originalWidth;
    static const int OFFS = 20;

    void childAdded(Block *newChild);
    void childRemoved(Block *oldChild);

// for testing
private slots:
    void writeZ();
    void writeToParent();
};

#endif // BLOCK_H
