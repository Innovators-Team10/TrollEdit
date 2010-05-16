#ifndef DOCUMENT_SCENE_H
#define DOCUMENT_SCENE_H

#include <QGraphicsScene>
#include <QHash>
#include <QString>
#include <QUrl>

class Analyzer;
class DocumentTabs;
class BlockGroup;

class DocumentScene : public QGraphicsScene
{
    Q_OBJECT

public:
    DocumentScene(QWidget *parent);
    void update(const QRectF &rect = QRectF());
    bool isModified() const {return modified;}
    
    void newGroup(Analyzer *defaultAnalyzer);
    void loadGroup(const QString &fileName, Analyzer *analyzer);
    void saveGroup(const QString &fileName);
    void closeGroup();

    void setHighlightning(const QHash<QString, QPair<QFont, QColor> > &highlightning);
    QHash<QString, QPair<QFont, QColor> > getHighlightning() const;
    void setBlockFormatting(const QHash<QString, QHash<QString, QColor> > &blockFormats);
    QHash<QString, QHash<QString, QColor> > getBlockFormatting() const;

    void print(QString text) const;

signals:
    void requestSize();

private slots:
    void adjustSceneRect(QRectF rect);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void wheelEvent(QGraphicsSceneWheelEvent *event);
    void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
    void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);
    void dropEvent(QGraphicsSceneDragDropEvent *event);

private:
    bool modified;
    QList<BlockGroup*> groups;
    BlockGroup *currentGroup;

    QHash<QString, QPair<QFont, QColor> > highlightning;
    QHash<QString, QHash<QString, QColor> > blockFormats;
};

#endif // DOCUMENT_SCENE_H
