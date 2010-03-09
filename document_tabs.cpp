#include "document_tabs.h"

int DocumentTabs::documentNumber;

DocumentTabs::DocumentTabs(QWidget *parent)
        : QTabWidget(parent)
{
    documentNumber = 1;
    MainWindow *win = qobject_cast<MainWindow*>(parent);
    connect(this, SIGNAL(currentChanged(int)), win, SLOT(setCurrentFile(int)));
}
// zeby anonymna trieda??
