#include "rightdock.h"
#include "ui_rightdock.h"

rightDock::rightDock(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::rightDock)
{
    ui->setupUi(this);
}

rightDock::~rightDock()
{
    delete ui;
}
