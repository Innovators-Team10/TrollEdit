#include "qlist_AST.h"

#include <QDebug>
class TreeElement;

QListAST::QListAST() : QList<TreeElement*>()
{
    qDebug() << "argh!!";
    //((QList<TreeElement*>*)this)();
}

TreeElement* QListAST::operator[](int i)
{
    qDebug() << "operator[]";
//    ((TreeElement*)(this->at(i)))->analyzer->resetAST();
//    ((TreeElement*)(this->at(i)))->analyzer->setIndexAST(((TreeElement*)(this->at(i)))->local_index);
    return ((QList<TreeElement*>)*(this))[i];
}

//QListAST::~QListAST() : ~QList<TreeElement*>()
//{
//    qDebug() << "argh!!";
//}
