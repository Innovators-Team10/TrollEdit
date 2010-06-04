#include "document_scene.h"
#include "main_window.h"
#include "block_group.h"
#include "analyzer.h"
#include "block.h"
#include "text_item.h"
#include "doc_block.h"
#include "tree_element.h"
#include <QtGui>

QTime DocumentScene::time;

DocumentScene::DocumentScene(MainWindow *parent)
    : QGraphicsScene(parent)
{
    window = parent;
    currentGroup = 0;
//    setItemIndexMethod(QGraphicsScene::NoIndex);
}

void DocumentScene::newGroup(Analyzer *defaultAnalyzer)
{
    loadGroup("", defaultAnalyzer);
}

void DocumentScene::loadGroup(const QString &fileName, Analyzer *analyzer)
{
    QString content;
    if (fileName.isEmpty()) {
        content = "";
    } else {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::warning(new QWidget, tr("TrollEdit"),
                                 tr("Cannot read file %1:\n%2.").arg(file.fileName()).arg(file.errorString()));
            return;
        }
        QTextStream in(&file);
        content = in.readAll();
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    time.start();
    if (currentGroup != 0) { // todo
        groups.removeOne(currentGroup);
        removeItem(currentGroup);
        delete currentGroup;
        qDebug("\n Old group deleted: %d", time.restart());
    }
    currentGroup = 0;

    BlockGroup *newGr = new BlockGroup(content, analyzer, this);
    groups << newGr;
    qDebug("\nGroup created: %d", time.restart());
    newGr->setFileName(fileName);
    newGr->setModified(false);
    newGr->setPos(30, 30);

    selectGroup(newGr);

    QApplication::restoreOverrideCursor();
    update();
}

void DocumentScene::saveGroup(const QString &fileName, BlockGroup *group)
{
    if (group == 0) {
        if (currentGroup == 0) return;
        group = currentGroup;
    }

    QFile file;
    if (fileName.isEmpty()) {
        if (group->getFileName().isEmpty()) {
            saveGroupAs(group);
            return;
        } else {
            file.setFileName(group->getFileName());
        }
    } else {
        file.setFileName(fileName);
    }

    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(new QWidget, tr("TrollEdit"),
                             tr("Cannot write file %1:\n%2.").arg(file.fileName()).arg(file.errorString()));
        return;
    }
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QTextStream out(&file);
    out << group->toText();
    QApplication::restoreOverrideCursor();

    group->setFileName(file.fileName());
    group->setModified(false);
    update();
    window->setWindowModified(false);
}

void DocumentScene::saveGroupAs(BlockGroup *group)
{
    if (group == 0) {
        if (currentGroup == 0) return;
        group = currentGroup;
    }
    QString fileName = QFileDialog::getSaveFileName((QWidget*)parent());

    if (!fileName.isEmpty())
        saveGroup(fileName, group);
}

void DocumentScene::saveAllGroups() {
    foreach (BlockGroup *group, groups) {
        saveGroup("", group);
    }
}

void DocumentScene::closeGroup(BlockGroup *group)
{
    if (group == 0) {
        if (currentGroup == 0) return;
        group = currentGroup;
        currentGroup = 0;
    }
    if (group->isModified())
        saveGroup("", group);

    groups.removeOne(group);
    delete group;
    if (!groups.isEmpty())
        selectGroup(groups.last());
    else
        selectGroup();
    update();
}

void DocumentScene::closeAllGroups() {
    foreach (BlockGroup *group, groups) {
        closeGroup(group);
    }
}

void DocumentScene::selectGroup(BlockGroup *group)
{
    bool updateNedded = false;
    if (currentGroup != 0 && currentGroup != group) {
        currentGroup->deselect();
        currentGroup->mainBlock()->updateBlock();
        currentGroup = 0;
        updateNedded = true;
    }
    if (currentGroup != group) {
        currentGroup = group;        
        updateNedded = true;
    }
    if (currentGroup != 0) {
        QString fileName = QFileInfo(currentGroup->getFileName()).fileName();
        if (fileName.isEmpty()) fileName = "unknown";
        window->setWindowFilePath(fileName);
        window->setWindowModified(currentGroup->isModified());
    } else {
        window->setWindowFilePath("");
        window->setWindowModified(false);
    }

    if (updateNedded)
        update();
}

void DocumentScene::groupWasModified(BlockGroup *group)
{
    if (group == currentGroup)
        window->setWindowModified(group->isModified());
}

void DocumentScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if ((event->modifiers() & Qt::ControlModifier) == Qt::ControlModifier) {
            if (currentGroup != 0) {
                DocBlock *block = currentGroup->addDocBlock(event->scenePos());
                block->addText("");
                currentGroup->selectBlock(block, true);
                block->textItem()->setTextCursorPos(0);
                return;
            }
        } else {
            QGraphicsItem *item = itemAt(event->scenePos());
            if (item == 0) {
                selectGroup();
            }
        }
    }
//*** DEBUGING & TESTING
    if (currentGroup != 0)
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

void DocumentScene::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    if ((event->modifiers() & Qt::ControlModifier) == Qt::ControlModifier) {
        qreal delta = event->delta() / 100.0;
        if (delta > 0)
            currentGroup->setScale(currentGroup->scale() * delta);
        if (delta < 0)
            currentGroup->setScale(currentGroup->scale() / -delta);
        event->accept();
    } else {
        event->ignore();
    }
}

void DocumentScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if ((event->modifiers() & Qt::ControlModifier) == Qt::ControlModifier
        && event->button() == Qt::MidButton) {
        currentGroup->setScale(1.0);
        event->accept();
    } else {
        event->ignore();
    }
    QGraphicsScene::mouseDoubleClickEvent(event);

}

