#ifndef TEXT_ITEM_H
#define TEXT_ITEM_H

#include<QGraphicsTextItem>

class Block;

class TextItem : public QGraphicsTextItem
{
        Q_OBJECT

public:
    TextItem(const QString &text, Block *parent = 0);
    void setTextCursorPosition(int i);
protected:
    void keyPressEvent(QKeyEvent *event);
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);
signals:
    void textChanged(QKeyEvent *event);
    void focusChanged(QFocusEvent *event);
    void moveCursorLR(int key);
    void moveCursorUD(int key);
    void enterPressed(QString text);
};

#endif // TEXT_ITEM_H
