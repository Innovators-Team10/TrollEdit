#include "text_item.h"
#include "block.h"
#include "block_group.h"

TextItem::TextItem(const QString &text, Block *parentBlock, bool multiText)
    : QGraphicsTextItem(text, parentBlock)
{
    myBlock = parentBlock;

    this->multiText = multiText;
    setTextInteractionFlags(Qt::TextEditable | Qt::TextSelectableByKeyboard);
    setAcceptedMouseButtons(Qt::NoButton);

    QFontMetricsF *fm = new QFontMetricsF(font());
    MARGIN = (QGraphicsTextItem::boundingRect().width() - fm->width(toPlainText())) / 2;

    connect(this, SIGNAL(focusChanged(QFocusEvent*)), myBlock, SLOT(textFocusChanged(QFocusEvent*)));
    connect(document(), SIGNAL(contentsChanged()), myBlock, SLOT(textChanged()));
    
    connect(this, SIGNAL(keyPressed(QKeyEvent*)), myBlock->blockGroup(), SLOT(keyPressed(QKeyEvent*)));
    connect(this, SIGNAL(enterPressed(Block*, int)), myBlock->blockGroup(), SLOT(splitLine(Block*, int)));
    connect(this, SIGNAL(erasePressed(Block*, int)), myBlock->blockGroup(), SLOT(eraseChar(Block*, int)));
    connect(this, SIGNAL(moveCursor(Block*, int, int)), myBlock->blockGroup(),
            SLOT(moveFrom(Block*, int, int)));

    setPos(QPointF());
}

void TextItem::setFont(const QFont &font)
{
    QGraphicsTextItem::setFont(font);
    QFontMetricsF *fm = new QFontMetricsF(font);
    MARGIN = (QGraphicsTextItem::boundingRect().width() - fm->width(toPlainText())) / 2;
}

void TextItem::setPos(const QPointF &pos)
{
    QGraphicsTextItem::setPos(pos - QPointF(MARGIN, 0));
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
    rect.adjust(MARGIN, 0, -MARGIN+1, 0);
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

//    QPointF cursorPoint = cursor().;
    QString text = toPlainText();

    if (event->key() == Qt::Key_Return && !multiText) {
        if (cursorPos == text.length())
            cursorPos = -1;
        emit enterPressed(myBlock, cursorPos);
        return;
    }
    QGraphicsTextItem::keyPressEvent(event);

    int index;
    switch(event->key()) {
    case Qt::Key_Up :
        index = text.indexOf("\n");
        if (cursorPos <= index || index < 0) {
            emit moveCursor(myBlock, event->key(), cursorPos);
        }
        break;
    case Qt::Key_Down :
        index = text.lastIndexOf("\n");
        if (cursorPos > index) {
            if (index < 0) index = 0;
            emit moveCursor(myBlock, event->key(), cursorPos - index);
        }
        break;
    case Qt::Key_Left :
        if (cursorPos == 0) {
            emit moveCursor(myBlock, Qt::Key_Left);
        }
        break;
    case Qt::Key_Right :
        if (cursorPos == text.length()) {
            emit moveCursor(myBlock, Qt::Key_Right);
        }
        break;
    case Qt::Key_Backspace :
        if (cursorPos == 0) {
            emit erasePressed(myBlock, Qt::Key_Backspace);
        }
        break;
    case Qt::Key_Delete :
        if (cursorPos == text.length()) {
            emit erasePressed(myBlock, Qt::Key_Delete);
        }
        event->ignore();
        break;
    default :
        event->ignore();
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

void TextItem::adaptToFloating()
{
    disconnect(this, 0, myBlock->blockGroup(), 0);
}
