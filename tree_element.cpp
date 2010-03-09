#include "tree_element.h"

TreeElement::TreeElement(QString type) {
    parent = 0;
    this->type = type;
}

TreeElement::~TreeElement() {
    qDeleteAll(children);
}

void TreeElement::setType(QString type)
{
    this->type = type;
}

void TreeElement::appendChild(TreeElement *child) {
    children.append(child);
    child->parent = this;
}

void TreeElement::appendChildren(QList<TreeElement *> children) {
    foreach (TreeElement *child, children) {
        appendChild(child);
    }
}

void TreeElement::insertChild(int index, TreeElement *child) {
    children.insert(index,child);
    child->parent = this;
}

void TreeElement::insertChildren(int index, QList<TreeElement *> children) {
    for (int i=0; i<children.size(); i++) {
        insertChild(index+i, children.at(i));
    }
}

void TreeElement::removeChild(TreeElement *child){
    children.removeOne(child);
}

void TreeElement::removeAllChildren() {
    children.clear();
}

bool TreeElement::isLeaf() {
    return !(children.count());
}

bool TreeElement::hasSiblings()
{
    if (parent != NULL)
        return parent->childCount() != 1;
    else
        return false;
}

int TreeElement::childCount() {
    return children.count();
}

int TreeElement::indexOfChild(TreeElement* child) {
    return children.indexOf(child, 0);
}

QList<TreeElement *> TreeElement::getChildren() {
    return children;
}

QList<TreeElement*> TreeElement::getAncestors()
{
    QList<TreeElement*> list;
    TreeElement *e = this;
    while ((e = e->getParent()) != NULL) {
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
    if (parent == NULL) return this;
    else return parent->getRoot();
}

TreeElement *TreeElement::getParent()
{
    return parent;
}

QString TreeElement::getType() {
    return type;
}

// returns all text in this element and it's descendants
QString TreeElement::getText() {
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
    if (parent == NULL) return false;
    return parent->hasNext((*parent)[this] + 1);
}

TreeElement *TreeElement::next(int index)
{
    if (index < childCount()) return children.at(index);
    if (parent == NULL) return NULL;
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
