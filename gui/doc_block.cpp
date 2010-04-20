#include "doc_block.h"


DocBlock::DocBlock(QString text, QPointF pos, Block *relatedBlock, QGraphicsScene *parentScene)
    : Block(new TreeElement("", false, true), 0, parentScene)
{
    Block *commentBl = new Block(new TreeElement("doc_comment", true, true),
                                 0, parentScene);
    element->setFloating(false);
    element->setType(text);

    setParentItem(commentBl);

    arrow = 0;
    if (relatedBlock != 0) {
//        relatedBlock->element->appendChild(commentBl->element);
        addArrow(this, relatedBlock, parentScene);
    }

    commentBl->getElement()->setFloating(true);
    commentBl->setPos(pos);

//    if (docScene->getHighlightning().contains("multi_comment")) {
//        myTextItem->setFont(docScene->getHighlightning().value("multi_comment").first);
//    }

}

DocBlock::~DocBlock()
{
    if (arrow != 0) {
        arrow->setVisible(false);
        docScene->update();
        arrow->deleteLater();
    }
}

void DocBlock::addImage(const QImage &image){
    QTextCursor cursor = myTextItem->textCursor();
    this->image = image;
    if (!image.isNull())
        cursor.insertImage(image);
    myTextItem->setTextCursor(cursor);
    myTextItem->setTextInteractionFlags(Qt::NoTextInteraction);
}

void DocBlock::addFile(QUrl url){
    //todo: pridanie linkov na subory ktore sa dragnu na scenu do dokumentacnych blokov
    //file = new QTextBrowser();
    myTextItem->setTextInteractionFlags(Qt::NoTextInteraction);
}

void DocBlock::addArrow(DocBlock *start,Block *end, QGraphicsScene *parentScene){
    arrow = new Arrow(start, end, end, parentScene);
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
    rect.adjust(-width/2-2, -width/2, width/2+2, width/2);
    painter->drawRect(rect);

//    Block::paint(painter, option, widget);
}
