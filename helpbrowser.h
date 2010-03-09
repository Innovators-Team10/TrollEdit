#include <QWidget>

class QPushButton;
class QTextBrowser;
class QListWidget;
class QStringList;

class HelpBrowser : public QWidget
{
    Q_OBJECT

public:
    HelpBrowser(const QString &path, const QString &page, QWidget *parent = 0);
    static void showPage(const QString &page);

private slots:
    void updateWindowTitle();
    void updatePage();

private:
    QTextBrowser *textBrowser;
    QListWidget *listWidget;
    QStringList pages;
    QPushButton *homeButton;
    QPushButton *backButton;
    QPushButton *forwardButton;
    QPushButton *closeButton;
};