void DocumentScene::update(const QRectF &rect)
{
    adjustSceneRect();//QRectF(views().first()->rect()));
    QGraphicsScene::update(rect);
}

void DocumentScene::adjustSceneRect()
{
    QRectF rect = views().first()->rect();
    foreach (BlockGroup *group, groups) {
        rect = rect.united(group->mapRectToParent(group->boundingRect()));
    }
    setSceneRect(rect);
}

void DocumentScene::setHighlighting(const QList<QPair<QString, QHash<QString, QString> > > configData)
{
    for (int i = 0; i < configData.length(); i++) {
        QHash<QString, QString>  attributes = configData.value(i).second;
        QFont font;
        QColor color;

        if (attributes.contains("base")) {
            QPair<QFont, QColor> baseStyle = highlighting.value(attributes.value("base"));
            font = QFont(baseStyle.first);
            color = QColor(baseStyle.second);
        }

        // set attributes
        if (attributes.contains("color"))
            color.setNamedColor(attributes.value("color"));
        if (attributes.contains("family"))
            font.setFamily(attributes.value("family"));
        if (attributes.contains("size"))
            font.setPointSize(attributes.value("size").toInt());
        if (attributes.contains("bold"))
            font.setBold(toBool(attributes.value("bold")));
        if (attributes.contains("italic"))
            font.setItalic(toBool(attributes.value("italic")));
        if (attributes.contains("underline"))
            font.setUnderline(toBool(attributes.value("underline")));

        highlighting.insert(configData.value(i).first, QPair<QFont, QColor>(font, color));
    }
}

bool DocumentScene::hasFormatFor(QString key) const
{
    return highlighting.contains(key);
}

QPair<QFont, QColor> DocumentScene::getFormatFor(QString key) const
{
    return highlighting.value(key);
}

QPair<QFont, QColor> DocumentScene::getDefaultFormat() const
{
    return highlighting.value("text_style");
}

void DocumentScene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    focusInEvent(new QFocusEvent(QEvent::FocusIn, Qt::MouseFocusReason));
    if (event->mimeData()->hasText() || event->mimeData()->hasUrls() || event->mimeData()->hasImage()) {
//        || event->mimeData()->hasFormat(BlockGroup::BLOCK_MIME))
        event->accept();
    } else {
        event->ignore();
    }

    if (itemAt(event->scenePos()) != 0) {
        QGraphicsScene::dragEnterEvent(event);
        return;
    }
}

void DocumentScene::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    focusOutEvent(new QFocusEvent(QEvent::FocusOut, Qt::MouseFocusReason));

    if (itemAt(event->scenePos()) != 0) {
        QGraphicsScene::dragLeaveEvent(event);
        return;
    }
}

void DocumentScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    if (event->mimeData()->hasText() || event->mimeData()->hasUrls() || event->mimeData()->hasImage()) {
//        || event->mimeData()->hasFormat(BlockGroup::BLOCK_MIME))
        event->accept();
        if (currentGroup != 0) { // scroll while dragging
            QPointF p = currentGroup->mapFromScene(event->scenePos());
            currentGroup->ensureVisible(p.x(), p.y(), 1, 1, 30, 30);
        }
    } else {
        event->ignore();
    }

    if (itemAt(event->scenePos()) != 0) {
        QGraphicsScene::dragMoveEvent(event);
        return;
    }
}

void DocumentScene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    DocBlock *docBlock = 0;
    if (event->mimeData()->hasUrls()) {
        foreach (QUrl url, event->mimeData()->urls()) {
            QFileInfo info(url.toLocalFile());
            if (QImageReader::supportedImageFormats().contains(info.suffix().toLower().toLatin1())) {
                QImage image = QImage(info.filePath());
                if(!image.isNull()) {
                    docBlock = currentGroup->addDocBlock(event->scenePos());
                    docBlock->addImage(image, info.filePath());
                }
                return;
            }
            else if (info.isFile()){
                docBlock = currentGroup->addDocBlock(event->scenePos());
                docBlock->addLink(url);
                return;
            }
            docBlock = currentGroup->addDocBlock(event->scenePos());
            docBlock->addWebLink(url);
        }
//    } else if (event->mimeData()->hasFormat(BlockGroup::BLOCK_MIME)) {
//        Block *selected = currentGroup->selectedBlock();
//        if (selected != 0) {
//            selected->removeBlock(true);
//            selected = currentGroup->selectedBlock();
//            selected->getFirstLeaf()->textItem()->setTextCursorPos(0);
//        }
    } else if (event->mimeData()->hasText()) {
        QString str = event->mimeData()->text();
        str = str.mid(0, 4);
        if (str.operator ==("http") || str.operator >=("www")){
            docBlock = currentGroup->addDocBlock(event->scenePos());
            docBlock->addWebLink(event->mimeData()->text());
            return;
        }
        docBlock = currentGroup->addDocBlock(event->scenePos());
        docBlock->addText(event->mimeData()->text());
    }

    if (docBlock != 0) {
        currentGroup->selectBlock(docBlock);
        docBlock->updateGeometryAfter();
        if (docBlock->type() == docBlock->Text)
            docBlock->textItem()->setTextCursorPos(0);
    }

    if (itemAt(event->scenePos()) != 0) {
        QGraphicsScene::dropEvent(event);
        return;
    }
}

bool DocumentScene::toBool(QString textBool)
{
    if (textBool.toLower() == "true")
        return true;
    return false;
}
