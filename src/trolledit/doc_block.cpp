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
    removeLinks();            // remove all "contacts with family"
    setParentItem(parentgroup);         // set group as parent item
    myTextItem->adaptToFloating();  // disconnect all harmful signals

    // set flags
    type = Generic;

    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemClipsChildrenToShape, false);
    myTextItem->setTextInteractionFlags(Qt::NoTextInteraction);
    setAcceptDrops(false);
    backup = 0;

    setPos(pos);

    // add arrow
    addArrowTo(arrowTarget);

    // add to list of docblocks
    group->docBlocks << this;
}

DocBlock::DocBlock(TreeElement* element, Block* parentBlock, BlockGroup *parentgroup)
        // automatic creation
    : Block(element, parentBlock, parentgroup)
{
    Q_ASSERT(element->isFloating()); // automatic creation should be called for floating elements only
    // find arrow target
    Block *arrowTarget = 0;
    if (prevSib != 0)
        arrowTarget = prevSib;
    else if (parent != 0)
        arrowTarget = parent;
    // clean up
    removeLinks();            // remove all "contacts with family"
    if (parentBlock != 0) {
        setParentItem(parentBlock->blockGroup()); // set gruop as parent item
    }
    myTextItem->adaptToFloating();  // disconnect all harmful signals

    // set flags
    type = Generic;
    element->setFloating(true);

    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemClipsChildrenToShape, false);
    myTextItem->setTextInteractionFlags(Qt::NoTextInteraction);
    setAcceptDrops(false);
    backup = 0;

    setVisible(false);

    // add arrow
    addArrowTo(arrowTarget);

    // add to list of docblocks
    group->docBlocks << this;
}

DocBlock::~DocBlock()
{
    group->docBlocks.removeOne(this);
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
}

void DocBlock::setDefaultPos()
{
    if (group->mainBlock() == 0) return;
    qreal x = group->mainBlock()->idealPos().x() + group->mainBlock()->idealSize().width() + 100;
    if (arrow == 0) {
        setPos(x ,0);
    } else {
        Block *endItem = arrow->endItem();
        setPos(x, endItem->mapToScene(endItem->idealPos()).y());
    }
    setVisible(true);
}

void DocBlock::updateGeometryAfter(bool doAnimation) {
    updateBlock(doAnimation);
    if (parent != 0)
        parent->updateGeometryAfter(doAnimation);
}

void DocBlock::addText(QString text){
    myTextItem->setTextInteractionFlags(Qt::TextEditable | Qt::TextSelectableByKeyboard);
    myTextItem->setPlainText(text);
    if (arrow != 0)
        arrow->setColor(Qt::darkGreen);
    type = Text;
    updateBlock(false);
}

void DocBlock::addImage(const QImage &image){
    type = Image;
    QTextCursor cursor = QTextCursor(myTextItem->document());
    cursor.document()->setPlainText("");
    cursor.insertImage(image);

    if (arrow != 0)
        arrow->setColor(Qt::blue);
    updateBlock(false);
}

void DocBlock::addLink(QUrl url){
    type = Link;
    myTextItem->setPlainText(url.toString());
    if (arrow != 0)
        arrow->setColor(Qt::blue);

    updateBlock(false);
}

void DocBlock::addArrowTo(Block *end){
    if (end == 0) {
        arrow = 0;
    } else {
        connect(end, SIGNAL(destroyed()), this, SLOT(deleteLater()));
        arrow = new Arrow(this, end, group);
    }
}

QString DocBlock::convertToText() const //TODO
{
    switch (type) {
    case Generic :
        return QString();
    case Text :
        return myTextItem->toPlainText();
    case Image :
        return QString();
    case Link :
        return QString();
    default:
        return QString();
    }
}

void DocBlock::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
    if (type == Link)
        QDesktopServices::openUrl(QUrl(myTextItem->toPlainText()));
}

void DocBlock::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsRectItem::mouseMoveEvent(event);
    if (foldButton != 0)
        foldButton->updatePos();
}

void DocBlock::textChanged()
{
    QString text = myTextItem->toPlainText();
    if (text.isEmpty() && !myTextItem->hasFocus() && type != Generic && type != Image) {
        removeBlock(true);
    } else {
        if (element->getType() != text) {
            if(!isFolded()) {
                element->setType(text);
            }
            if (pos().x() >= 0)
                updateBlock(false);
        }
    }
}

bool DocBlock::isFoldable() const
{
    return Block::isFoldable() || type == Image;
}

void DocBlock::setFolded(bool fold)
{
    if (fold == folded) return; // do nothing
    folded = fold;

    if (fold) {
        backup = myTextItem->document()->clone();
        QString cue;
        if (type == Image) {
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
        if (!type == Text)
            myTextItem->setTextInteractionFlags(Qt::NoTextInteraction);
        backup = 0;
    }
    updateBlock();
}

void DocBlock::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Block::paint(painter, option, widget);
}
