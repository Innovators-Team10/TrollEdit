#include <QtGui>
#include "helpbrowser.h"

HelpBrowser::HelpBrowser(const QString &path, const QString &page, QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_GroupLeader);

    textBrowser = new QTextBrowser;
    listWidget = new QListWidget;
    homeButton = new QPushButton(tr("&Home"));
    backButton = new QPushButton(tr("&Back"));
    forwardButton = new QPushButton(tr("&Forward"));
    closeButton = new QPushButton(tr("Close"));
    closeButton->setShortcut(tr("Esc"));

    QDir dir = QDir("doc");
    QStringList files = dir.entryList(QStringList("*.html"), QDir::Files | QDir::NoSymLinks);
    textBrowser->setSearchPaths(QStringList() << path << ":/res");
    foreach (QString file, files) {
        textBrowser->setSource(file);
        listWidget->addItem(textBrowser->documentTitle());
        pages.append(file);
    }
    connect(listWidget, SIGNAL(itemSelectionChanged()), this, SLOT(updatePage()));

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(homeButton);
    buttonLayout->addWidget(backButton);
    buttonLayout->addWidget(forwardButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(closeButton);

    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->addLayout(buttonLayout);
    rightLayout->addWidget(textBrowser);


    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(listWidget);
    mainLayout->addLayout(rightLayout);
    setLayout(mainLayout);

    connect(homeButton, SIGNAL(clicked()), textBrowser, SLOT(home()));
    connect(backButton, SIGNAL(clicked()), textBrowser, SLOT(backward()));
    connect(forwardButton, SIGNAL(clicked()), textBrowser, SLOT(forward()));
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(textBrowser, SIGNAL(sourceChanged(const QUrl &)), this, SLOT(updateWindowTitle()));

    textBrowser->setSource(page);
}

void HelpBrowser::updateWindowTitle()
{
    setWindowTitle(tr("Help: %1").arg(textBrowser->documentTitle()));
}

void HelpBrowser::updatePage()
{
    textBrowser->setSource(pages.at(listWidget->currentIndex().row()));
}

void HelpBrowser::showPage(const QString &page)
{
    QString path = QApplication::applicationDirPath() + "/doc";
    HelpBrowser *browser = new HelpBrowser(path, page);
    browser->resize(500, 400);
    browser->show();
}
