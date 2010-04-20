#ifndef DOC_BLOCK_H
#define DOC_BLOCK_H

#include "block.h"
#include "arrow.h"
#include "text_item.h"
#include "../analysis/tree_element.h"
#include "../widget/document_scene.h"
#include <QTextBrowser>
class TreeElement;
class DocumentScene;
class HideBlockButton;
class TextItem;
class Arrow;

class DocBlock : public Block
{
    Q_OBJECT
public:
    DocBlock(TreeElement *element, Block *parentBlock, QGraphicsScene *parentScene = 0);
    void addImage(const QImage &image);
    void addFile(QUrl url);
    void addArrow(DocBlock *start,Block *end, QGraphicsScene *parentScene);
    QImage image;
    Arrow *arrow;

    enum { Type = UserType + 2 };
protected:
    QTextBrowser file;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

};

#endif // DOC_BLOCK_H
