#ifndef RIGHTDOCK_H
#define RIGHTDOCK_H

#include <QWidget>

namespace Ui {
class rightDock;
}

class rightDock : public QWidget
{
    Q_OBJECT
    
public:
    explicit rightDock(QWidget *parent = 0);
    ~rightDock();
    
private:
    Ui::rightDock *ui;
};

#endif // RIGHTDOCK_H
