/**
 * block.h
 *  ---------------------------------------------------------------------------
 * Contains the declaration of class Block and it's funtions and identifiers
 *
 */

#ifndef BLOCK_H
#define BLOCK_H

#include <QObject>
#include <QGraphicsRectItem>
#include <QtGui>
#include <QList>
#include <QPropertyAnimation>

class TreeElement;
class BlockGroup;
class FoldButton;
class TextItem;
class DocBlock;

class Block : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
    Q_PROPERTY(QRectF geometry READ geometry WRITE setGeometry)

public:
    Block(TreeElement *element, Block *parentBlock, BlockGroup *blockGroup = 0);
    ~Block();

    enum { Type = UserType + 1 };
    enum OffsetType
    {
        InnnerTopLeft = 0, InnerBottomRight = 1, Outer = 2,
    };

    // methods to change hierarchy (blocks + AST)
    void setParentBlock(Block *newParent, Block *nextSibling = 0);
    virtual Block *removeBlock(bool deleteThis);

    // block management methods
    Block *getFirstLeaf() const;
    Block *getLastLeaf() const;
    Block *getFirstChild() const {return firstChild;}
    Block *getAncestorWhereFirst() const;
    Block *getAncestorWhereLast() const;
    Block *getNextSibling() const {return nextSib;}
    Block *getPrevSibling() const {return prevSib;}
    Block *getNext(bool textOnly = false) const;
    Block *getPrev(bool textOnly = false) const;
    Block *getFirstSelectableAncestor() const;

    bool hasMoreLines() const;
    int numberOfLines() const;

    void addBlockInLine(Block *block, QPointF pos);
    int getLineAfter(QPointF pos) const;
    void addBlockBeforeLine(Block *block, int line);
    QPair<Block*, bool> findClosestChild(QPointF pos) const;
    QPair<Block*, bool> findClosestLeaf(QPointF pos) const;

    // main properties
    virtual int type() const {return Type;}
    TreeElement *getElement() const {return element;}
    Block *parentBlock() const {return parent;}
    QList<Block*> childBlocks() const;
    BlockGroup *blockGroup() const {return group;}
    int getLine() const {return line;}
    void setLine(int line) {this->line = line;}
    virtual bool isFoldable() const;
    virtual void setFolded(bool folded);
    bool isFolded() const {return folded;}
    bool isEdited() const;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    // textItem properties
    bool isTextBlock() const {return myTextItem != 0;}
    TextItem *textItem() const {return myTextItem;}
    virtual Block *addTextCursorAt(QPointF pos);

    // geometry
    QPointF idealPos() const {return idealGeometry.topLeft();}
    QSizeF idealSize() const {return idealGeometry.size();}
    QRectF idealRect() const {return QRectF(QPointF(), idealGeometry.size());}
    QRectF geometry() const;
    void setGeometry(QRectF geometry);
    QRectF boundingRect() const; //! currently same as QGraphicsRectItem::rect()
    QPointF mapIdealToAncestor(Block* ancestor, QPointF pos) const;    

    // visualization
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void highlight(QPair<QFont, QColor> format);
    void setShowing(bool newState, Block *until = 0);
    virtual QColor getHoverColor() const;
    bool isOverlapPossible() const;
    void setRepaintNeeded() {repaintNeeded = true;}
    void setYellow(bool flag) {isSearchResult = flag;}

    // updaters
    virtual void updateBlock(bool doAnimation = true);
    virtual void updateGeometryAfter(bool doAnimation = true);
    void animate();

public slots:
    void textFocusChanged(QFocusEvent* event);
    virtual void textChanged();
    void acceptHover(); //! called by hover timer

signals:
    void visibilityChanged(bool flag);

protected:
    virtual void updatePos(bool updateReal = false);
    void updateSize(bool updateReal = false);
    void updateGeometry(bool updateReal = false);
    void updateLine();
    void updateFoldButton();
    QPointF getOffset(OffsetType type) const;

    // event processing
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    void startDrag(QWidget *widget);
    void dropEvent(QGraphicsSceneDragDropEvent *event);
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event);

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

    bool folded;     //! true when block is folded
    bool moveStarted;//! true while block is moving
    bool edited;     //! edited after last AST analysis
    bool showing;    //! true when block border is painted, block has inner and outer offset
    int level;       //! level of showing
    bool moreSpace;  //! true when dropping to this block's parent, block has outer offset
    bool pointed;    //! true if it is last hovered block
    bool repaintNeeded;
    bool isSearchResult;

    TreeElement *element;       //! my AST element
    Block *parent;              //! my parent
    BlockGroup *group;          //! my block group
    TextItem *myTextItem;       //! my text area (AST leafs only)
    int line;                   //! my line
    QPropertyAnimation *animation; //! assigned animation
    QRectF idealGeometry;       //! desired geometry (position + size)

    Block *nextSib, *prevSib, *firstChild;    //! links

    FoldButton *foldButton;
    QTimer *timer;              //! hover timer

    QPair<QFont, QColor> highlightFormat; //! format of my text (if any)

    QPointF startDragPos; //! used to determine drag start

    void removeLinks();
    void assignHighlighting(TreeElement* el);
    friend class BlockGroup;
};

#endif // BLOCK_H
