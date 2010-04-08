#ifndef BLOCK_H
#define BLOCK_H

#include <QGraphicsRectItem>
#include <QtGui>

class TreeElement;
class DocumentScene;
class HideBlockButton;
class TextItem;

class Block : public QObject, public QGraphicsRectItem
{
    Q_OBJECT

public:
    Block(TreeElement *element, Block *parentBlock, QGraphicsScene *parentScene = 0);
    ~Block();

    enum { Type = UserType + 1 };

    // overriden methods to provide AST consistency
    void setParentItem (QGraphicsItem *parent);
    void stackBefore (const QGraphicsItem *sibling);
    QList<Block*> removeBlock();

    // block management methods
    Block *getFirstLeaf() const;
    Block *getLastLeaf() const;
    Block *getAncestorWhereFirst() const;
    Block *getNextSibling() const;
    Block *getNext(bool textOnly = false) const;
    Block *getPrev(bool textOnly = false) const;

    // property getters and setters
    int type() const;
    Block *parentBlock() const;
    QList<Block*> childBlocks() const;
    TextItem *textItem() const;
    TreeElement *getElement() const;
    static int getLastLine(){return lastLine;}

    void setFolded(bool folded);
    bool isFolded() const;
    bool isTextBlock() const;
    int length() const;
    bool hasMoreLines() const;
    int numberOfLines() const;
//    void addSpaces(int count);
    int getSpaces() const;
    int getAbsoluteSpaces() const;
    void setLine(int newLine);

    // visualization
    QPainterPath shape() const;
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QList<Block*> blocklist_cast(QList<QGraphicsItem*> list) const;


    void updateLayout();
    void updateAfter(bool updateThis = false);
    void updateLineStarts();
    void updateXPosInLine(int lineNo);

    static QMap<int, Block*> lineStarts;// move to private..
    static int OFFSH, OFFSV;// temp

signals:
    void lostFocus(Block *block);

public slots:
    void textFocusChanged(QFocusEvent* event);
    void textChanged();
    void keyPressed(QKeyEvent* event);
    void splitLine(int cursorPos = -1);
    void eraseChar(int key);
    void moveCursorLR(int key);
    void moveCursorUD(int key, int from);

protected:
    QPointF computeNextSiblingPos() const;
    int computeNextSiblingLine() const;
    Block* findNextChildAt(QPointF pos) const;
    QLineF getInsertLineAt(const Block* nextBlock) const;

    // event processing
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);

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

    static int lastLine;

    bool folded;    // true when block is folded
    bool pressed;   // true while mouse is pressed
    bool edited;    // edited after last AST analysis

    DocumentScene *docScene;    // my scene
    Block *parent;              // my parent
    TreeElement *element;       // my AST element
    TextItem *myTextItem;       // my text area (AST leafs only)
    int line;                   // my line (global index)
//    int spaces;                 // number of spaces visible before this block

    Block *nextSib, *prevSib, *firstChild;    // used to create links

    // graphic elements
    HideBlockButton *hideButton;

    void createControls();//todo
    void removeLinks();


    Block *futureParent;
    Block *futureSibling;   // used for block insertion
};

#endif // BLOCK_H
