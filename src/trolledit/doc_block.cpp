#include <QTextBrowser>
#include "doc_block.h"
#include "arrow.h"
#include "text_item.h"
#include "tree_element.h"
#include "document_scene.h"
#include "block_group.h"
#include "fold_button.h"

DocBlock::DocBlock(QPointF pos, BlockGroup *parentgroup)    // manual creation
    : Block(new TreeElement("doc_comment", true, true), 0, parentgroup)
{    
    element->setFloating(true);

    // find arrow target
    Block *arrowTarget = group->selectedBlock();
    setParentBlock(arrowTarget); // set parent block
    // clean up
    removeLinks();                  // remove all "contacts with family"
    setParentItem(parentgroup);     // set group as parent item
    myTextItem->adaptToFloating();  // disconnect all harmful signals

    // set flags
    docType = Generic;

    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemClipsChildrenToShape, false);
    myTextItem->setTextInteractionFlags(Qt::NoTextInteraction);
    setAcceptDrops(false);
    setZValue(1);
    backup = 0;

    setPos(pos);
    setVisible(true);
    locked = true;

    // add arrow
    addArrowTo(arrowTarget);
}

DocBlock::DocBlock(TreeElement* el, Block* parentBlock, BlockGroup *parentgroup)
        // automatic creation
    : Block(el, parentBlock, parentgroup)
{
    Q_ASSERT(el->isFloating()); // automatic creation should be called for floating elements only
    // find arrow target
    Block *arrowTarget = 0;
    if (parent != 0)
        arrowTarget = parent;
    // clean up
    removeLinks();            // remove all "contacts with family"
    if (parentBlock != 0) {
        setParentItem(parentBlock->blockGroup()); // set group as parent item
    }
    myTextItem->adaptToFloating();  // disconnect all harmful signals

    // set flags
    docType = Generic;
    element->setFloating(true);
    element->setType("doc_comment");

    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemClipsChildrenToShape, false);
    myTextItem->setTextInteractionFlags(Qt::NoTextInteraction);
    setAcceptDrops(false);
    setZValue(1);
    backup = 0;

    setVisible(true);
    locked = false;

    // add arrow
    addArrowTo(arrowTarget);
}

DocBlock::~DocBlock()
{
}

void DocBlock::updateBlock(bool doAnimation)
{
    // update line
    line = -1;
    // update pos
    updatePos(!doAnimation);
    // update size
    updateSize(!doAnimation);
    // update fold button
    updateFoldButton();
    // animate
    if (doAnimation)
        animate();
    group->updateSize();
}

void DocBlock::updatePos(bool updateReal)
{
    if (group->mainBlock() == 0) {
        idealGeometry.moveTo(0, 0);
        return;
    }

    if (!locked) {
        qreal x = group->mainBlock()->idealPos().x() + group->mainBlock()->idealSize().width() + 100;
        if (arrow == 0) {
            setPos(x ,0);
        } else {
            Block *endItem = arrow->endItem();
            setPos(x, group->mapFromItem(endItem->parentItem(), endItem->idealPos()).y());
        }
    }
    idealGeometry.moveTo(pos());
}

void DocBlock::updateGeometryAfter(bool doAnimation)
{
    updateBlock(doAnimation);
}

Block *DocBlock::addTextCursorAt(QPointF pos)
{
    pos = mapToItem(myTextItem, pos);   // map to my TextItem
    int cursorPos;
    // find cursor position
    cursorPos = myTextItem->document()->documentLayout()->hitTest(pos, Qt::FuzzyHit);
    // set cursor position
    if (cursorPos < 0 || !myTextItem->setTextCursorPos(cursorPos))
        myTextItem->setTextCursorPos(0);
    return this;                        // return block with cursor
}

void DocBlock::addText(QString text)
{
    myTextItem->setTextInteractionFlags(Qt::TextEditable | Qt::TextSelectableByKeyboard);
    myTextItem->setPlainText(text);
    docType = Text;
    if (arrow != 0)
        arrow->setColor(getHoverColor());
    path = "";
    updateBlock(false);
}

void DocBlock::addImage(const QImage &image, QString imagePath)
{
    myTextItem->setTextInteractionFlags(Qt::TextSelectableByKeyboard);
    docType = Image;
    QTextCursor cursor = QTextCursor(myTextItem->document());
    cursor.document()->setPlainText("");
    cursor.insertImage(image);
    path = imagePath;

    if (arrow != 0)
        arrow->setColor(getHoverColor());
    updateBlock(false);
}

