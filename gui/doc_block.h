#ifndef DOC_BLOCK_H
#define DOC_BLOCK_H

#include "block.h"

class TreeElement;
class DocumentScene;
class FoldButton;
class TextItem;
class Arrow;

class DocBlock : public Block
{
    Q_OBJECT
public:
    DocBlock(QString text, QPointF pos, Block *relatedBlock,  BlockGroup *parentgroup = 0);
    ~DocBlock();
    void addImage(const QImage &image);
    void addFile(QUrl url);
    void addArrow(DocBlock *start,Block *end, QGraphicsScene *parentScene);
    QImage image;
    Arrow *arrow;

    enum { Type = UserType + 3 };
protected:
    QTextBrowser file;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

};

#endif // DOC_BLOCK_H
