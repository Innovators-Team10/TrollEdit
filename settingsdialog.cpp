#include "settingsdialog.h"
#include "settingspages.h"
#include "mainwindow.h"
#include <QtGui>

SettingsDialog::SettingsDialog(MainWindow *win, const QSettings &settings)
 {
     contentsWidget = new QListWidget;
     contentsWidget->setMovement(QListView::Static);
     contentsWidget->setMaximumWidth(128);

     QListWidgetItem *brushButton = new QListWidgetItem(contentsWidget);
     brushButton->setText(tr("Brush"));
     brushButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

     QListWidgetItem *penButton = new QListWidgetItem(contentsWidget);
     penButton->setText(tr("Pen"));
     penButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

     pagesWidget = new QStackedWidget;
     pagesWidget->addWidget(new BrushPage(settings));
     pagesWidget->addWidget(new PenPage(settings));

     okButton = new QPushButton(tr("&Ok"));
     okButton->setDefault(true);
     cancelButton = new QPushButton(tr("&Cancel"));
     applyButton = new QPushButton(tr("&Apply"));

     connect(contentsWidget, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
        this, SLOT(changePage(QListWidgetItem *, QListWidgetItem*)));
     contentsWidget->setCurrentRow(0);

     connect(okButton, SIGNAL(clicked()), this, SLOT(close()));
     connect(okButton, SIGNAL(clicked()), win, SLOT(applyChanges()));
     connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));
     connect(applyButton, SIGNAL(clicked()), win, SLOT(applyChanges()));

     QHBoxLayout *horizontalLayout = new QHBoxLayout;
     horizontalLayout->addWidget(contentsWidget);
     horizontalLayout->addWidget(pagesWidget, 1);

     QHBoxLayout *buttonLayout = new QHBoxLayout;
     buttonLayout->addStretch();
     buttonLayout->addWidget(okButton);
     buttonLayout->addWidget(cancelButton);
     buttonLayout->addWidget(applyButton);

     QVBoxLayout *mainLayout = new QVBoxLayout;
     mainLayout->addLayout(horizontalLayout);
     mainLayout->addLayout(buttonLayout);
     setLayout(mainLayout);

     setWindowTitle(tr("Block Style Settings"));
     setMinimumSize(450, 200);
 }

void SettingsDialog::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current)
        current = previous;

    pagesWidget->setCurrentIndex(contentsWidget->row(current));
}
