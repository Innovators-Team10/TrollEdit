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
    QGraphicsView *graphicsView;
    QLabel *label_2;

public slots:
    void showWindow();

private slots:
    void on_commandLinkButton_clicked();
    void on_commandLinkButton_2_clicked();
};

#endif // TIPS_TRICKS_H
