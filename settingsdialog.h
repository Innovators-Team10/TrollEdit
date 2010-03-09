#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

class QListWidget;
class QListWidgetItem;
class QStackedWidget;
class QPushButton;
class MainWindow;
class QSettings;

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    SettingsDialog(MainWindow *win, const QSettings &settings);
    QStackedWidget *pagesWidget;

public slots:
    void changePage(QListWidgetItem *current, QListWidgetItem *previous);

private:
    QListWidget *contentsWidget;
    QPushButton *applyButton;
    QPushButton *okButton;
    QPushButton *cancelButton;
};

#endif // SETTINGSDIALOG_H
