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
    loadVersionInfo();
}

//! load conf. file for info this version
void aboutTrollEdit::loadVersionInfo()
{
    QFile sfile(":/files/aboutThisVersion");
    if(sfile.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream in(&sfile);
        QString text= in.readAll();
        sfile.close();
        ui->textEdit_5->setPlainText(text);
    }


} //! END of aboutTrollEDit
