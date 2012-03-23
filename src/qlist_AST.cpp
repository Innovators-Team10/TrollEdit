#include "qlist_AST.h"

#include <QDebug>
//class TreeElement;

QListAST::QListAST() : QList<TreeElement*>()
{
    qDebug() << "QListAST() argh!!";
//    (reinterpret_cast< QList<TreeElement*> &>(*this)());
}

TreeElement* QListAST::operator[](int i)// : QList<TreeElement*>::operator []()
{
    qDebug() << "QListAST operator[]";
    ((TreeElement*)(this->at(i)))->analyzer->resetAST();
//    return ((QList<TreeElement*>)*(this))[i];
}

//QListAST::~QListAST() : ~QList<TreeElement*>()
//{
//    qDebug() << "argh!!";
//}
