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

public slots:
    void newGroup(Analyzer *defaultAnalyzer);
    void loadGroup(const QString &fileName, Analyzer *analyzer);
    void saveGroup(const QString &fileName = "", BlockGroup *group = 0);
    void saveGroupAs(BlockGroup *group = 0);
    void saveAllGroups();
    void closeGroup(BlockGroup *group = 0);
    void closeAllGroups();

public:
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

    static QTime time;

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
    bool toBool(QString textBool);
};

#endif // DOCUMENT_SCENE_H
