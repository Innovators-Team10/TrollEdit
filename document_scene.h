#ifndef DOCUMENTSCENE_H
#define DOCUMENTSCENE_H

#include <QGraphicsScene>
#include <QtGui>
#include "block.h"
#include "analyzer.h"
#include "tree_element.h"

class DocumentTabs;

class DocumentScene : public QGraphicsScene
{
    Q_OBJECT

public:
    DocumentScene(Analyzer *analyzer, QObject *parent = 0);
    //void setFont(const QFont &newFont);
    //void loadFile(const QString &fileName);
    //void saveFile(const QString &fileName);

    Analyzer *analyzer;
    bool modified;

public slots:

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

private slots:
    void lostFocus(Block *block);

private:
    Block *clickedBlock;
    TreeElement *root;

    Block* blockAt(QPointF pos);
};

#endif // DOCUMENTSCENE_H
