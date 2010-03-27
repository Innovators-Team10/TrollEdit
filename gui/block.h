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
    int id;

    // overriden methods to provide AST consistency
    void setParentItem (QGraphicsItem *parent);
    void stackBefore (const QGraphicsItem *sibling);
    Block *getParentBlock() const;

    void setFolded(bool folded);
    bool isFolded() const;
    bool isTextBlock() const;
    int length() const;
    Block *getFirstLeaf() const;
    Block *getNext(bool textOnly = false) const;
    Block *getPrev(bool textOnly = false) const;
    int numberOfLines() const;
    void setLine(int newLine);

    QList<Block*> childBlocks() const;
    TextItem *textItem() const;

    QPainterPath shape() const;
    int type() const;
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QList<Block*> blocklist_cast(QList<QGraphicsItem*> list) const;

    void setChanged();

    static int getLastLine(){return lastLine;}

signals:
    void lostFocus(Block *block);

public slots:
    void textFocusChanged(QFocusEvent* event);
    void textChanged();
    void keyPressed(QKeyEvent* event);
    void splitLine(int cursorPos = -1);
    void moveCursorLR(int key);
    void moveCursorUD(int key, int from);

protected:
    void updateLayout();
    void updatePos();
    void updateXPosInLine();

    QPointF computeNextSiblingPos() const;
    int computeNextSiblingLine() const;
    Block* findNextChildAt(QPointF pos) const;
    QLineF getInsertLineAt(const Block* nextBlock) const;

    void focusInEvent(QFocusEvent *event);
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
    static int OFFS;
    static int lastLine;

    bool folded;    // true when block is folded
    bool pressed;   // true while mouse is pressed
    bool edited;    // edited after last AST analysis
    bool changed;   // changed after last updateLayout() call

    DocumentScene *docScene;    // my scene
    Block *parent;              // my parent
    TreeElement *element;       // my AST element
    TextItem *myTextItem;       // my text area (AST leafs only)
    int line;                   // my line (global index)

    Block *nextSib, *prevSib, *firstChild;    // used to create links

    // graphic elements
    HideBlockButton *hideButton;

    void createControls();//todo
    void removeLinks();


    Block *futureParent;
    Block *futureSibling;   // used for block insertion
};

#endif // BLOCK_H
