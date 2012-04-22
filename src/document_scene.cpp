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
#include "blockmanager.h"
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
    blockManager = 0;
//    setItemIndexMethod(QGraphicsScene::NoIndex);
}

DocumentScene::~DocumentScene(){
    main=0;
    blockManager=0;
    window=0;
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

    selectGroup(getBlockManager());
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
    BlockManager *manager;
    if(fileName.startsWith("Unknown")){
        if(main->mode==MainWindow::Text){
            manager = new TextGroup(new BlockManager(content, extension, this));
            manager->mode=MainWindow::Text;
        } else if(main->mode==MainWindow::Graphic){
            manager = new BlockGroup(new BlockManager(content, extension, this));
            manager->mode=MainWindow::Graphic;
        }
    }else{
        if(main->mode==MainWindow::Text){
            manager = new TextGroup(new BlockManager(content, fileName, this));
            manager->mode=MainWindow::Text;
        } else if(main->mode==MainWindow::Graphic){
            manager = new BlockGroup(new BlockManager(content, fileName, this));
            manager->mode=MainWindow::Graphic;
        }
    }
    this->blockManager=manager;
    groups << manager;
    qDebug("\nGroup created: %d", time.restart());
    manager->setFileName(fileName);
    manager->setModified(false);
    selectGroup(manager);

    loadingFinished = true;

    if (groups.size() == 1)
    {
        if(manager->mode==MainWindow::Graphic){
            ((BlockGroup*)manager)->setPos(30, 30);
            ((BlockGroup*)manager)->mainBlock()->getFirstLeaf()->textItem()->setTextCursorPos(0);
        }else{
            ((TextGroup*)manager)->setPos(30, 30);
        }
        window->statusBar()->showMessage("File loaded", 2000);
        QApplication::restoreOverrideCursor();
        update();
    }
}

void DocumentScene::revertGroup(BlockManager *group)
{
    group=getBlockManager();

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
    if(group->mode==MainWindow::Graphic){
        ((BlockGroup*)group)->mainBlock()->getFirstLeaf()->textItem()->setTextCursorPos(0);
    }
    QApplication::restoreOverrideCursor();
    update();
}

void DocumentScene::saveGroup(QString fileName, BlockManager *group, bool noDocs)
{
    group=getBlockManager();

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
    out << group->text;
    QApplication::restoreOverrideCursor();

    group->setFileName(file.fileName());
    group->setModified(false);
    update();
    emit modified(false);

    QString msg = "File saved";

    if (noDocs) msg.append(" without comments");

    window->statusBar()->showMessage(msg, 2000);
}

void DocumentScene::saveGroupAs(BlockManager *group)
{
    QString dir = QFileInfo(window->windowFilePath()).absoluteDir().absolutePath();

    QString fileName = QFileDialog::getSaveFileName((QWidget*)parent(), tr("Save file..."),
                                                    dir);

    if (!fileName.isEmpty()) saveGroup(fileName, group);
}

void DocumentScene::saveGroupAsWithoutDoc(BlockManager *group)
{
    group=getBlockManager();

    QString fileName = QFileDialog::getSaveFileName((QWidget*)parent());

    if (!fileName.isEmpty()) saveGroup(fileName, group, true);

}

void DocumentScene::saveAllGroups()
{
    foreach (BlockManager *group, groups)
    {
        saveGroup(group->fileName, group);
//        saveGroup("", group); // povodna funkcia
    }
}

void DocumentScene::closeGroup(BlockManager *group)
{
    group=getBlockManager();
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

    if (blockManager == group) blockManager = 0;
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
    foreach (BlockManager *group, groups)
    {
        closeGroup(group);
    }
}

void DocumentScene::findText(QString searchStr, BlockManager *group)
{
    if (searchStr.isEmpty()) return;
    if(group->mode==MainWindow::Graphic){

        group=getBlockManager();

        bool ret = false;
        ((BlockGroup*)group)->clearSearchResults();
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
            ret = ((BlockGroup*)group)->searchBlocks(searchStr, inner, true);
        }
        else //! any blocks
        {
            ret = ((BlockGroup*)group)->searchBlocks(searchStr, inner, false);
        }

        if (!ret && !inner) //! search text
        {
            QSet<int> lineNumbers;
            QString allText = group->text;
            QStringList lines = allText.split("\n", QString::KeepEmptyParts);

            for (int i =0 ;i < lines.size(); i++)
            {
                QString line = lines.at(i);

                if (line.contains(searchStr)) lineNumbers << i;

            }

            if (!lineNumbers.isEmpty())
            {
                ((BlockGroup*)group)->highlightLines(lineNumbers);
                ret = true;
            }
        }
        if (ret)
        {
            ((BlockGroup*)group)->update();
            window->statusBar()->showMessage("Search finished", 1000);
        }
    }
}

