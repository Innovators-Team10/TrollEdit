/** 
* @file text_group.cpp
* @author Team 04 Ufopak + Team 10 Innovators
* @version 
* 
* @section DESCRIPTION
* Contains the defintion of class TextGroup.
*/

#include "text_group.h"
#include "document_scene.h"
#include "block_group.h"
#include "main_window.h"
#include "tree_element.h"
#include "block.h"
#include "text_item.h"
#include <QTextItem>

TextGroup::TextGroup(BlockGroup *block, DocumentScene *scene)
    : QGraphicsTextItem(0, scene)
{
    this->block = block;
    this->scene = scene;

//    this->setPlainText(block->toText());
    this->setPos(block->pos().x(),block->pos().y());
    this->setScale(block->scale());
    this->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable | QGraphicsItem::ItemIsMovable);
    this->setTextInteractionFlags(Qt::TextEditorInteraction);

    Block *temp = new Block(new TreeElement("temp"), 0, block);
    QFont *f = new QFont(temp->textItem()->font());
    this->setFont(*f);

    QPen pen = QPen(Qt::DashLine);
    pen.setColor(Qt::lightGray);
    rc = scene->addRect(block->boundingRect(),pen,Qt::NoBrush);
    rc->setFlags(QGraphicsItem::ItemIsMovable );
    this->setParentItem(rc);
    this->setFlag(QGraphicsItem::ItemIsMovable, false);
}

TextGroup::~TextGroup()
{
    this->rc = 0;
    this->block=0;
    this->scene=0;
}

void TextGroup::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton){
        if ((event->modifiers() & Qt::AltModifier) == Qt::AltModifier)
        {
            block->changeMode(this->scene->getWindow()->getActionList());
            event->accept();
        }
        else
        {
//            QGraphicsTextItem::mousePressEvent(event);
        }
    }
    QGraphicsTextItem::mousePressEvent(event);
}

void TextGroup::copy()
{
    QKeyEvent *event = new QKeyEvent(QEvent::KeyPress,Qt::CTRL + Qt::Key_C, Qt::NoModifier);
    keyPressEvent(event);
}

void TextGroup::paste()
{
    QKeyEvent *event = new QKeyEvent(QEvent::KeyPress,Qt::CTRL + Qt::Key_V, Qt::NoModifier);
    keyPressEvent(event);
}

void TextGroup::cut()
{
    QKeyEvent *event = new QKeyEvent(QEvent::KeyPress,Qt::CTRL + Qt::Key_X, Qt::NoModifier);
    keyPressEvent(event);
}

void TextGroup::deleteFunction()
{
    QKeyEvent *event = new QKeyEvent(QEvent::KeyPress,Qt::Key_Delete, Qt::NoModifier);
    keyPressEvent(event);
}

void TextGroup::selectAll()
{
    QKeyEvent *event = new QKeyEvent(QEvent::KeyPress,Qt::CTRL + Qt::Key_A, Qt::NoModifier);
    keyPressEvent(event);
}

void TextGroup::undo()
{
    QKeyEvent *event = new QKeyEvent(QEvent::KeyPress,Qt::CTRL + Qt::Key_Z, Qt::NoModifier);
    keyPressEvent(event);
}

void TextGroup::redo()
{
    QKeyEvent *event = new QKeyEvent(QEvent::KeyPress,Qt::CTRL + Qt::Key_Y, Qt::NoModifier);
    keyPressEvent(event);
}

void TextGroup::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    this->rc->setRect(this->boundingRect().adjusted(-10,-10,+10,+10));
    if( (this->hasFocus() || this->isUnderMouse()) || (rc->hasFocus() || rc->isUnderMouse()) ){
        QPen pen = QPen(Qt::DashLine);
        pen.setColor(Qt::black);
        painter->setPen(pen);
        painter->drawRect(boundingRect().adjusted(-10,-10,+10,+10));

        QFont font = painter->font() ;
        font.setPointSize ( 8 );
        font.setWeight(QFont::DemiBold);
        painter->setFont(font);
        QStringList list = block->getFilePath().split(QString(QDir::separator()));
        QPoint new_point = QPoint(widget->pos().x() - 7 ,widget->pos().y() - 2);
        painter->drawText(new_point, list.at(list.size()-1) );
    }
        scene->update();

    QGraphicsTextItem::paint(painter, option, widget);
}
