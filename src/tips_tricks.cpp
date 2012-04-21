#include "tips_tricks.h"
#include "ui_tips_tricks.h"

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
