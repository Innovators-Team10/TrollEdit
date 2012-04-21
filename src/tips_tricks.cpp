#include "tips_tricks.h"
#include "ui_tips_tricks.h"
#include <QtGui>

tips_tricks::tips_tricks(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::tips_tricks)
{
    ui->setupUi(this);
}

tips_tricks::~tips_tricks()
{
    delete ui;
}

void tips_tricks::showWindow()
{
    this->setFixedSize(400,400);
    this->show();
}

//! load conf. file for tips and tricks
