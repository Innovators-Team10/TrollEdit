#include "text_item.h"
#include "block.h"

TextItem::TextItem(const QString &text, Block *parent, bool multiText)
    : QGraphicsTextItem(text, parent)
{
    QFontMetricsF *fm = new QFontMetricsF(font());
    margin = (QGraphicsTextItem::boundingRect().width() - fm->width(toPlainText())) / 2;

    connect(this, SIGNAL(focusChanged(QFocusEvent*)), parent, SLOT(textFocusChanged(QFocusEvent*)));
    connect(document(), SIGNAL(contentsChanged()), parent, SLOT(textChanged()));
    connect(this, SIGNAL(keyPressed(QKeyEvent*)), parent, SLOT(keyPressed(QKeyEvent*)));
    connect(this, SIGNAL(enterPressed(int)), parent, SLOT(splitLine(int)));
    connect(this, SIGNAL(erasePressed(int)), parent, SLOT(eraseChar(int)));
    connect(this, SIGNAL(moveCursorLR(int)), parent, SLOT(moveCursorLR(int)));
    connect(this, SIGNAL(moveCursorUD(int, int)), parent, SLOT(moveCursorUD(int, int)));

    this->multiText = multiText;
    setTextInteractionFlags(Qt::TextEditable | Qt::TextSelectableByKeyboard);
    setAcceptedMouseButtons(Qt::NoButton);
}

void TextItem::setFont(const QFont &font)
{
    QGraphicsTextItem::setFont(font);
    QFontMetricsF *fm = new QFontMetricsF(font);
    margin = (QGraphicsTextItem::boundingRect().width() - fm->width(toPlainText())) / 2;
    // NOTE: margin == 4;
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

bool TextItem::removeCharAt(int i)
{
    QString text = toPlainText();
    if (text.length() == 0)
        i = 0;
    else if (i < 0)
        i = text.length() + i;
    text.remove(i,1);
    setPlainText(text);
    if (text.isEmpty()) {
        return false;
    }
    return true;
}

QRectF TextItem::boundingRect() const
{
    QRectF rect = QGraphicsTextItem::boundingRect();
    rect.adjust(margin, 0, -margin+1, 0);
    // NOTE: +1 pixel is needed to draw cursor at the end of item
    return rect;
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
    QString text = toPlainText();

    if (event->key() == Qt::Key_Return) {
        emit enterPressed(cursorPos);
        if (!multiText)
            return;
    }
    QGraphicsTextItem::keyPressEvent(event);

    int index;
    switch(event->key()) {
    case Qt::Key_Up :
        index = text.indexOf("\n");
        if (cursorPos <= index || index < 0) {
            emit moveCursorUD(event->key(), cursorPos);
        }
        break;
    case Qt::Key_Down :
        index = text.lastIndexOf("\n");
        if (cursorPos > index) {
            if (index < 0) index = 0;
            emit moveCursorUD(event->key(), cursorPos-index);
        }
        break;
    case Qt::Key_Left :
        if (cursorPos == 0) {
            emit moveCursorLR(Qt::Key_Left);
        }
        break;
    case Qt::Key_Right :
        if (cursorPos == text.length()) {
            emit moveCursorLR(Qt::Key_Right);
        }
        break;
    case Qt::Key_Backspace :
        if (cursorPos == 0) {
            emit erasePressed(Qt::Key_Backspace);
        }
        break;
    case Qt::Key_Delete :
        if (cursorPos == text.length()) {
            emit erasePressed(Qt::Key_Delete);
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
