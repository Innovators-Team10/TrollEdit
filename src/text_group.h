/**
 * block_group.h
 *  ---------------------------------------------------------------------------
 * Contains the declaration of class TextGroup and it's funtions and identifiers
 *
 */

#ifndef TEXT_GROUP_H
#define TEXT_GROUP_H

#include <QObject>
#include <QGraphicsTextItem>
#include <QGraphicsSceneMouseEvent>

class DocumentScene;
class BlockGroup;

class TextGroup : public QGraphicsTextItem
{

public:
    TextGroup(BlockGroup *block, DocumentScene *scene);
    ~TextGroup();

    DocumentScene *scene;
    BlockGroup *block;
    QGraphicsRectItem *rc;
    void copy();
    void paste();
    void cut();
    void deleteFunction();
    void selectAll();
    void undo();
    void redo();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

signals:

public slots:

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:

};

#endif // TEXT_GROUP_H
