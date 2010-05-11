#ifndef BLOCK_H
#define BLOCK_H

#include <QGraphicsRectItem>
#include <QtGui>
#include <QList>
#include <QPropertyAnimation>

class TreeElement;
class BlockGroup;
class FoldButton;
class TextItem;

class Block : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
    Q_PROPERTY(QRectF geometry READ geometry WRITE setGeometry)

public:
    Block(TreeElement *element, Block *parentBlock, BlockGroup *blockGroup = 0);
    ~Block();

    enum { Type = UserType + 1 };
    enum OffsetType {
        InnnerTopLeft = 0, InnerBottomRight = 1, Outer = 2, Drop = 3,
    };

    // methods to change hierarchy (blocks + AST)
    void setParentBlock (QGraphicsItem *parent);
    void stackBeforeBlock (const QGraphicsItem *sibling);
    Block *removeBlock(bool deleteThis);

    // block management methods
    Block *getFirstLeaf() const;
    Block *getLastLeaf() const;
    Block *getAncestorWhereFirst() const;
    Block *getAncestorWhereLast() const;
    Block *getNextSibling() const {return nextSib;}
    Block *getPrevSibling() const {return prevSib;}
    Block *getNext(bool textOnly = false) const;
    Block *getPrev(bool textOnly = false) const;
    Block *getFirstSelectableAncestor() const;

    bool hasMoreLines() const;
    int numberOfLines() const;

    void addBlockAt(Block *block, QPointF pos);
    QPair<Block*, bool> findClosestBlock(QPointF pos);

    // main properties
    int type() const {return Type;}
    TreeElement *getElement() const {return element;}
    Block *parentBlock() const {return parent;}
    QList<Block*> childBlocks() const;
    BlockGroup *blockGroup() const {return group;}
    int getLine() const {return line;}
    void setLine(int line) {this->line = line;}
    bool isFoldable() const;
    void setFolded(bool folded);
    bool isFolded() const {return folded;}
    bool isEdited() const;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    // textItem properties
    bool isTextBlock() const {return myTextItem != 0;}
    TextItem *textItem() const {return myTextItem;}
    Block *addTextCursorAt(QPointF pos);

    // geometry
    QPointF idealPos() const {return idealGeometry.topLeft();}
    QSizeF idealSize() const {return idealGeometry.size();}
    QRectF idealRect() const {return QRectF(QPointF(), idealGeometry.size());}
    QRectF geometry() const;
    void setGeometry(QRectF geometry);
    QRectF boundingRect() const; // currently same as QGraphicsRectItem::rect()
    QPointF mapIdealToAncestor(Block* ancestor, QPointF pos) const;    

    // visualization
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void updatePen();
    void highlight(QPair<QFont, QColor> format);
    void setShowing(bool newState);
    void offsetChildren(bool flag, QPointF pos = QPointF());

    virtual void updateBlock(bool doAnimation = true);
    void updateBlockAfter(bool doAnimation = true);
    void updateGeometryAfter(bool doAnimation = true);
    void animate();

public slots:
    void textFocusChanged(QFocusEvent* event);
    void textChanged();
signals:
    void visibilityChanged(bool flag);

protected:
    void updatePos(bool updateReal = false);
    void updateSize(bool updateReal = false);
    void updateGeometry(bool updateReal = false);
    void updateLine();
    void updateFoldButton();
    QPointF getOffset(OffsetType type) const;

    // event processing
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    void dropEvent(QGraphicsSceneDragDropEvent *event);
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event);

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

    bool folded;     // true when block is folded
    bool moveStarted;// true while block is moving
    bool edited;     // edited after last AST analysis
    bool showing;    // true when block border is painted, block has inner and outer offset
    bool moreSpace;  // true when dropping to this block's parent, block has outer offset
    bool hovered;    // true after hoverEnterEvent
    bool pointed;    // true if it is last hovered block
    int level;

    TreeElement *element;       // my AST element
    Block *parent;              // my parent
    BlockGroup *group;          // my block group
    TextItem *myTextItem;       // my text area (AST leafs only)
    int line;                   // my line
    QPropertyAnimation *animation; // assigned animation
    QRectF idealGeometry;       // desired geometry (position + size)

    Block *nextSib, *prevSib, *firstChild;    // links

    QHash<QString, QColor> format;

    FoldButton *foldButton;

    void createControls();
    void removeLinks();
    void assignHighlighting(TreeElement* elm);
    friend class BlockGroup;
};

#endif // BLOCK_H