void DocumentScene::cleanGroup(BlockManager *group)
{
    if(group->mode==MainWindow::Graphic){
        group=getBlockManager();
        ((BlockGroup*)group)->clearSearchResults();
        ((BlockGroup*)group)->update();
    }
}

void DocumentScene::setGroupLang(Analyzer *newAnalyzer, BlockManager *group)
{
    group=getBlockManager();
    if(group==0){
        return;
    }
//    QString content = group->text; // zbytocne
    if(group->mode==MainWindow::Graphic){
        ((BlockGroup*)group)->setAnalyzer(newAnalyzer);
    }
//    group->setContent(content); // WTF? najprv si vypyta content z groupy a potom ho do nej zapise? seriously? :D
}


void DocumentScene::selectGroup(BlockManager *group)
{
    if(group!=0){
    if(group->mode==MainWindow::Text){
        return;
    }else{
        bool updateNedded = false;
        if (blockManager != 0 && blockManager != group)
        {
            ((BlockGroup*)blockManager)->deselect();
            ((BlockGroup*)blockManager)->mainBlock()->updateBlock();
            blockManager = 0;
            updateNedded = true;
        }
        if (blockManager != group)
        {
            blockManager = group;
            updateNedded = true;
        }
        if (blockManager != 0)
        {
            emit fileSelected(blockManager);
            emit modified(blockManager->isModified());
        }
        else
        {
            emit fileSelected(0);
            emit modified(false);
        }

        if (updateNedded) update();
    }
    }
}

void DocumentScene::groupWasModified(BlockManager *group)
{
    if (group == blockManager)
        emit modified(group->isModified());
}

BlockManager *DocumentScene::getBlockManager()
{
    if(this->blockManager!=0){
        qDebug("scene->blockManager !=0");
        return this->blockManager;
    } else{
        qDebug("current=0");
        return 0;
    }
}

