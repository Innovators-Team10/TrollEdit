#ifndef DOCUMENTSCENE_H
#define DOCUMENTSCENE_H

#include <QGraphicsScene>
#include <QHash>
#include <QString>
#include <QUrl>

class Analyzer;
class Block;
class DocumentTabs;

class DocumentScene : public QGraphicsScene
{
    Q_OBJECT

public:
    DocumentScene(Analyzer *analyzer, QObject *parent = 0);
    void update(const QRectF &rect = QRectF());
    
    void loadFile(const QString &fileName);
    void saveFile(const QString &fileName);
    void showInsertLine(QLineF line);
    void hideInsertLine();

    Analyzer *analyzer;
    bool modified;

    bool analyzeAll(QString text);
    void reanalyze();
    bool reanalyze(Block* block);
    void addDocBlock(QPointF pos);

    void setHighlightning(const QHash<QString, QPair<QFont, QColor> > &highlightning);
    QHash<QString, QPair<QFont, QColor> > getHighlightning() const;
    void setBlockFormatting(const QHash<QString, QHash<QString, QColor> > &blockFormats);
    QHash<QString, QHash<QString, QColor> > getBlockFormatting() const;

    Block *mainBlock;

signals:
    void requestSize();

public slots:
    void adjustSceneRect(QRectF rect);


protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

    void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    void dropEvent(QGraphicsSceneDragDropEvent *event);
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
    void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);

protected:

private slots:
    void animationFinished();

private:

    QHash<QString, QPair<QFont, QColor> > highlightning;
    QHash<QString, QHash<QString, QColor> > blockFormats;
    QGraphicsLineItem *insertLine;

    void dropImage(const QImage &image, QGraphicsSceneDragDropEvent *event);
    void dropFile(QUrl url, QGraphicsSceneDragDropEvent *event);
    Block* blockAt(QPointF pos) const;
};

#endif // DOCUMENTSCENE_H
