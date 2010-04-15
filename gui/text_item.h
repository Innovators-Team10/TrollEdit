#ifndef TEXT_ITEM_H
#define TEXT_ITEM_H

#include<QGraphicsTextItem>

class Block;

class TextItem : public QGraphicsTextItem
{
        Q_OBJECT

public:
    TextItem(const QString &text, Block *parent = 0, bool multiText = false);
    void setTextCursorPosition(int i);
    bool removeCharAt(int i);   // returns false if text is empty after removal
    void setFont(const QFont &font);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QRectF boundingRect() const;
    qreal margin;

protected:
    void keyPressEvent(QKeyEvent *event);
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);

signals:
    void focusChanged(QFocusEvent *event);
    void keyPressed(QKeyEvent *event);
    void moveCursorLR(int key);
    void moveCursorUD(int key, int from);
    void enterPressed(int cursorPos);
    void erasePressed(int key);
private:
    bool multiText;
};

#endif // TEXT_ITEM_H
