#ifndef SETTING_H
#define SETTING_H

#include <QWidget>

namespace Ui {
class setting;
}

class setting : public QWidget
{
    Q_OBJECT
    
public:
    explicit setting(QWidget *parent = 0);
    ~setting();
    
private:
    Ui::setting *ui;
};

#endif // SETTING_H
