/**
 * document_scene.h
 *  ---------------------------------------------------------------------------
 * Contains the declaration of class DocumentScene and it's funtions and identifiers
 *
 */
#ifndef DOCUMENT_SCENE_H
#define DOCUMENT_SCENE_H

#include <QGraphicsScene>
#include <QHash>
#include <QString>
#include <QUrl>

class Analyzer;
class BlockManager;
class MainWindow;

class DocumentScene : public QGraphicsScene
{
    Q_OBJECT

signals:
    void modified(bool flag);
    void fileSelected(BlockManager *group);

public slots:
    void newGroup(QString extension);
    void loadGroup(QString fileName, QString extension);
    void revertGroup(BlockManager *group = 0);
    void saveGroup(QString fileName = "", BlockManager *group = 0, bool noDocs = false);
    void saveGroupAs(BlockManager *group = 0);
    void saveAllGroups();
    void saveGroupAsWithoutDoc(BlockManager *group = 0);
    void closeGroup(BlockManager *group = 0);
    void closeAllGroups();
    void setGroupLang(Analyzer *newAnalyzer, BlockManager *group = 0);
    void showPreview(BlockManager *group = 0);
    void findText(QString searchStr, BlockManager *group = 0);
    void cleanGroup(BlockManager *group = 0);

public:
    MainWindow *main;
    DocumentScene(MainWindow *parent);
    void update(const QRectF &rect = QRectF());
    void selectGroup(BlockManager *group = 0);
    BlockManager *selectedGroup() const {return blockManager;}
    void groupWasModified(BlockManager *group);

    void setHighlighting(const QList<QPair<QString, QHash<QString, QString> > > configData);
    bool hasFormatFor(QString key) const;
    QPair<QFont, QColor> getFormatFor(QString key) const;
    QPair<QFont, QColor> getDefaultFormat() const;

    void print(QString text) const;
    
    void adjustScale(qreal delta);

    static QTime time;
    ~DocumentScene();

    MainWindow *getWindow() const {return window;}

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void wheelEvent(QGraphicsSceneWheelEvent *event);
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
    void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);
    void dropEvent(QGraphicsSceneDragDropEvent *event);

private:
    MainWindow *window;
    QList<BlockManager*> groups;
    BlockManager *blockManager;

    QHash<QString, QPair<QFont, QColor> > highlighting;

    void adjustSceneRect();
    BlockManager* getBlockManager();
    bool toBool(QString textBool);
};

#endif // DOCUMENT_SCENE_H
