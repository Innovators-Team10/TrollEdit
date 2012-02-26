#ifndef QLIST_AST_H
#define QLIST_AST_H

#include <QObject>
#include <QList>
class TreeElement;

class QListAST : public QObject, public QList<TreeElement*>
{
    Q_OBJECT

public:
    QListAST();
    TreeElement* operator[](int i);
//    ~QListAST();

};

#endif // QLIST_AST_H
