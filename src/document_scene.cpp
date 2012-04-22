/** 
* @file document_scene.cpp
* @author Team 04 Ufopak + Team 10 Innovators
* @version 
* 
* @section DESCRIPTION
* Contains the defintion of class DocumentScene and it's functions and identifiers.
*/

#include "document_scene.h"
#include "main_window.h"
#include "block_group.h"
#include "text_group.h"
#include "analyzer.h"
#include "block.h"
#include "text_item.h"
#include "doc_block.h"
#include "tree_element.h"
#include "language_manager.h"
#include <QtGui>

QTime DocumentScene::time;
int unknownCounter = 0;

DocumentScene::DocumentScene(MainWindow *parent)
    : QGraphicsScene(parent)
{
    window = parent;    // currently not in use
    currentGroup = 0;
//    setItemIndexMethod(QGraphicsScene::NoIndex);
}

DocumentScene::~DocumentScene(){
    main=0;
}

void DocumentScene::newGroup(QString extension)
{
    loadGroup("", extension);
}

bool loadingFinished;

void DocumentScene::loadGroup(QString fileName, QString extension)
{
    QString content;

    if (fileName.isEmpty())
    {
        content = "";
        fileName = QString("Unknown%1").arg(++unknownCounter);
    }
    else
    {
        QFile file(fileName);

        if (!file.open(QIODevice::ReadOnly))
        {
            QMessageBox::warning(new QWidget, tr("TrollEdit"),
                                 tr("Cannot read file %1:\n%2.").arg(file.fileName()).arg(file.errorString()));
            return;
        }

        QTextStream in(&file);
        content = in.readAll();
    }

    selectGroup(getBlockGroup());
    loadingFinished = false;

    if (groups.size() == 0)
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);
    }
    else
    {
        QApplication::setOverrideCursor(Qt::CrossCursor);
        window->statusBar()->showMessage("Select position");
    }

    time.start();
    BlockGroup *newGr;
    if(fileName.startsWith("Unknown")){
        newGr = new BlockGroup(content, extension, this);
    }else{
        newGr = new BlockGroup(content, fileName, this);
    }
    newGr->setVisible(false);
    groups << newGr;
    qDebug("\nGroup created: %d", time.restart());
    newGr->setFileName(fileName);
    newGr->setModified(false);
    selectGroup(newGr);

    loadingFinished = true;

    if (groups.size() == 1)
    {
        newGr->setPos(30, 30);
        newGr->setVisible(true);
        window->statusBar()->showMessage("File loaded", 2000);
        newGr->mainBlock()->getFirstLeaf()->textItem()->setTextCursorPos(0);
        QApplication::restoreOverrideCursor();
        update();
    }
}

