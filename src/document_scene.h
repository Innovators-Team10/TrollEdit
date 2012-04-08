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
class BlockGroup;
class MainWindow;

class DocumentScene : public QGraphicsScene
{
    Q_OBJECT

signals:
    void modified(bool flag);
    void fileSelected(BlockGroup *group);

public slots:
    void newGroup(QString extension);
    void loadGroup(QString fileName, QString extension);
    void revertGroup(BlockGroup *group = 0);
    void saveGroup(QString fileName = "", BlockGroup *group = 0, bool noDocs = false);
    void saveGroupAs(BlockGroup *group = 0);
    void saveAllGroups();
    void saveGroupAsWithoutDoc(BlockGroup *group = 0);
    void closeGroup(BlockGroup *group = 0);
    void closeAllGroups();
    void setGroupLang(Analyzer *newAnalyzer, BlockGroup *group = 0);
    void showPreview(BlockGroup *group = 0);
    void findText(QString searchStr, BlockGroup *group = 0);
    void cleanGroup(BlockGroup *group = 0);

public:
    MainWindow *main;
    DocumentScene(MainWindow *parent);
    void update(const QRectF &rect = QRectF());
    void selectGroup(BlockGroup *group = 0);
    BlockGroup *selectedGroup() const {return currentGroup;}
    void groupWasModified(BlockGroup *group);

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
    QList<BlockGroup*> groups;
    BlockGroup *currentGroup;

    QHash<QString, QPair<QFont, QColor> > highlighting;

    void adjustSceneRect();
    BlockGroup* getBlockGroup();
    bool toBool(QString textBool);
};

#endif // DOCUMENT_SCENE_H