void DocumentScene::showPreview(BlockManager *group)
{
    group=getBlockManager();

    QDialog *dialog = new QDialog(window);
    QLayout *layout = new QBoxLayout(QBoxLayout::TopToBottom, dialog);
    QPlainTextEdit *edit = new QPlainTextEdit();
    edit->appendPlainText(group->text);
//    edit->setTextInteractionFlags(Qt::NoTextInteraction);
//    connect(edit->document(), SIGNAL(contentsChanged()),
//            dialog, SLOT(setWindowModified(bool)));
    layout->addWidget(edit);
    dialog->setLayout(layout);
    dialog->setWindowTitle(window->windowTitle() + " - text preview");
    dialog->exec();
    QString newText = edit->toPlainText();

    if (newText!=group->text && group->mode==MainWindow::Graphic)
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        ((BlockGroup*)group)->analyzeAll(newText);
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

        if(blockManager->mode==MainWindow::Graphic){
            ((BlockGroup*)blockManager)->setPos(event->scenePos());
            ((BlockGroup*)blockManager)->setVisible(true);
            ((BlockGroup*)blockManager)->mainBlock()->getFirstLeaf()->textItem()->setTextCursorPos(0);
        } else{
            ((BlockGroup*)blockManager)->setPos(event->scenePos());
            ((BlockGroup*)blockManager)->setVisible(true);
        }
        window->statusBar()->showMessage("File loaded", 2000);
        QApplication::restoreOverrideCursor();

        return;
    }

    if (event->button() == Qt::LeftButton)
    {
        if ((event->modifiers() & Qt::ControlModifier) == Qt::ControlModifier)
        {
            if (blockManager!=0 && blockManager->mode==MainWindow::Graphic)
            {
                DocBlock *block = ((BlockGroup*)blockManager)->addDocBlock(event->scenePos());
                block->addText("");
                ((BlockGroup*)blockManager)->selectBlock(block, true);
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
    if (blockManager != 0)
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
    if (blockManager != 0)

    if (event->button()==Qt::RightButton && blockManager->mode==MainWindow::Graphic) //! AST testing
    {
        QString str = "";
        TreeElement *rootEl = ((BlockGroup*)blockManager)->root->getElement()->getRoot();

        if ((event->modifiers() & Qt::ControlModifier) == Qt::ControlModifier)
        {
            for (int i = 0; i <= ((BlockGroup*)blockManager)->lastLine; i++)
            {
                str.append(QString("%1").arg(i)).
                        append(" - "+((BlockGroup*)blockManager)->lineStarts[i]->getElement()->getType()+": ").
                        append(((BlockGroup*)blockManager)->lineStarts[i]->getFirstLeaf()->getElement()->getType()+"\n");
            }

            str.append(QString("Last line: %1").arg(((BlockGroup*)blockManager)->lastLine));

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
    if (blockManager == 0)
    {
        event->ignore();

        return;
    }

    if ((event->modifiers() & Qt::ControlModifier) == Qt::ControlModifier)
    {
        qreal delta = event->delta() / 100.0;

        //QSyntaxHighlighter  *highlighter = new QSyntaxHighlighter(txt->document());
        //currentGroup->setVisible(false);
        if(blockManager->mode==MainWindow::Graphic){
            ((BlockGroup*)blockManager)->highlightON_OFF();
        }
        qDebug()<< "highlightON_OFF()";
        qDebug("delta: %f", delta);
        update();

        if (delta > 0)
        {
            if(blockManager->mode==MainWindow::Graphic){
                ((BlockGroup*)blockManager)->setScale(((BlockGroup*)blockManager)->scale() * delta);
            }else{
                ((TextGroup*)blockManager)->setScale(((TextGroup*)blockManager)->scale() * delta);
            }
        }

        if (delta < 0)
        {
            if(blockManager->mode==MainWindow::Graphic){
                ((BlockGroup*)blockManager)->setScale(((BlockGroup*)blockManager)->scale() / -delta);
            }else{
                ((TextGroup*)blockManager)->setScale(((TextGroup*)blockManager)->scale() / -delta);
            }
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
    if (blockManager == 0)
    {
        qDebug("adjustScale ignored");
        return;
    }
    else
    {
        if(blockManager->mode==MainWindow::Graphic){
            ((BlockGroup*)blockManager)->highlightON_OFF();
        }
        qDebug()<< "highlightON_OFF()";
        qDebug("delta: %f", delta);
        update();
        if (delta > 0)
        {
            if(blockManager->mode==MainWindow::Graphic){
                ((BlockGroup*)blockManager)->setScale(((BlockGroup*)blockManager)->scale() * delta);
            }else{
                ((TextGroup*)blockManager)->setScale(((TextGroup*)blockManager)->scale() * delta);
            }
        }

        if (delta < 0)
        {
            if(blockManager->mode==MainWindow::Graphic){
                ((BlockGroup*)blockManager)->setScale(((BlockGroup*)blockManager)->scale() / -delta);
            }else{
                ((TextGroup*)blockManager)->setScale(((TextGroup*)blockManager)->scale() / -delta);
            }
        }
        adjustSceneRect();
    }
}

void DocumentScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if ((event->modifiers() & Qt::ControlModifier) == Qt::ControlModifier
        && event->button() == Qt::MidButton)
    {
        if(blockManager->mode==MainWindow::Graphic){
            ((BlockGroup*)blockManager)->setScale(1.0);
        }else{
            ((TextGroup*)blockManager)->setScale(1.0);
        }
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

    foreach (BlockManager *group, groups)
    {
        if(group->mode==MainWindow::Graphic){
            rect = rect.united(((BlockGroup*)group)->mapRectToParent(((BlockGroup*)group)->boundingRect()));
        }else{
            rect = rect.united(((TextGroup*)group)->mapRectToParent(((TextGroup*)group)->boundingRect()));
        }
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

        if (blockManager != 0) //! scroll while dragging
        {
            if(blockManager->mode==MainWindow::Graphic){
                QPointF p = ((BlockGroup*)blockManager)->mapFromScene(event->scenePos());
                ((BlockGroup*)blockManager)->ensureVisible(p.x(), p.y(), 1, 1, 30, 30);
            }else{
                QPointF p = ((TextGroup*)blockManager)->mapFromScene(event->scenePos());
                ((TextGroup*)blockManager)->ensureVisible(p.x(), p.y(), 1, 1, 30, 30);
            }
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
    if(blockManager->mode==MainWindow::Text){
        return;
    }else{
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
                        docBlock = ((BlockGroup*)blockManager)->addDocBlock(event->scenePos());
                        docBlock->addImage(image, info.filePath());
                    }

                    return;
                }
                else if (info.isFile())
                {
                    docBlock = ((BlockGroup*)blockManager)->addDocBlock(event->scenePos());
                    docBlock->addLink(url);

                    return;
                }

                docBlock = ((BlockGroup*)blockManager)->addDocBlock(event->scenePos());
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
            QString str = event->mimeData()->text();
            str = str.mid(0, 4);

            if (str.operator ==("http") || str.operator >=("www"))
            {
                docBlock = ((BlockGroup*)blockManager)->addDocBlock(event->scenePos());
                docBlock->addWebLink(event->mimeData()->text());

                return;
            }

            docBlock = ((BlockGroup*)blockManager)->addDocBlock(event->scenePos());
            docBlock->addText(event->mimeData()->text());
        }

        if (docBlock != 0)
        {
            ((BlockGroup*)blockManager)->selectBlock(docBlock);
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
}

bool DocumentScene::toBool(QString textBool)
{
    if (textBool.toLower() == "true")
        return true;

    return false;
}
