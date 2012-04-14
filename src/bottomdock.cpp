#include "bottomdock.h"
#include "ui_bottomdock.h"

bottomDock::bottomDock(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::bottomDock)
{
    ui->setupUi(this);
}

bottomDock::~bottomDock()
{
    delete ui;
}
