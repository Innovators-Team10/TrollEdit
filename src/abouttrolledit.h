#ifndef ABOUTTROLLEDIT_H
#define ABOUTTROLLEDIT_H

#include <QWidget>
#include <QtGui>

namespace Ui {
class aboutTrollEdit;
}

class aboutTrollEdit : public QWidget
{
    Q_OBJECT
    
public:
    explicit aboutTrollEdit(QWidget *parent = 0);
    ~aboutTrollEdit();
    
private:
    Ui::aboutTrollEdit *ui;

    QTextEdit  *textEdit_5;

public slots:
    void showWindow();
    void loadVersionInfo();

private slots:

};

#endif // ABOUTTROLLEDIT_H