void DocumentScene::revertGroup(BlockGroup *group)
{
    group=getBlockGroup();

    QString fileName = group->getFilePath();

    if (!QFileInfo(fileName).exists()) return;

    QString content;

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(new QWidget, tr("TrollEdit"),
                             tr("Cannot read file %1:\n%2.").arg(file.fileName()).arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
    content = in.readAll();

    QApplication::setOverrideCursor(Qt::WaitCursor);
    group->setContent(content);
    group->setModified(false);

    window->statusBar()->showMessage("File reverted", 2000);
    group->mainBlock()->getFirstLeaf()->textItem()->setTextCursorPos(0);
    QApplication::restoreOverrideCursor();
    update();
}

void DocumentScene::saveGroup(QString fileName, BlockGroup *group, bool noDocs)
{
    group=getBlockGroup();

    QFile file;

    if (fileName.isEmpty())
    {
        if (!QFileInfo(group->getFilePath()).exists())
        {
            saveGroupAs(group);

            return;
        }
        else
        {
            file.setFileName(group->getFilePath());
        }
    }
    else
    {
        file.setFileName(fileName);
    }

    if (!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(new QWidget, tr("TrollEdit"),
                             tr("Cannot write file %1:\n%2.").arg(file.fileName()).arg(file.errorString()));
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    QTextStream out(&file);
    out << group->toText(noDocs);
    QApplication::restoreOverrideCursor();

    group->setFileName(file.fileName());
    group->setModified(false);
    update();
    emit modified(false);

    QString msg = "File saved";

    if (noDocs) msg.append(" without comments");

    window->statusBar()->showMessage(msg, 2000);
}

void DocumentScene::saveGroupAs(BlockGroup *group)
{
    //group=getBlockGroup();

    QString dir = QFileInfo(window->windowFilePath()).absoluteDir().absolutePath();

    QString fileName = QFileDialog::getSaveFileName((QWidget*)parent(), tr("Save file..."),
                                                    dir);

    if (!fileName.isEmpty()) saveGroup(fileName, group);

}

void DocumentScene::saveGroupAsWithoutDoc(BlockGroup *group)
{
    group=getBlockGroup();

    QString fileName = QFileDialog::getSaveFileName((QWidget*)parent());

    if (!fileName.isEmpty()) saveGroup(fileName, group, true);

}

void DocumentScene::saveAllGroups()
{
    foreach (BlockGroup *group, groups)
    {
        saveGroup(group->fileName, group);
//        saveGroup("", group); // povodna funkcia
    }
}

void DocumentScene::closeGroup(BlockGroup *group)
{
    group=getBlockGroup();
    if(group==0) return;

    if (group->isModified())
    {
        QFileInfo info(group->getFilePath());
        QString fileName;

        if (info.exists())
            fileName = info.fileName();
        else
            fileName = group->getFilePath();

        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(window, tr("Save changes"),
                                   tr("File \"%1\" has been modified.\n"
                                      "Do you want to save your changes?").arg(fileName),
                                   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
        {
            saveGroup("", group);
        }
        else if (ret == QMessageBox::Cancel)
        {
            return;
        }
    }

    if (currentGroup == group) currentGroup = 0;
    groups.removeOne(group);
    delete group;

    if (!groups.isEmpty())
    {
        selectGroup(groups.last());
    }
    else
    {
        selectGroup();
    }
    update();
}

void DocumentScene::closeAllGroups()
{
    foreach (BlockGroup *group, groups)
    {
        closeGroup(group);
    }
}

void DocumentScene::findText(QString searchStr, BlockGroup *group)
{
    if (searchStr.isEmpty()) return;

    group=getBlockGroup();

    bool ret = false;
    group->clearSearchResults();
    bool inner = false;

    QRegExp blockMatch("@(\\S*)");

    if (blockMatch.indexIn(searchStr) > -1) //! search inner blocks
    {
        searchStr = blockMatch.cap(1);
        inner = true;
    }
    QRegExp exactMatch("\"(\\S*)\"");

    if (exactMatch.indexIn(searchStr) > -1) //! only exact blocks
    {
        searchStr = exactMatch.cap(1);
        ret = group->searchBlocks(searchStr, inner, true);
    }
    else //! any blocks
    {
        ret = group->searchBlocks(searchStr, inner, false);
    }

    if (!ret && !inner) //! search text
    {
        QSet<int> lineNumbers;
        QString allText = group->toText(true);
        QStringList lines = allText.split("\n", QString::KeepEmptyParts);

        for (int i =0 ;i < lines.size(); i++)
        {
            QString line = lines.at(i);

            if (line.contains(searchStr)) lineNumbers << i;

        }

        if (!lineNumbers.isEmpty())
        {
            group->highlightLines(lineNumbers);
            ret = true;
        }
    }
    if (ret)
    {
        group->update();
        window->statusBar()->showMessage("Search finished", 1000);
    }
    else
    {
        window->statusBar()->showMessage("Not found", 1000);
    }
}

void DocumentScene::cleanGroup(BlockGroup *group)
{
    group=getBlockGroup();
    group->clearSearchResults();
    group->update();
}

void DocumentScene::setGroupLang(Analyzer *newAnalyzer, BlockGroup *group)
{
    group=getBlockGroup();
    if(group==0){
        return;
    }
    QString content = group->toText();
    group->setAnalyzer(newAnalyzer);
    group->setContent(content);
}


void DocumentScene::selectGroup(BlockGroup *group)
{
    bool updateNedded = false;

    if (currentGroup != 0 && currentGroup != group)
    {
        currentGroup->deselect();
        currentGroup->mainBlock()->updateBlock();
        currentGroup = 0;
        updateNedded = true;
    }
    if (currentGroup != group)
    {
        currentGroup = group;
        updateNedded = true;
    }
    if (currentGroup != 0)
    {
        emit fileSelected(currentGroup);
        emit modified(currentGroup->isModified());
    }
    else
    {
        emit fileSelected(0);
        emit modified(false);
    }

    if (updateNedded) update();
}

void DocumentScene::groupWasModified(BlockGroup *group)
{
    if (group == currentGroup)
        emit modified(group->isModified());
}

BlockGroup *DocumentScene::getBlockGroup()
{
    if(main->getScene()->currentGroup!=0){
        qDebug("scene->currentGroup !=0");
        return main->getScene()->currentGroup;
    } else{
        qDebug("current=0");
        return 0;
    }
}

void DocumentScene::showPreview(BlockGroup *group)
{
    group=getBlockGroup();

    QDialog *dialog = new QDialog(window);
    QLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom, dialog);
    QPlainTextEdit *edit = new QPlainTextEdit();
    QString text = group->toText();
    edit->appendPlainText(text);
//    edit->setTextInteractionFlags(Qt::NoTextInteraction);
//    connect(edit->document(), SIGNAL(contentsChanged()),
//            dialog, SLOT(setWindowModified(bool)));
    layout->addWidget(edit);
    dialog->setLayout(layout);
    dialog->setWindowTitle(window->windowTitle() + " - text preview");
    dialog->exec();
    QString newText = edit->toPlainText();

    if (newText != text)
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        group->analyzeAll(newText);
        QApplication::restoreOverrideCursor();
    }
    else
    {
        window->statusBar()->showMessage("No changes", 2000);
    }

}

void DocumentScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{

    if (QApplication::overrideCursor() != 0 &&
        QApplication::overrideCursor()->shape() == Qt::CrossCursor)
    {
        QApplication::restoreOverrideCursor();
        QApplication::setOverrideCursor(Qt::WaitCursor);

        while (!loadingFinished);

        currentGroup->setPos(event->scenePos());
        currentGroup->setVisible(true);
        currentGroup->mainBlock()->getFirstLeaf()->textItem()->setTextCursorPos(0);
        window->statusBar()->showMessage("File loaded", 2000);
        QApplication::restoreOverrideCursor();

        return;
    }

    if (event->button() == Qt::LeftButton)
    {
        if ((event->modifiers() & Qt::ControlModifier) == Qt::ControlModifier)
        {
            if (currentGroup != 0)
            {
                DocBlock *block = currentGroup->addDocBlock(event->scenePos());
                block->addText("");
                currentGroup->selectBlock(block, true);
                block->textItem()->setTextCursorPos(0);

                return;
            }
        }
        else
        {
           QGraphicsItem *item = itemAt(event->scenePos());
//            TextGroup* t = ((TextGroup*)(item));
            //            if(qobject_cast<TextGroup*>(item)!=0){
 //           if(t!=0){
  //              selectGroup(t->block);
  //          }
            if (item == 0)
            {
                selectGroup();
            }
        }
    }

//*** experiment
    if (currentGroup != 0)
//    if (event->button() == Qt::LeftButton){
//        if ((event->modifiers() & Qt::AltModifier) == Qt::AltModifier)
//        {
//            TextGroup *txt = new TextGroup(currentGroup, this);

//            this->addItem(txt);
            /*
            QGraphicsTextItem *txt = new QGraphicsTextItem(currentGroup->toText(), currentGroup);
            //txt->setText(currentGroup->toText());
            txt->setScale(currentGroup->scale());
            txt->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable | QGraphicsItem::ItemIsMovable);
            txt->setTextInteractionFlags(Qt::TextEditorInteraction);
            this->addItem(txt);
            */

//            event->accept();
//        }
//    }

//*** DEBUGING & TESTING
    if (currentGroup != 0)

    if (event->button() == Qt::RightButton) //! AST testing
    {
        QString str = "";
        TreeElement *rootEl = currentGroup->root->getElement()->getRoot();

        if ((event->modifiers() & Qt::ControlModifier) == Qt::ControlModifier)
        {
            for (int i = 0; i <= currentGroup->lastLine; i++)
            {
                str.append(QString("%1").arg(i)).
                        append(" - "+currentGroup->lineStarts[i]->getElement()->getType()+": ").
                        append(currentGroup->lineStarts[i]->getFirstLeaf()->getElement()->getType()+"\n");
            }

            str.append(QString("Last line: %1").arg(currentGroup->lastLine));

        }
        else if ((event->modifiers() & Qt::AltModifier) == Qt::AltModifier){
            QList<TreeElement*> list = rootEl->getDescendants();

            foreach (TreeElement *el, list)
            {
                str.append("- ");

                str.append(QString("%1").arg(el->getSpaces()));
                str.append("  "+el->getType());
                if(TreeElement::DYNAMIC){
                    for(int i = 0; i < el->local_deep_AST; i++)
                        str.append(QString("%1 ").arg(el->local_nodes_AST[i]));
                    str.append(QString("child:%1 ").arg(el->childCount()));
                }
                if (el->isLineBreaking()) str.append("*");

                str.append("\n");
            }
        }
        else if ((event->modifiers() & Qt::ShiftModifier) == Qt::ShiftModifier)
        {
            QList<TreeElement*> list = rootEl->getDescendants();

            foreach (TreeElement *el, list)
            {
                if (el->getBlock() != 0)
                {
                    str.append(QString("%1 ").arg(el->getBlock()->getLine()));
                }
                else
                {
                    str.append("- ");
                }

                str.append(QString("%1").arg(el->getSpaces()));
                str.append("  "+el->getType());

                if (el->isLineBreaking()) str.append("*");

                str.append("\n");
            }
        }
        else
        {
            str = "";//rootEl->getText();
        }
        if (!str.isEmpty())
        {
            QGraphicsItem *text = addText(str);
            text->setPos(event->scenePos());
            text->setZValue(-1);
        }
    }

    QGraphicsScene::mousePressEvent(event);
}

void DocumentScene::wheelEvent(QGraphicsSceneWheelEvent *event)
{
    if (currentGroup == 0)
    {
        event->ignore();

        return;
    }

    if ((event->modifiers() & Qt::ControlModifier) == Qt::ControlModifier)
    {
        qreal delta = event->delta() / 100.0;

        //QSyntaxHighlighter  *highlighter = new QSyntaxHighlighter(txt->document());
        //currentGroup->setVisible(false);
        currentGroup->highlightON_OFF();
        qDebug()<< "highlightON_OFF()";
        qDebug("delta: %f", delta);
        update();

        if (delta > 0)
        {
            currentGroup->setScale(currentGroup->scale() * delta);
            currentGroup->getTextGroup()->rc->setScale(currentGroup->getTextGroup()->rc->scale() * delta);
        }

        if (delta < 0)
        {
            currentGroup->setScale(currentGroup->scale() / -delta);
            currentGroup->getTextGroup()->rc->setScale(currentGroup->getTextGroup()->rc->scale() / -delta);
        }

        event->accept();
        adjustSceneRect();
    }
    else
    {
        event->ignore();
    }
}

void DocumentScene::adjustScale(qreal delta)
{
    if (currentGroup == 0)
    {
        qDebug("adjustScale ignored");
        return;
    }
    else
    {
        currentGroup->highlightON_OFF();
        qDebug()<< "highlightON_OFF()";
        qDebug("delta: %f", delta);
        update();
        if (delta > 0)
        {
            currentGroup->setScale(currentGroup->scale() * delta);
            currentGroup->getTextGroup()->setScale(currentGroup->getTextGroup()->scale() * delta);
        }

        if (delta < 0)
        {
            currentGroup->setScale(currentGroup->scale() / -delta);
            currentGroup->getTextGroup()->setScale(currentGroup->getTextGroup()->scale() / -delta);
        }
        adjustSceneRect();
    }
}

void DocumentScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if ((event->modifiers() & Qt::ControlModifier) == Qt::ControlModifier
        && event->button() == Qt::MidButton)
    {
        currentGroup->setScale(1.0);

        event->accept();
    }
    else
    {
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

    foreach (BlockGroup *group, groups)
    {
        rect = rect.united(group->mapRectToParent(group->boundingRect()));
    }

    setSceneRect(rect);
}

void DocumentScene::setHighlighting(const QList<QPair<QString, QHash<QString, QString> > > configData)
{
    for (int i = 0; i < configData.length(); i++)
    {
        QHash<QString, QString>  attributes = configData.value(i).second;
        QFont font;
        QColor color;

        if (attributes.contains("base"))
        {
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

    if (event->mimeData()->hasText() || event->mimeData()->hasUrls() || event->mimeData()->hasImage()
        || event->mimeData()->hasFormat(BlockGroup::BLOCK_MIME))
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }

    if (itemAt(event->scenePos()) != 0)
    {
        QGraphicsScene::dragEnterEvent(event);

        return;
    }
}

void DocumentScene::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    focusOutEvent(new QFocusEvent(QEvent::FocusOut, Qt::MouseFocusReason));
    QApplication::restoreOverrideCursor();

    if (itemAt(event->scenePos()) != 0)
    {
        QGraphicsScene::dragLeaveEvent(event);

        return;
    }
}

void DocumentScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    if (event->mimeData()->hasText() || event->mimeData()->hasUrls() || event->mimeData()->hasImage()
        || event->mimeData()->hasFormat(BlockGroup::BLOCK_MIME))
    {
        event->accept();

        if (currentGroup != 0) //! scroll while dragging
        {
            QPointF p = currentGroup->mapFromScene(event->scenePos());
            currentGroup->ensureVisible(p.x(), p.y(), 1, 1, 30, 30);
        }
    }
    else
    {
        event->ignore();
    }

    if (itemAt(event->scenePos()) != 0)
    {
        QGraphicsScene::dragMoveEvent(event);

        return;
    }
}

