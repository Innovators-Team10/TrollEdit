/** 
* @file doc_block.cpp
* @author Team 04 Ufopak + Team 10 Innovators
* @version 
* 
* @section DESCRIPTION
* Contains the defintion of class DocumentBlock (DocBlock) and it's functions and identifiers.
*/

#include <QTextBrowser>
#include "doc_block.h"
#include "arrow.h"
#include "text_item.h"
#include "tree_element.h"
#include "document_scene.h"
#include "block_group.h"
#include "fold_button.h"

DocBlock::DocBlock(QPointF pos, BlockGroup *parentgroup)    //! manual creation
    : Block(new TreeElement("doc_comment", true, true), 0, parentgroup)
{    
    element->setFloating(true);

    // find arrow target
    Block *arrowTarget = group->selectedBlock();
    setParentBlock(arrowTarget); //! set parent block
    // clean up
    removeLinks();                  //! remove all "contacts with family"
    setParentItem(parentgroup);     //! set group as parent item
    myTextItem->adaptToFloating();  //! disconnect all harmful signals

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
    highlightFormat = group->docScene->getFormatFor(element->getType());
    highlight(highlightFormat);

    // add arrow
    arrow = 0;
    addArrowTo(arrowTarget);
}

DocBlock::DocBlock(QString text, TreeElement* el, Block* parentBlock, BlockGroup *parentgroup)
        // automatic creation
    : Block(el, parentBlock, parentgroup)
{
    Q_ASSERT(el->isFloating()); //! automatic creation should be called for floating elements only
    // find arrow target
    Block *arrowTarget = 0;

    if (prevSib != 0) arrowTarget = prevSib;

    else if (parent != 0)
        arrowTarget = parent;
    // adjust linebreaks
    if (text.endsWith('\n'))
    {
        text.chop(1);

        if (nextSib == 0)
            parent->getElement()->setLineBreaking(true);
        else if (prevSib != 0) prevSib->getElement()->setLineBreaking(true);
    }
    else if (prevSib != 0 && nextSib == 0 &&
               parent->getElement()->getAncestorWhereLast()->isLineBreaking()) {
        prevSib->getElement()->setLineBreaking(false);
    }
    // clean up
    removeLinks();            //! remove all "contacts with family"

    if (parentBlock != 0) setParentItem(parentBlock->blockGroup()); //! set group as parent item

    myTextItem->adaptToFloating();  //! disconnect all harmful signals

    // set flags
    docType = Generic;
    element->setFloating(true);
    element->setType("doc_comment");

    setFlag(QGraphicsItem::ItemIsMovable);
    setFlag(QGraphicsItem::ItemClipsChildrenToShape, false);
    myTextItem->setTextInteractionFlags(Qt::NoTextInteraction); //preco to pada?
    setAcceptDrops(false);
    setZValue(1);
    backup = 0;

    setVisible(true);
    locked = false;
    highlightFormat = group->docScene->getFormatFor(element->getType());
    highlight(highlightFormat);

    // add arrow
    arrow = 0;
    addArrowTo(arrowTarget);

    setContent(text);
}

DocBlock::~DocBlock()
{
}

