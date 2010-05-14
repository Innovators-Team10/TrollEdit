#include <QtGui>

#include "document_scene.h"
#include "block_group.h"
#include "analyzer.h"
#include "block.h"
#include "text_item.h"
#include "doc_block.h"
#include "tree_element.h"

QGraphicsTextItem *textArea;

DocumentScene::DocumentScene(QWidget *parent)
    : QGraphicsScene(parent)
{
    currentGroup = 0;
    modified = false;
//    textArea = new QGraphicsTextItem(0, this);
//    textArea->setPos(200, 0);
    textArea = 0;
}

void DocumentScene::newGroup(Analyzer *defaultAnalyzer)
{
//    groups << new BlockGroup(QString(" /n"), defaultAnalyzer, this);
//    currentGroup = groups.last();
}

void DocumentScene::loadGroup(const QString &fileName, Analyzer *analyzer)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(new QWidget, tr("TrollEdit"),
                             tr("Cannot read file %1:\n%2.").arg(file.fileName()).arg(file.errorString()));
        return;
    }
    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QString content = in.readAll();

    if (currentGroup != 0) delete currentGroup;

    groups << new BlockGroup(content, analyzer, this);
    currentGroup = groups.last();
//    currentGroup->setRect(sceneRect());

    QApplication::restoreOverrideCursor();

    modified = false;
}

void DocumentScene::saveGroup(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(new QWidget, tr("TrollEdit"),
                             tr("Cannot write file %1:\n%2.").arg(file.fileName()).arg(file.errorString()));
        return;
    }
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QTextStream out(&file);
    if (currentGroup != 0)
        out << currentGroup->toText();
    QApplication::restoreOverrideCursor();

    modified = false;
}

void DocumentScene::closeGroup()
{
    if (currentGroup != 0) {
        groups.removeOne(currentGroup);
        delete(currentGroup);
        if (!groups.isEmpty())
            currentGroup = groups.last();
        else
            currentGroup = 0;
    }
}

void DocumentScene::adjustSceneRect(QRectF rect)
{
    if (currentGroup != 0) {    // todo - other groups
        QRectF blockRect = currentGroup->sceneBoundingRect();
        if (blockRect.right() > rect.right())
            rect.setRight(blockRect.right());
        if (blockRect.bottom() > rect.bottom())
            rect.setBottom(blockRect.bottom());
    }
    setSceneRect(rect);
}

void DocumentScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (itemAt(event->scenePos()) != 0) {
        QGraphicsScene::mousePressEvent(event);
        return;
    }

    if (event->button() == Qt::LeftButton) {
        if ((event->modifiers() & Qt::ControlModifier) == Qt::ControlModifier) {
            DocBlock *block = currentGroup->addDocBlock(event->scenePos());
            block->addText("");
            currentGroup->selectBlock(block);
            block->textItem()->setTextCursorPosition(0);
        } else {
            currentGroup->deselect();
            currentGroup->updateAll();
            update(QRect());
        }
    }

    if (event->button() == Qt::RightButton) { // AST testing
        QString str = "";
        TreeElement *rootEl = currentGroup->root->getElement()->getRoot();
        if ((event->modifiers() & Qt::ControlModifier) == Qt::ControlModifier) {
            for (int i = 0; i <= currentGroup->lastLine; i++) {
                str.append(QString("%1").arg(i)).
                        append(" - "+currentGroup->lineStarts[i]->getElement()->getType()+": ").
                        append(currentGroup->lineStarts[i]->getFirstLeaf()->getElement()->getType()+"\n");
            }
            str.append(QString("Last line: %1").arg(currentGroup->lastLine));

        } else if ((event->modifiers() & Qt::ShiftModifier) == Qt::ShiftModifier) {
            QList<TreeElement*> list = rootEl->getDescendants();
            foreach (TreeElement *el, list) {
                if (el->getBlock() != 0) {
                    str.append(QString("%1 ").arg(el->getBlock()->getLine()));
                } else {
                    str.append("- ");
                }
                str.append(QString("%1").arg(el->getSpaces()));
                str.append("  "+el->getType());
                if (el->isLineBreaking()) str.append("*");
                str.append("\n");
            }
        } else {
            str = rootEl->getText();
        }
        QGraphicsItem *text = addText(str);
        text->setPos(event->scenePos());
        text->setZValue(-1);
    }

    QGraphicsScene::mousePressEvent(event);
}

void DocumentScene::update(const QRectF &rect)
{
    emit requestSize();
    QGraphicsScene::update(rect);
}

void DocumentScene::setHighlightning(const QHash<QString, QPair<QFont, QColor> > &highlightning)
{
    this->highlightning = highlightning;
}

QHash<QString, QPair<QFont, QColor> > DocumentScene::getHighlightning() const
{
    return highlightning;
}

void DocumentScene::setBlockFormatting(const QHash<QString, QHash<QString, QColor> > &blockFormats)
{
    this->blockFormats = blockFormats;
}

QHash<QString, QHash<QString, QColor> > DocumentScene::getBlockFormatting() const
{
    return blockFormats;
}

void DocumentScene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    if (itemAt(event->scenePos()) != 0) {
        QGraphicsScene::dragEnterEvent(event);
        return;
    }
    focusInEvent(new QFocusEvent(QEvent::FocusIn, Qt::MouseFocusReason));
    if (event->mimeData()->hasText() || event->mimeData()->hasUrls() || event->mimeData()->hasImage()
        || event->mimeData()->hasFormat("block_data"))
        event->accept();
    else
        event->ignore();
}

void DocumentScene::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    if (itemAt(event->scenePos()) != 0) {
        QGraphicsScene::dragLeaveEvent(event);
        return;
    }
    Q_UNUSED(event);
    focusOutEvent(new QFocusEvent(QEvent::FocusOut, Qt::MouseFocusReason));
}

void DocumentScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    if (itemAt(event->scenePos()) != 0) {
        QGraphicsScene::dragMoveEvent(event);
        return;
    }
    if (event->mimeData()->hasText() || event->mimeData()->hasUrls() || event->mimeData()->hasImage()
        || event->mimeData()->hasFormat("block_data"))
        event->accept();
    else
        event->ignore();
}

void DocumentScene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    if (itemAt(event->scenePos()) != 0) {
        QGraphicsScene::dropEvent(event);
        return;
    }
    if (event->mimeData()->hasUrls()) {
        foreach (QUrl url, event->mimeData()->urls()) {
            QFileInfo info(url.toLocalFile());
            if (QImageReader::supportedImageFormats().contains(info.suffix().toLower().toLatin1())) {
                QImage image = QImage(info.filePath());
                if(!image.isNull()) {
                    DocBlock *block = currentGroup->addDocBlock(event->scenePos());
                    block->addImage(image);
                }
                return;
            }
            DocBlock *block = currentGroup->addDocBlock(event->scenePos());
            block->addFile(url);
        }
    } else if (event->mimeData()->hasFormat("block_data")) {
        Block *selected = currentGroup->selectedBlock();
        if (selected != 0) {
//            currentGroup->deselect();
            selected->removeBlock(true);
            selected = currentGroup->selectedBlock();
            selected->getFirstLeaf()->textItem()->setTextCursorPosition(0);
        }
    } else if (event->mimeData()->hasText()) {
        DocBlock *block = currentGroup->addDocBlock(event->scenePos());
        block->addText(event->mimeData()->text());
    }
}
