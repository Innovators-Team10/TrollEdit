#include "text_item.h"
#include "block.h"

TextItem::TextItem(const QString &text, Block *parent)
    : QGraphicsTextItem(text, parent)
{
    setTextInteractionFlags(Qt::TextEditorInteraction);
//    myTextItem->setFont(QFont("Courier"));
    connect(this, SIGNAL(focusChanged(QFocusEvent*)), parent, SLOT(textFocusChanged(QFocusEvent*)));
    connect(document(), SIGNAL(contentsChanged()), parent, SLOT(textChanged()));
    connect(this, SIGNAL(enterPressed(int)), parent, SLOT(splitLine(int)));
    connect(this, SIGNAL(moveCursorLR(int)), parent, SLOT(moveCursorLR(int)));
    connect(this, SIGNAL(moveCursorUD(int)), parent, SLOT(moveCursorUD(int)));
}

void TextItem::setTextCursorPosition(int i) 
{
    if (i < 0)
        i = toPlainText().length();
    textCursor().setPosition(i);
    setFocus();
}

void TextItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // turn off dashed frame (which is present when edited) and do normal painting
    QStyleOptionGraphicsItem *opt = const_cast<QStyleOptionGraphicsItem*>(option);
    opt->state &= ~QStyle::State_Selected;
    opt->state &= ~QStyle::State_HasFocus;
    QGraphicsTextItem::paint(painter, opt, widget);
}

void TextItem::keyPressEvent(QKeyEvent *event) 
{
    int cursorPos = textCursor().position();
    QString oldText = toPlainText();

    if (event->key() == Qt::Key_Return) {
        emit enterPressed(cursorPos);
        return;
    }
    QGraphicsTextItem::keyPressEvent(event);

    if (oldText != toPlainText()) {
        emit textChanged(event);
    }
    switch(event->key()) {
    case Qt::Key_Up :
    case Qt::Key_Down :
        emit moveCursorUD(event->key());
        break;
    case Qt::Key_Left :
        if (cursorPos == 0)
            emit moveCursorLR(Qt::Key_Left);
        break;
    case Qt::Key_Right :
        if (cursorPos == oldText.length())
            emit moveCursorLR(Qt::Key_Right);
        break;
    }
}

void TextItem::focusInEvent(QFocusEvent *event)
{
    QGraphicsTextItem::focusInEvent(event);
    emit focusChanged(event);
}

void TextItem::focusOutEvent(QFocusEvent *event)
{
    QGraphicsTextItem::focusOutEvent(event);
    emit focusChanged(event);
}