void DocBlock::addLink(QUrl url)
{
    myTextItem->setTextInteractionFlags(Qt::TextSelectableByKeyboard);
    docType = Link;
    path = url.toString();
    QString str = path;
	
	QTextCursor cursor = QTextCursor(myTextItem->document());
    QFileInfo info(url.toLocalFile());
    QFileIconProvider *provider = new QFileIconProvider();
    QImage image(provider->icon(info).pixmap(16,16).toImage());

    cursor.document()->setPlainText(" ");
    cursor.insertImage(image);
	
    if (str.lastIndexOf("/") > -1)
        str = str.right(str.size() - str.lastIndexOf("/") - 1);
    QString html = "<a href=\""+path+"\">"+str+"</a>";
    cursor.insertHtml(html);
	
    if (arrow != 0)
        arrow->setColor(getHoverColor());
    updateBlock(false);
}

void DocBlock::addWebLink(QUrl url)
{
    myTextItem->setTextInteractionFlags(Qt::TextSelectableByKeyboard);
    docType = WebLink;
    path = url.toString();
    QString str = path;
    if (str.lastIndexOf("/") > 7){
        str = str.left(str.lastIndexOf("/"));
    }
    QString html = "<a href=\""+path+"\">"+str+"</a>";
    myTextItem->setHtml(html);
    if (arrow != 0)
        arrow->setColor(getHoverColor());
    updateBlock(false);
}

void DocBlock::addArrowTo(Block *end)
{
    if (end == 0) {
        arrow = 0;
    } else {
        connect(end, SIGNAL(destroyed()), this, SLOT(deleteLater()));
        arrow = new Arrow(this, end, group);
        connect(arrow, SIGNAL(destroyed()), this, SLOT(arrowDestroyed()));
        arrow->setColor(getHoverColor());
    }
}

void DocBlock::arrowDestroyed()
{
    parent = 0;
    arrow = 0;
}

Block *DocBlock::targetBlock() const
{
    if (arrow != 0) return arrow->endItem();
    return 0;
}

QString DocBlock::convertToText() const //TODO
{
    switch (docType) {
    case Text :
        return myTextItem->toPlainText();
    case Image :
        return QString("//" + path);
    case Link :
        return QString("//" + path);
    case WebLink :
        return QString("//" + path);
    default:
        return QString("// nothing");
    }
}

QColor DocBlock::getHoverColor() const
{
    switch (docType) {
    case Text :
        return Qt::darkGreen;
    case Image :
        return Qt::darkMagenta;
    case Link :
        return Qt::darkMagenta;
    case WebLink :
        return Qt::darkBlue;
    default:
        return Block::getHoverColor();
    }
}

void DocBlock::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
    if (docType == Link)
        QDesktopServices::openUrl(QUrl(path));
    if (docType == WebLink)
        QDesktopServices::openUrl(QUrl(path));
}

void DocBlock::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsRectItem::mouseMoveEvent(event);
    locked = true;
    group->updateSize();
}

Block *DocBlock::removeBlock(bool deleteThis)
{
    Block *next = 0;
    setParentBlock(0);
    if (group->selectedBlock() == this) {
        if (arrow != 0) {
            next = arrow->endItem();
            group->selectBlock(next);
            arrow->deleteLater();
        } else {
            group->deselect();
        }
    }
    
    if (deleteThis) {
        group->removeFoldable(this);
        deleteLater();
    }
    group->updateSize();
    return next;
}

void DocBlock::textChanged()
{
    if (docType == Generic) return;

    QString text = myTextItem->toPlainText();
    if (text.isEmpty() && !myTextItem->hasFocus() && docType != Image) {
        removeBlock(true);
    } else {
        if (element->getType() != text) {
            if(!isFolded()) {
                element->setType(text);
            }
            updateBlock(false);
        }
    }
}

bool DocBlock::isFoldable() const
{
    return Block::isFoldable() || docType == Image;
}

void DocBlock::setFolded(bool fold)
{
    if (fold == folded) return; // do nothing
    folded = fold;

    if (fold) {
        backup = myTextItem->document()->clone();
        QString cue;
        if (docType == Image) {
            cue = "image ..."; //todo
        } else {
            cue = myTextItem->toPlainText();
            int index = cue.indexOf("\n");
            cue.truncate(qMin(8, index));
            cue.append(" ...");
        }
        myTextItem->setPlainText(cue);
        myTextItem->setTextInteractionFlags(Qt::TextEditable | Qt::TextSelectableByKeyboard);
    } else {
        Q_ASSERT(backup != 0);
        myTextItem->setDocument(backup);
        if (!docType == Text)
            myTextItem->setTextInteractionFlags(Qt::NoTextInteraction);
        backup = 0;
    }
    updateBlock();
}

void DocBlock::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Block::paint(painter, option, widget);
}
