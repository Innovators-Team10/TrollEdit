#ifndef BOTTOMDOCK_H
#define BOTTOMDOCK_H

#include <QWidget>

namespace Ui {
class bottomDock;
}

class bottomDock : public QWidget
{
    Q_OBJECT
    
public:
    explicit bottomDock(QWidget *parent = 0);
    ~bottomDock();
    
private:
    Ui::bottomDock *ui;
};

#endif // BOTTOMDOCK_H
