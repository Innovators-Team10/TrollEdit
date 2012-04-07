/**
 * doc_block.h
 *  ---------------------------------------------------------------------------
 * Contains the declaration of class DocBlock and it's funtions and identifiers
 *
 */

#ifndef DOC_BLOCK_H
#define DOC_BLOCK_H

#include <QObject>

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
    DocBlock(QString text, TreeElement *element, Block* parentBlock, BlockGroup *parentgroup = 0);
    ~DocBlock();

    enum { Type = UserType + 3 };
    int type() const {return Type;}

    void setContent(QString text);
    void addText(QString text = "");
    void addImage(const QImage &image, QString imagePath);
    void addLink(QUrl url);
    void addWebLink(QUrl url);
    void addArrowTo(Block *end);
    Block *targetBlock() const;

    QString convertToText() const;
    enum DocType  //! type of docblock
    {
        Generic = 0, Text = 1, Image = 2, Link = 3, WebLink = 4,
    };
    DocType getDocType() const {return docType;}

    // reimplemented from block
    Block *addTextCursorAt(QPointF pos);
    bool isFoldable() const;
    void setFolded(bool fold);
    void setLocked(bool lock);
    Block *removeBlock(bool deleteThis);
    QColor getHoverColor() const;

    // udaters
    void updateBlockInMap(DocBlock* element);
    // reimplemented from block
    void updateBlock(bool doAnimation = true);
    void updateGeometryAfter(bool doAnimation = true);

public slots:
    void textChanged();             //! reimplemented from block
    void arrowDestroyed();

protected:
    void updatePos(bool updateReal = false);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

private:
    QString path;           //! full path for Image and Link types
    Arrow *arrow;
    DocType docType;        //! type of this docblock
    QTextDocument *backup;  //! backup data used for folding
    bool locked;

    friend class BlockGroup;
};

#endif // DOC_BLOCK_H
