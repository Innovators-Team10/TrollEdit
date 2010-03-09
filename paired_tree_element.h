#ifndef PAIRED_TREE_ELEMENT_H
#define PAIRED_TREE_ELEMENT_H

#include <QList>
#include <QString>
#include <QTreeWidgetItem>
#include "tree_element.h"

class PairedTreeElement : public TreeElement
{
public:
    PairedTreeElement(QString type);
    PairedTreeElement(TreeElement *source);
    void setPair(PairedTreeElement *element);
    PairedTreeElement *getPair();

    void select();

private:
    PairedTreeElement *pair;
};

#endif // PAIRED_TREE_ELEMENT_H
