/**
* @file blockmanager.cpp
* @author Team 10 Innovators
* @version
*
* @section DESCRIPTION
* Handles events above BlockGroup and TextGroup.
*/

#include "block_group.h"
#include "text_group.h"
#include "block.h"
#include "doc_block.h"
#include "text_item.h"
#include "tree_element.h"
#include "document_scene.h"
#include "main_window.h"
#include "language_manager.h"

#include <QMessageBox>

BlockManager::BlockManager(QString text, QString file, DocumentScene *scene)
{
    this->docScene = scene;
    this->text=text;
    this->file=file;

    selected = 0;
    lastXPos = -1;
    modified = true;
    searched = false;
    smoothTextAnimation = false;

    computeTextSize();
    time.start();

    docScene->update();
}

BlockManager::~BlockManager()
{
    docScene = 0;
}

void BlockManager::setContent(QString content)
{
    docScene->update();
}

void BlockManager::setModified(bool flag)
{
    if (flag != modified)
    {
        modified = flag;
        docScene->groupWasModified(this);
    }
}

void BlockManager::computeTextSize()
{
//    TAB_LENGTH = analyzer->TAB.length();
//    Block *temp = new Block(new TreeElement("temp"), 0, this);
//    QFontMetricsF *fm = new QFontMetricsF(temp->textItem()->font());
//    CHAR_WIDTH = fm->width(' ');
//    CHAR_HEIGHT = temp->textItem()->boundingRect().height();
//    delete temp;
    CHAR_WIDTH = 10;
    CHAR_HEIGHT = 26;
    TAB_LENGTH = 4;
//    qDebug()<<"CHAR_WIDTH: " << CHAR_WIDTH;     //10
//    qDebug()<<"CHAR_HEIGHT: " << CHAR_HEIGHT;   //26
//    qDebug()<<"TAB_LENGTH: " << TAB_LENGTH;     //4
}

/* **** slots called by signals form TextItem **** */
void BlockManager::keyTyped(QKeyEvent* event)
{
    if (event->key() != Qt::Key_Up && event->key() != Qt::Key_Down)
        lastXPos = -1;
}

// changes the mode and disables/enables the editing actions in the menu
void BlockManager::changeMode(QList<QAction *> actionList)
{
/*    if(isVisible())
    {
        text_group->setPlainText(this->toText());
        text_group->setPos(this->pos().x(),this->pos().y());
        text_group->setScale(this->scale());
        text_group->setFocus();
        text_group->setVisible(true);
        this->setVisible(false);
        docScene->selectGroup(this);
        docScene->update();

        for (int i=0; i<actionList.size(); i++)
            actionList.at(i)->setEnabled(true);
    }
    else
    {
        text_group->setVisible(false);
        this->setContent(text_group->toPlainText());
        this->setPos(text_group->pos().x(),text_group->pos().y());
        this->updateSize();
        this->setVisible(true);
        this->updateSize();
        docScene->update();

        for (int i=0; i<actionList.size(); i++)
            actionList.at(i)->setEnabled(false);
    }*/
}

void BlockManager::changeMode(){
/*    if(isVisible()){
        text_group->setPlainText(this->toText());
        text_group->setPos(this->pos().x(),this->pos().y());
        text_group->setScale(this->scale());
        text_group->setFocus();
        text_group->setVisible(true);
        this->setVisible(false);
        docScene->selectGroup(this);
        docScene->update();
    }else{
        text_group->setVisible(false);
        this->setContent(text_group->toPlainText());
        this->setPos(text_group->pos().x(),text_group->pos().y());
        this->updateSize();
        this->setVisible(true);
        this->updateSize();
        docScene->update();
    }*/
}

QStatusBar *BlockManager::getStatusBar()
{
    foreach (QWidget *widget, QApplication::topLevelWidgets())
    {
        QMainWindow *mainWin = qobject_cast<QMainWindow *>(widget);

        if (mainWin) return mainWin->statusBar();

    }

    Q_ASSERT(false);

    return 0;
}
