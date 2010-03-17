#ifndef DOCUMENTTABS_H
#define DOCUMENTTABS_H

#include <QTabWidget>

#include "mainwindow.h"

class DocumentTabs : public QTabWidget
{
    Q_OBJECT

public:
    DocumentTabs(QWidget *parent = 0);

    static int documentNumber;

protected:
    void mousePressEvent(QMouseEvent *event);

private slots:
    void closeTab(int index);
};

#endif // DOCUMENTTABS_H
