#include "paired_tree_element.h"

PairedTreeElement::PairedTreeElement(QString type)
    : TreeElement(type)
{
    pair = NULL;
}

PairedTreeElement::PairedTreeElement(TreeElement *source)
    : TreeElement(source->getType())
{
    // set children
    this->appendChildren(source->getChildren());
    // set parent
    TreeElement * parent = source->getParent();
    int index = parent->indexOfChild(source);
    parent->removeChild(source);
    parent->insertChild(index, this);

    pair = NULL;
    // cleanup
    source->removeAllChildren();
    delete source;
}

void PairedTreeElement::setPair(PairedTreeElement *element)
{
    pair = element;
}
