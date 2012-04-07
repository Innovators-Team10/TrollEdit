#ifndef QLIST_AST_H
#define QLIST_AST_H

#include <QObject>
#include <QList>
#include "tree_element.h"
//template<class T>
class TreeElement;

//class QListAST : public QObject, public QList<T>
class QListAST : public QObject, public QList<TreeElement*>
{
    Q_OBJECT

public:
    QListAST();
    TreeElement* operator[](int i);
//    ~QListAST();

};

#endif // QLIST_AST_H
