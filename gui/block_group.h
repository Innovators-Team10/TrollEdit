#ifndef BLOCK_GROUP_H
#define BLOCK_GROUP_H

#include <QGraphicsRectItem>
#include <QSet>

class Block;
class DocumentScene;
class Analyzer;
class FoldButton;

class BlockGroup : public QObject, public QGraphicsRectItem
{
    Q_OBJECT


public:
    BlockGroup(QString text, Analyzer* analyzer, DocumentScene *scene);
    ~BlockGroup();

    enum { Type = UserType + 2 };

    // main properties
    int type() const { return Type; }
    Block *mainBlock() const {return root;}
//    QList<Block*> docBlocks() const;
    int getLastLine() const {return lastLine;}

    // block management
    Block *getBlockIn(int line) const;
    void setBlockIn(Block *block, int line);
    bool addFoldable(Block *block);
    void removeFoldable(Block *block);

    void selectBlock(Block *block);
    void deselect();
    Block *removeBlock(Block *block, bool deleteThis);
    Block* selectedBlock() const {return selected;}
    Block* blockAt(QPointF scenePos) const;

    // analysis
    void setAnalyzer(Analyzer *analyzer) {this->analyzer = analyzer;}
    bool analyzeAll(QString text);
    void reanalyze();
    bool reanalyze(Block* block, QPoint cursorPos);
    QString toText() const;
//    void addDocBlock(QPointF pos);

    // visualization
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    // helpers
    static QList<Block*> blocklist_cast(QList<QGraphicsItem*> list);

    static const QPointF OFFSET_IN, OFFSET_OUT, NO_OFFSET;
    int TAB_LENGTH;
    qreal CHAR_HEIGHT, CHAR_WIDTH;
    DocumentScene *docScene;    // my scene
    void updateAll(bool animate = true);

public slots:
    void keyPressed(QKeyEvent* event);
    void splitLine(Block *block, int cursorPos);
    void eraseChar(Block *block, int key);
    void moveFrom(Block *block, int key, int cursorPos);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {Q_UNUSED(event);}

private:

    void moveCursorUpDown(Block *start, bool moveUp, int from);
    void moveCursorLeftRight(Block *start, bool moveRight);

    void computeTextSize();

    // fields
    Analyzer *analyzer;         // my analyzer
    Block *root;                // main (root) block
    Block *selected;            // currently selected block
    QList<Block*> lineStarts;   // line starting blocks
    int lastLine;               // curent last line
    QSet<Block*> foldableBlocks;// foldable blocks, only 1 per line allowed

    qreal lastXPos;

    friend class DocumentScene;
};

#endif // BLOCK_GROUP_H
