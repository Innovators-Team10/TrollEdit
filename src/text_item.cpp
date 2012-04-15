/** 
* @file text_item.cpp
* @author Team 04 Ufopak + Team 10 Innovators
* @version 
* 
* @section DESCRIPTION
* Contains the defintion of class TextItem.
*/

#include "text_item.h"
#include "block.h"
#include "block_group.h"

TextItem::TextItem(const QString &text, Block *parentBlock, bool multiText, bool paired)
    : QGraphicsTextItem(text, parentBlock)
{
    myBlock = parentBlock;

    this->multiText = multiText;
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsFocusable, false);
    setTextInteractionFlags(Qt::TextEditable | Qt::TextSelectableByKeyboard);
    // forbid all mouse and drag-n-drop interactions
    setAcceptedMouseButtons(Qt::NoButton);
    setAcceptDrops(false);

    QFontMetricsF *fm = new QFontMetricsF(font());
    MARGIN = (QGraphicsTextItem::boundingRect().width() - fm->width(toPlainText())) / 2;

    if (paired) //! emit focusChanged() only for paired blocks
        connect(this, SIGNAL(focusChanged(QFocusEvent*)), myBlock, SLOT(textFocusChanged(QFocusEvent*)));

    connect(document(), SIGNAL(contentsChanged()), myBlock, SLOT(textChanged()));
    
    connect(this, SIGNAL(keyPressed(QKeyEvent*)), myBlock->blockGroup(), SLOT(keyTyped(QKeyEvent*)));
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

bool TextItem::setTextCursorPos(int i)
{
    int length = toPlainText().length();

    if (i < 0)
        i = length + i + 1;

    if (i < 0 || i > length)
        return false;

    QTextCursor cursor = textCursor();
    cursor.setPosition(i);
    setTextCursor(cursor);
    setFocus();

    return true;
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

    if (text.isEmpty()) return false;

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
    opt->state &= ~QStyle::State_HasFocus;
    QGraphicsTextItem::paint(painter, opt, widget);
}

void TextItem::keyPressEvent(QKeyEvent *event) 
{
//    myBlock->blockGroup()->keyTyped(event);
    emit keyPressed(event);

    int cursorPos = textCursor().position();

//    QPointF cursorPoint = cursor().;
    QString text = toPlainText();

    if (event->key() == Qt::Key_Return && !multiText)
    {
        if (cursorPos == text.length())
            cursorPos = -1;

        event->accept();
//        myBlock->blockGroup()->splitLine(myBlock, cursorPos);
        emit enterPressed(myBlock, cursorPos);

        return;
    }

    if ((event->modifiers() & Qt::ControlModifier) == Qt::ControlModifier)
        event->ignore();
    else QGraphicsTextItem::keyPressEvent(event);

    int index;

    switch(event->key())
    {
    case Qt::Key_Up :
        index = text.indexOf("\n");
        if (cursorPos <= index || index < 0)
        {
//            myBlock->blockGroup()->moveFrom(myBlock, event->key(), cursorPos);
            emit moveCursor(myBlock, event->key(), cursorPos);
        }

        break;
    case Qt::Key_Down :
        index = text.lastIndexOf("\n");

        if (cursorPos > index)
        {
            if (index < 0) index = 0;
//            myBlock->blockGroup()->moveFrom(myBlock, event->key(), cursorPos - index);
            emit moveCursor(myBlock, event->key(), cursorPos - index);
        }

        break;
    case Qt::Key_Left :
        if (cursorPos == 0)
        {
//            myBlock->blockGroup()->moveFrom(myBlock, Qt::Key_Left, 0);
            emit moveCursor(myBlock, Qt::Key_Left);
        }

        break;
    case Qt::Key_Right :
        if (cursorPos == text.length())
        {
//            myBlock->blockGroup()->moveFrom(myBlock, Qt::Key_Right, 0);
            emit moveCursor(myBlock, Qt::Key_Right);
        }

        break;
    case Qt::Key_Backspace :
        if (cursorPos == 0)
        {
//            myBlock->blockGroup()->eraseChar(myBlock, Qt::Key_Backspace);
            emit erasePressed(myBlock, Qt::Key_Backspace);
        }

        break;
    case Qt::Key_Delete :
        if (cursorPos == text.length())
        {
//            myBlock->blockGroup()->eraseChar(myBlock, Qt::Key_Delete);
            emit erasePressed(myBlock, Qt::Key_Delete);
        }

        break;
    case Qt::Key_Home :
    case Qt::Key_End :
//        myBlock->blockGroup()->moveFrom(myBlock, event->key(), 0);
        emit moveCursor(myBlock, event->key());

        break;
    default :
        event->ignore();
    }
}

void TextItem::focusInEvent(QFocusEvent *event)
{
    QGraphicsTextItem::focusInEvent(event);
//    myBlock->textFocusChanged(event);
    emit focusChanged(event);
}

void TextItem::focusOutEvent(QFocusEvent *event)
{
    QGraphicsTextItem::focusOutEvent(event);
//    myBlock->textFocusChanged(event);
    emit focusChanged(event);
}

void TextItem::adaptToFloating()
{
//    if(myBlock == 0){
//        qDebug() << "myBlock null";
//        return;
//    }
//    disconnect(this, 0, myBlock->blockGroup(), 0); //BUG!! zisti presne co to robi
}