void DocBlock::updateBlockInMap(DocBlock* element)
{
    element->updateBlock(false);
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
    if (group->mainBlock() == 0)
    {
        idealGeometry.moveTo(0, 0);
        return;
    }

    if (!locked)
    {
        Block *target = targetBlock();

        if (target == 0)
        {
            qreal x = group->mainBlock()->idealPos().x() + group->mainBlock()->idealSize().width() + 100;
            setPos(x ,0);
        }
        else
        {
            setPos(group->mapFromItem(target, target->idealRect().topRight()) + QPointF(100, 0));
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
    pos = mapToItem(myTextItem, pos);   //! map to my TextItem
    int cursorPos;
    // find cursor position
    if(myTextItem == 0)
        qDebug() << "myTextItem null";
    cursorPos = myTextItem->document()->documentLayout()->hitTest(pos, Qt::FuzzyHit);
    // set cursor position

    if (cursorPos < 0 || !myTextItem->setTextCursorPos(cursorPos))
        myTextItem->setTextCursorPos(0);

    return this;                        //! return block with cursor
}

void DocBlock::setContent(QString text)
//        TODO: improve!! QRegExp's different behavior in some cases made difficulties in finding more ellegant solution
{
    QHash<QString, QStringList> commentTokens = group->getAnalyzer()->getCommentTokens();
    QString pattern;
    QString content;

    // no comments supported by this language
    if (commentTokens["line"].value(0, "") == "" && commentTokens["multiline"].value(0, "")  == "")
        return;

    /* construction of the matching pattern; it's basically something like this:
     * "start_token(content)(\[!((pos.x),(pos.y))?(type)!\])?end_token"
     */
    if (commentTokens["line"].value(0, "") == "")
        pattern = QRegExp::escape(commentTokens["multiline"].value(0));
    else if(commentTokens["multiline"].value(0, "")  == "")
        pattern = QRegExp::escape(commentTokens["line"].value(0));
    else
        if (commentTokens["multiline"].value(0).startsWith(commentTokens["line"].value(0)))
            pattern = "(?:" + QRegExp::escape(commentTokens["line"].value(0)) + "(" + QRegExp::escape(commentTokens["multiline"].value(0).remove(0, commentTokens["line"].value(0).length())) + "))";
        else
            pattern = "(" + QRegExp::escape(commentTokens["multiline"].value(0)) + "|" + QRegExp::escape(commentTokens["line"].value(0)) + ")";
    pattern += "\\s*([^\\s].*[^\\s])\\s*\\[!((-?\\d+),(-?\\d+),)?(\\d)!\\].*";

    QRegExp regExp(pattern);
    int pos = regExp.indexIn(text);

    if (pos > -1)
    {
        QImage image;
        int type = regExp.cap(5).toInt();
        content = regExp.cap(1);

        // remove line comment tokens simulating multiline comment if it's not naturally supported
        if (group->getAnalyzer()->queryMultilineSupport() == "false")
            content.replace(QRegExp("\\n\\s*" + QRegExp::escape(commentTokens["line"].value(0))), "\n");

        switch (type)
        {
        case Image:
            image = QImage(content);
            addImage(image, content);
            break;
        case Link:
            addLink(QUrl::fromLocalFile(content));
            break;
        case WebLink:
            addWebLink(QUrl(content));
            break;
        case Text:
        default:
            addText(content);
        }

        // block has saved position
        if (!regExp.cap(2).isEmpty())
        {
            setPos(regExp.cap(3).toFloat(), regExp.cap(4).toFloat());
            locked = true;
        }
    }
    else //! no match with pattern, just add text
    {
        content = text.remove(commentTokens["line"].value(0, ""));
        content.remove(commentTokens["multiline"].value(0, ""));
        content.remove(commentTokens["line"].value(1, ""));
        content.remove(commentTokens["multiline"].value(1, ""));
        content = content.trimmed();
        content.replace(QRegExp("\\n( |\\t)*"), "\n");
        addText(content);
    }
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

    if (arrow != 0) arrow->setColor(getHoverColor());

    updateBlock(false);
}

void DocBlock::addLink(QUrl url)
{
    myTextItem->setTextInteractionFlags(Qt::TextSelectableByKeyboard);
    docType = Link;
    path = url.toString();
    QString str = path;

    // add file icon
    QTextCursor cursor = QTextCursor(myTextItem->document());
    QFileInfo info(url.toLocalFile());
    QFileIconProvider *provider = new QFileIconProvider();
    QImage image(provider->icon(info).pixmap(16, 16).toImage());
    cursor.document()->setPlainText(" ");
    cursor.insertImage(image);
	
    if (str.lastIndexOf("/") > -1)
        str = str.right(str.size() - str.lastIndexOf("/") - 1);

    QString html = "<a href=\""+path+"\">"+str+"</a>";
    cursor.insertHtml(html);
	
    if (arrow != 0) arrow->setColor(getHoverColor());

    updateBlock(false);
}

void DocBlock::addWebLink(QUrl url)
{
    myTextItem->setTextInteractionFlags(Qt::TextSelectableByKeyboard);
    docType = WebLink;

    // add web icon
    QTextCursor cursor = QTextCursor(myTextItem->document());
    cursor.document()->setPlainText(" ");
    cursor.insertImage(QImage(":/weblink.png"));

    path = url.toString();
    QString str = path;

    if (str.lastIndexOf("/") > 7)
        str = str.left(str.lastIndexOf("/"));

    QString html = "<a href=\""+path+"\">"+str+"</a>";
    cursor.insertHtml(html);

    if (arrow != 0)
        arrow->setColor(getHoverColor());

    updateBlock(false);
}

void DocBlock::addArrowTo(Block *end)
{
    if (arrow != 0)
    {
        disconnect(arrow, 0, 0, 0);
        disconnect(targetBlock(), 0, this, 0);
        delete arrow;
    }
    if (end == 0)
    {
        arrow = 0;
    }
    else
    {
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

QString DocBlock::convertToText() const //TODO ak nie je multiline support, upravit blok; a nezobrazujem ziadne znacky ked to mam
{
    QString returnText;
    QHash<QString, QStringList> commentTokens = group->getAnalyzer()->getCommentTokens();

    switch (docType)
    {
    case Text :
        if (hasMoreLines() || commentTokens["line"].value(0, "") == "") {
            returnText = commentTokens["multiline"].value(0) + " ";
            returnText += myTextItem->toPlainText();

            if (locked || docType != Text)
            {
                returnText += QString(" [!");

                if (locked)
                    returnText += QString::number(pos().x()) + "," + QString::number(pos().y()) + ",";

                returnText += QString::number(docType) + "!] ";
            }
            returnText += commentTokens["multiline"].value(1);

            // add line comment tokens simulating multiline comment if it's not naturally supported
            if (group->getAnalyzer()->queryMultilineSupport() == "false")
                returnText.replace("\n", commentTokens["line"].value(1) + "\n" + commentTokens["line"].value(0));
        }
        else
        {
            returnText = commentTokens["line"].value(0) + myTextItem->toPlainText();

            if (locked || docType != Text)
            {
                returnText += QString(" [!");

                if (locked)
                    returnText += QString::number(pos().x()) + "," + QString::number(pos().y()) + ",";

                returnText += QString::number(docType) + "!] ";
            }

            returnText += commentTokens["line"].value(1);
        }
        break;
    case Image :        
    case Link :
    case WebLink :
        returnText = commentTokens["multiline"].value(0) + " ";
        returnText += QString(path + " [!" + QString::number(pos().x()) + "," + QString::number(pos().y()) + "," + QString::number(docType) + "!] ");
        returnText += commentTokens["multiline"].value(1);
        break;
    default:
        if (commentTokens["line"].value(0, "") != "")
            returnText = commentTokens["line"].value(0) + " comment " + commentTokens["line"].value(1);
        else
            returnText = commentTokens["multiline"].value(0) + " comment " + commentTokens["multiline"].value(1);
    }
    return returnText;
}

QColor DocBlock::getHoverColor() const
{
    switch (docType)
    {
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

    if (group->selectedBlock() == this)
    {
        if (arrow != 0)
        {
            next = arrow->endItem();
            group->selectBlock(next);
            arrow->deleteLater();
        }
        else group->deselect();
    }
    
    if (deleteThis)
    {
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

    if (text.isEmpty() && !myTextItem->hasFocus() && docType != Image)
    {
        removeBlock(true);
    }
    else
    {
        if (element->getType() != text)
        {
//            if(!isFolded()) {
//                element->setType(text);
//            }
            group->setModified(true);
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
    if (fold == folded) return; //! do nothing

    folded = fold;

    if (fold)
    {
        backup = myTextItem->document()->clone();

        if (docType == Image)
        {
            QTextCursor cursor = QTextCursor(myTextItem->document());
            cursor.document()->setPlainText(" ");
            cursor.insertImage(QImage(":/image.png"));
            cursor.insertText(QFileInfo(path).fileName());
        }
        else
        {
            QString cue = myTextItem->toPlainText();
            int index = cue.indexOf("\n");
            cue.truncate(qMin(8, index));
            cue.append(" ...");
            myTextItem->setPlainText(cue);
            myTextItem->setTextInteractionFlags(Qt::TextEditable | Qt::TextSelectableByKeyboard);
        }

    }
    else
    {
        Q_ASSERT(backup != 0);
        myTextItem->setDocument(backup);

        if (!docType == Text)
            myTextItem->setTextInteractionFlags(Qt::NoTextInteraction);

        backup = 0;
    }
    updateBlock();
}

void DocBlock::setLocked(bool lock)
{
    locked = lock;
}

void DocBlock::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Block::paint(painter, option, widget);
}
