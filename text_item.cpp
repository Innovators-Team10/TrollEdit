#include "text_item.h"
#include "block.h"

TextItem::TextItem(const QString &text, Block *parent)
    : QGraphicsTextItem(text, parent)
{
    setTextInteractionFlags(Qt::TextEditorInteraction);
//    myTextItem->setFont(QFont("Courier"));
    connect(this, SIGNAL(focusChanged(QFocusEvent*)), parent, SLOT(textFocusChanged(QFocusEvent*)));
    connect(document(), SIGNAL(contentsChanged()), parent, SLOT(textChanged()));
    connect(this, SIGNAL(enterPressed(QString)), parent, SLOT(addNewLineAfterThis(QString)));
    connect(this, SIGNAL(moveCursorLR(int)), parent, SLOT(moveCursorLR(int)));
    connect(this, SIGNAL(moveCursorUD(int)), parent, SLOT(moveCursorUD(int)));
}

void TextItem::setTextCursorPosition(int i) {
    if (i < 0)
        i = toPlainText().length();
    textCursor().setPosition(i);
    setFocus();
}

void TextItem::keyPressEvent(QKeyEvent *event) {
    int cursorPos = textCursor().position();
    QString oldText = toPlainText();

    if (event->key() == Qt::Key_Return) {
        QString textAfter = oldText.right(oldText.length()-cursorPos);
        oldText.truncate(cursorPos);
        document()->setPlainText(oldText);
        emit enterPressed(textAfter);
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

