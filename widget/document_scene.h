#ifndef DOCUMENTSCENE_H
#define DOCUMENTSCENE_H

#include <QGraphicsScene>

class TreeElement;
class Analyzer;
class Block;
class DocumentTabs;

class DocumentScene : public QGraphicsScene
{
    Q_OBJECT

public:
    DocumentScene(Analyzer *analyzer, QObject *parent = 0);

    void loadFile(const QString &fileName);
    void saveFile(const QString &fileName);
    void showInsertLine(QLineF line);
    void hideInsertLine();

    Analyzer *analyzer;
    bool modified;
    void reanalyze();
    void analyzeAll(QString text);

public slots:

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

private slots:
    void lostFocus(Block *block);

private:
    TreeElement *root;
    Block *mainBlock;

    QGraphicsLineItem *insertLine;

    Block* blockAt(QPointF pos) const;
};

#endif // DOCUMENTSCENE_H
