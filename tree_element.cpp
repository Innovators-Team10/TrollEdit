#include "tree_element.h"

TreeElement::TreeElement(QString type)
{
    parent = 0;
    this->type = type;
}

TreeElement::~TreeElement()
{
    qDeleteAll(children);
    if (parent != 0) {
        parent->removeChild(this);
    }
}

void TreeElement::setType(QString type)
{
    this->type = type;
}

void TreeElement::appendChild(TreeElement *child)
{
    children.append(child);
    child->parent = this;
}

void TreeElement::appendChildren(QList<TreeElement*> children)
{
    foreach (TreeElement *child, children) {
        appendChild(child);
    }
}

void TreeElement::insertChild(int index, TreeElement *child)
{
    children.insert(index,child);
    child->parent = this;
}

void TreeElement::insertChildren(int index, QList<TreeElement*> children)
{
    for (int i = 0; i < children.size(); i++) {
        insertChild(index+i, children.at(i));
    }
}

bool TreeElement::removeChild(TreeElement *child)
{
    child->parent = 0;
    return children.removeOne(child);
}

bool TreeElement::deleteBranchTo(TreeElement *desc)
{
    if (!desc->getAncestors().contains(this))
        return false;

    TreeElement *temp = 0;
    TreeElement *el = desc->parent;
    el->removeChild(desc);

    while(el != this) {
        temp = el->parent;
        delete(el);
        el = temp;
    }
    return true;
}

bool TreeElement::removeDescendant(TreeElement *desc) { // not used?
    if (removeChild(desc)) {
        return true;
    } else {
        foreach (TreeElement *el, children) {
            if (el->removeDescendant(desc))
                return true;
        }
    }
    return false;
}

bool TreeElement::removeAllChildren()
{
    if (children.isEmpty())
        return false;
    foreach (TreeElement *child, children)
        removeChild(child);
    return true;
}

bool TreeElement::isLeaf()
{
    return !(children.count());
}

bool TreeElement::isImportant()
{
    return childCount() != 1;
}

bool TreeElement::isMultiLine()
{
    if (isLeaf()) {
        return type.contains("\n");
    }else {
        foreach (TreeElement *el, children) {
            if (el->isMultiLine())
                return true;
        }
        return false;
    }
}

bool TreeElement::hasSiblings()
{
    if (parent != 0)
        return parent->childCount() != 1;
    else
        return false;
}

int TreeElement::childCount()
{
    return children.count();
}

int TreeElement::index()
{
    if (parent == 0)
        return -1;
    else
        return parent->indexOfChild(this);
}

int TreeElement::indexOfChild(TreeElement *child)
{
    return children.indexOf(child, 0);
}

int TreeElement::indexOfDescendant(TreeElement *desc)
{
    int i = indexOfChild(desc);
    if (i > -1) {
        return i;
    } else {
        for (i = 0; i < children.size(); i++) {
            if (children[i]->indexOfDescendant(desc) > -1)
                return i;
        }
    }
    return -1;
}

QList<TreeElement *> TreeElement::getChildren()
{
    return children;
}

QList<TreeElement*> TreeElement::getAncestors()
{
    QList<TreeElement*> list;
    TreeElement *e = this;
    while ((e = e->getParent()) != 0) {
        list << e;
    }
    return list;
}

QList<TreeElement*> TreeElement::getDescendants()
{
    QList<TreeElement*> list;
    foreach (TreeElement *child, children) {
        list << child;
        list << child->getDescendants();
    }
    return list;
}

TreeElement *TreeElement::getRoot()
{
    if (parent == 0) return this;
    else return parent->getRoot();
}

TreeElement *TreeElement::getParent()
{
    return parent;
}

QString TreeElement::getType()
{
    return type;
}

// returns all text in this element and it's descendants
QString TreeElement::getText()
{
    if (isLeaf())
        return type;
    else {
        QString text;
        foreach (TreeElement *e, children) {
            text.append(e->getText());
        }
        return text;
    }
}

// iterator methods
bool TreeElement::hasNext()
{
    return hasNext(0);
}

TreeElement *TreeElement::next()
{
    return next(0);
}

bool TreeElement::hasNext(int index)
{
    if (index < childCount()) return true;
    if (parent == 0) return false;
    return parent->hasNext((*parent)[this] + 1);
}

TreeElement *TreeElement::next(int index)
{
    if (index < childCount()) return children.at(index);
    if (parent == 0) return 0;
    return parent->next((*parent)[this] + 1);
}

// operators
TreeElement *TreeElement::operator<<(TreeElement *child)
{
    appendChild(child);
    return this;
}
TreeElement *TreeElement::operator<<(QList<TreeElement *> children)
{
    appendChildren(children);
    return this;
}
TreeElement *TreeElement::operator[](int index)
{
    return children[index];
}
int TreeElement::operator[](TreeElement* child)
{
    return indexOfChild(child);
}
