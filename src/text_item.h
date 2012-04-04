/**
 * text_item.h
 *  ---------------------------------------------------------------------------
 * Contains the declaration of class TextItem and it's funtions and identifiers
 *
 */
#ifndef TEXT_ITEM_H
#define TEXT_ITEM_H

#include <QObject>
#include <QGraphicsTextItem>

class Block;

class TextItem : public QGraphicsTextItem
{
        Q_OBJECT

public:
    TextItem(const QString &text, Block *parentBlock, bool multiText = false, bool paired = false);
    enum { Type = UserType + 9 };
    int type() const {return Type;}

    bool setTextCursorPos(int i);
    bool removeCharAt(int i);   //! returns false if text is empty after removal

    void setPos(const QPointF &pos);
    void setFont(const QFont &font);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;

    void adaptToFloating();

    qreal MARGIN;   //! distance between QGraphicsTextItem bounding rect and actial text start

protected:
    void keyPressEvent(QKeyEvent *event);
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);

signals:
    void focusChanged(QFocusEvent *event);
    void keyPressed(QKeyEvent *event);
    void moveCursor(Block *, int key, int cursorPos = 0);
    void enterPressed(Block *, int cursorPos);
    void erasePressed(Block *, int key);

private:
    Block *myBlock;
    bool multiText;
};

#endif // TEXT_ITEM_H
