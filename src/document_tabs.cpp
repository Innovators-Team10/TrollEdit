/** 
* @file fold_button.cpp
* @author Team 04 Ufopak + Team 10 Innovators
* @version 
* 
* @section DESCRIPTION
* Contains the defintion of class DocumentTabs and it's functions and identifiers
*/

#include "document_tabs.h"
#include "main_window.h"

int DocumentTabs::documentNumber;

DocumentTabs::DocumentTabs(QWidget *parent)
        : QTabWidget(parent)
{
    documentNumber = 1;
    MainWindow *win = qobject_cast<MainWindow*>(parent);
    connect(this, SIGNAL(currentChanged(int)), win, SLOT(setCurrentFile(int)));

    // this makes tabs to have close button
    setTabsClosable(true);

    // connect it to some close action, otherwise close button does nothing
    QTabBar *tabs = tabBar();
    connect(tabs, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
}

void DocumentTabs::resizeEvent(QResizeEvent *event)
{
    QTabWidget::resizeEvent(event);
    provideSize();
}

void DocumentTabs::provideSize()
{
    emit adjustScenes(rect());
}

void DocumentTabs::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MidButton)
    {
        QTabBar *tabs = tabBar();
        removeTab(tabs->tabAt(event->pos()));
    }

    QTabWidget::mousePressEvent(event);
}

void DocumentTabs::closeTab(int index)
{
    removeTab(index);
}
