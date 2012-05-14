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
    this->setFixedSize(645,325);
    this->show();
}

//! button next
void tips_tricks::on_commandLinkButton_clicked()
{
    QMessageBox::information(this,"title","On Function is working!");
}

//! button back
void tips_tricks::on_commandLinkButton_2_clicked()
{

}
