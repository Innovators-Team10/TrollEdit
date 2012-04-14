#include "abouttrolledit.h"
#include "ui_abouttrolledit.h"

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
    this->show();
}


void aboutTrollEdit::loadVersionInfo()
{
    //ešte dokonèit treba

    /*aboutVersionWindow = new QDialog();
    aboutVersionLabel= new QLabel(aboutVersionWindow);
    aboutVersionLabel->setPixmap(QPixmap(":/img/logoSmall"));
    aboutVersionTextEdit = new QTextEdit(aboutVersionWindow);

    outer = new QVBoxLayout();
    outer->addWidget(aboutVersionLabel);
    outer->addWidget(aboutVersionTextEdit);
    aboutVersionWindow->setLayout(outer);

    // open file
    QFile sfile(":/files/aboutThisVersion");
    if(sfile.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream in(&sfile);
        QString text= in.readAll();
        sfile.close();
        aboutVersionTextEdit->setPlainText(text);
        aboutVersionTextEdit->setReadOnly(true);
    }

    aboutVersionWindow ->setWindowTitle("About this version");
    aboutVersionWindow->resize(510,350);
    aboutVersionWindow ->show();*/
}
