/**
 * document_tabs.h
 *  ---------------------------------------------------------------------------
 * Contains the declaration of class DocumentTabs and it's funtions and identifiers
 *
 */
#ifndef DOCUMENTTABS_H
#define DOCUMENTTABS_H

#include <QTabWidget>

class DocumentTabs : public QTabWidget
{
    Q_OBJECT

public:
    DocumentTabs(QWidget *parent = 0);
    void resizeEvent(QResizeEvent *event);

    static int documentNumber;

signals:
    void adjustScenes(QRectF rect);

public slots:
    void provideSize();

protected:
    void mousePressEvent(QMouseEvent *event);

private slots:
    void closeTab(int index);
};

#endif // DOCUMENTTABS_H
