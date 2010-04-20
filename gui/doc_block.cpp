#include "doc_block.h"


DocBlock::DocBlock(TreeElement *element, Block *parentBlock, QGraphicsScene *parentScene)
    : Block(element, parentBlock, parentScene)
{
    parentBlock->getElement()->setFloating(true);


    addArrow(this, selectedBlock, parentScene);

    if (myTextItem != 0)
        delete(myTextItem);
    myTextItem = new TextItem("", this, element->allowsParagraphs());
    myTextItem->setPos(-myTextItem->margin, 0);

    if (selectedBlock != 0)
        parentBlock->setPos(parentBlock->pos().x(), selectedBlock->scenePos().y());

}
void DocBlock::addImage(const QImage &image){

    QTextCursor cursor = myTextItem->textCursor();
    this->image=image;
    if (!image.isNull())
        cursor.insertImage(image);
    myTextItem->setTextCursor(cursor);
    this->textItem()->setTextInteractionFlags(Qt::NoTextInteraction);
}
void DocBlock::addFile(QUrl url){
    //todo: pridanie linkov na subory ktore sa dragnu na scenu do dokumentacnych blokov
    //file = new QTextBrowser();
}
void DocBlock::addArrow(DocBlock *start,Block *end, QGraphicsScene *parentScene){
    this->arrow = new Arrow(start, end, end, parentScene);
    if(this->getElement()->getType() == "image")
        arrow->setColor(Qt::yellow);
    else
        arrow->setColor(Qt::magenta);
}

void DocBlock::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QRectF rect = boundingRect();
    painter->fillRect(rect, Qt::white);

    qreal width;
    Qt::PenStyle style;
    QColor color;

    width = 2; style = Qt::SolidLine; color = format["selected"];

    painter->setPen(QPen(QBrush(color), width, style));
    rect.adjust(-width/2, -width/2, width/2, width/2);
    painter->drawRect(rect);
}
