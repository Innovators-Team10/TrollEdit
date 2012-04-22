#ifndef TIPS_TRICKS_H
#define TIPS_TRICKS_H

#include <QWidget>
#include <QtGui>

namespace Ui {
class tips_tricks;
}

class tips_tricks : public QWidget
{
    Q_OBJECT
    
public:
    explicit tips_tricks(QWidget *parent = 0);
    ~tips_tricks();
    
private:
    Ui::tips_tricks *ui;


public slots:
    void showWindow();

private slots:
};

#endif // TIPS_TRICKS_H
