#include "abouttrolledit.h"
#include "ui_abouttrolledit.h"
#include <QtGui>

aboutTrollEdit::aboutTrollEdit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::aboutTrollEdit)
{
    ui->setupUi(this);
}

aboutTrollEdit::~aboutTrollEdit()
{
    delete ui;
}

void aboutTrollEdit::showWindow()
{
    this->setFixedSize(758,479);
    this->show();
}

//! END of aboutTrollEDit
