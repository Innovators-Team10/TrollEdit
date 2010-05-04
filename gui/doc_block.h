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
    DocBlock(QPointF pos, BlockGroup *parentgroup = 0);
    DocBlock(TreeElement *element, Block* parentBlock = 0, BlockGroup *parentgroup = 0);
    ~DocBlock();

    enum { Type = UserType + 3 };
    void addText(QString text);
    void addImage(const QImage &image);
    void addFile(QUrl url);

protected:
    void addArrowTo(Block *end);
    void updateBlock(bool doAnimation = true);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QImage image;
    Arrow *arrow;
    QString type;

    friend class BlockGroup;
};

#endif // DOC_BLOCK_H
