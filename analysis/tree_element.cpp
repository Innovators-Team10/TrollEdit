#include "tree_element.h"

const char *TreeElement::WHITE_EL = "whites";
const char *TreeElement::UNKNOWN_EL = "unknown";

TreeElement::TreeElement(QString type)
{
    parent = 0;
    this->type = type;
    lineBreaking = false;
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

bool TreeElement::isLeaf() const
{
    return !(children.count());
}

bool TreeElement::isImportant() const
{
    return childCount() != 1;
}

bool TreeElement::isMultiLine() const
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
bool TreeElement::isNewline() const
{
    return type.contains("\n");
}
bool TreeElement::setLineBreaking(bool flag)
{
    if (!isImportant())
        return children[0]->setLineBreaking(flag);
    if (lineBreaking == flag) return false;
    lineBreaking = flag;
    return true;
}
bool TreeElement::isLineBreaking() const
{
    return lineBreaking;
}
bool TreeElement::isWhite() const
{
    return (parent != 0 && parent->getType() == WHITE_EL);
}
bool TreeElement::isUnknown() const
{
    return (parent != 0 && parent->getType() == UNKNOWN_EL);
}
bool TreeElement::hasSiblings() const
{
    if (parent != 0)
        return parent->childCount() != 1;
    else
        return false;
}

int TreeElement::childCount() const
{
    return children.count();
}

int TreeElement::index() const
{
    if (parent == 0)
        return -1;
    else
        return parent->indexOfChild(this);
}

int TreeElement::indexOfChild(const TreeElement *child) const
{
    int p = children.indexOf(const_cast<TreeElement*>(child), 0);
    return p;
}

int TreeElement::indexOfDescendant(const TreeElement *desc) const
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

QList<TreeElement *> TreeElement::getChildren() const
{
    return children;
}

QList<TreeElement*> TreeElement::getAncestors() const
{
    QList<TreeElement*> list;
    TreeElement *e = parent;
    while (e != 0) {
        list << e;
        e = e->getParent();
    }
    return list;
}

QList<TreeElement*> TreeElement::getDescendants() const
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
    if (parent == 0)
        return this;
    else
        return parent->getRoot();
}

TreeElement *TreeElement::getParent() const
{
    return parent;
}

QString TreeElement::getType() const
{
    return type;
}

// returns all text in this element and it's descendants
QString TreeElement::getText() const
{
    QString text;
    if (isLeaf())
        text = type;
    else {
        foreach (TreeElement *e, children) {
            text.append(e->getText());
        }
    }
    if (lineBreaking) text.append("\n");
    return text;
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
    return parent->hasNext(parent->indexOfChild(this) + 1);
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
