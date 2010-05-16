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
    DocBlock(TreeElement *element, Block* parentBlock, BlockGroup *parentgroup = 0);
    ~DocBlock();

    enum { Type = UserType + 3 };
    void addText(QString text = "");
    void addImage(const QImage &image);
    void addLink(QUrl url);

    QString convertToText() const;
    enum DocType {  // type of docblock
        Generic = 0, Text = 1, Image = 2, Link = 3,
    };
    DocType getDocType() const {return type;}

    bool isFoldable() const;        // reimplemented from block
    void setFolded(bool fold);      // reimplemented from block

public slots:
    void textChanged();             // reimplemented from block

protected:
    void addArrowTo(Block *end);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void updateBlock(bool doAnimation = true);              // reimplemented from block
    void updateGeometryAfter(bool doAnimation = true);      // reimplemented from block
    void setDefaultPos();

private:
    QImage image;
    Arrow *arrow;
    DocType type;
    QTextDocument *backup;

    friend class BlockGroup;
};

#endif // DOC_BLOCK_H
