#include "text_item.h"
#include "block.h"

TextItem::TextItem(const QString &text, Block *parent, bool multiText)
    : QGraphicsTextItem(text, parent)
{
    setTextInteractionFlags(Qt::TextEditorInteraction);
    setFont(QFont("Courier"));
    connect(this, SIGNAL(focusChanged(QFocusEvent*)), parent, SLOT(textFocusChanged(QFocusEvent*)));
    connect(document(), SIGNAL(contentsChanged()), parent, SLOT(textChanged()));
    connect(this, SIGNAL(keyPressed(QKeyEvent*)), parent, SLOT(keyPressed(QKeyEvent*)));
    connect(this, SIGNAL(enterPressed(int)), parent, SLOT(splitLine(int)));
    connect(this, SIGNAL(moveCursorLR(int)), parent, SLOT(moveCursorLR(int)));
    connect(this, SIGNAL(moveCursorUD(int, int)), parent, SLOT(moveCursorUD(int, int)));

    this->multiText = multiText;
}

void TextItem::setTextCursorPosition(int i) 
{
    setFocus();
    int length = toPlainText().length();
    if (length == 0)
        i = 0;
    else if (i < 0)
        i = length + i + 1;
    QTextCursor cursor = textCursor();
    cursor.setPosition(i);
    setTextCursor(cursor);  // important - textCursor() returns only copy!
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
    emit keyPressed(event);
    int cursorPos = textCursor().position();
    QString oldText = toPlainText();

    if (event->key() == Qt::Key_Return) {
        emit enterPressed(cursorPos);
        if (!multiText)
            return;
    }
    QGraphicsTextItem::keyPressEvent(event);

    int index;
    switch(event->key()) {
    case Qt::Key_Up :
        index = oldText.indexOf("\n");
        if (cursorPos <= index || index < 0) {
            emit moveCursorUD(event->key(), cursorPos);
//            return;
        }
        break;
    case Qt::Key_Down :
        index = oldText.lastIndexOf("\n");
        if (cursorPos > index) {
            if (index < 0) index = 0;
            emit moveCursorUD(event->key(), cursorPos-index);
//            return;
        }
        break;
    case Qt::Key_Left :
        if (cursorPos == 0) {
            emit moveCursorLR(Qt::Key_Left);
//            return;
        }
        break;
    case Qt::Key_Right :
        if (cursorPos == oldText.length()) {
            emit moveCursorLR(Qt::Key_Right);
//            return;
        }
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