void DocumentScene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    QApplication::restoreOverrideCursor();
    DocBlock *docBlock = 0;

    if (event->mimeData()->hasUrls())
    {
        foreach (QUrl url, event->mimeData()->urls())
        {
            QFileInfo info(url.toLocalFile());

            if (QImageReader::supportedImageFormats().contains(info.suffix().toLower().toLatin1()))
            {
                QImage image = QImage(info.filePath());

                if(!image.isNull())
                {
                    docBlock = currentGroup->addDocBlock(event->scenePos());
                    docBlock->addImage(image, info.filePath());
                }

                return;
            }
            else if (info.isFile())
            {
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
    }
    else if (event->mimeData()->hasText())
    {
        if(currentGroup != 0  && currentGroup->isVisible())
        {
            QString str = event->mimeData()->text();
            str = str.mid(0, 4);

            if (str.operator ==("http") || str.operator >=("www"))
            {
                docBlock = currentGroup->addDocBlock(event->scenePos());
                docBlock->addWebLink(event->mimeData()->text());

                return;
            }

            docBlock = currentGroup->addDocBlock(event->scenePos());
            docBlock->addText(event->mimeData()->text());
        }
    }

    if (docBlock != 0)
    {
        currentGroup->selectBlock(docBlock);
        docBlock->updateGeometryAfter();

        if (docBlock->type() == docBlock->Text)
            docBlock->textItem()->setTextCursorPos(0);
    }

    if (itemAt(event->scenePos()) != 0)
    {
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
