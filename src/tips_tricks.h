#ifndef TIPS_TRICKS_H
#define TIPS_TRICKS_H

#include <QWidget>

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
};

#endif // TIPS_TRICKS_H
