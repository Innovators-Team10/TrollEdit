#include "tree_element.h"
#include "block.h"
#include "doc_block.h"

const char *TreeElement::WHITE_EL = "whites";
const char *TreeElement::UNKNOWN_EL = "unknown";
const char *TreeElement::NEWLINE_EL = "nl";

TreeElement::TreeElement(QString type, bool selectable,
                         bool multiText, bool lineBreaking, bool paired)
{
    parent = 0;
    this->type = type;
    this->selectable = selectable;
    this->paragraphsAllowed = multiText;
    this->lineBreaking = lineBreaking;
    this->paired = paired;
    spaces = 0;
    myBlock = 0;
    pair = 0;
    floating = false;
}

TreeElement::~TreeElement()
{
    if (pair != 0) {
        pair->setPair(0);
        pair = 0;
    }
    if (!isLeaf())
        removeAllChildren();
    if (parent != 0) {
        if (!parent->isImportant())
            delete parent;
        else
            parent->removeChild(this);
    }
}

void TreeElement::setType(QString type)
{
    this->type = type;
}

void TreeElement::setBlock(Block *block)
{
    myBlock = block;
}

Block *TreeElement::getBlock() const
{
    return myBlock;
}

void TreeElement::setPair(TreeElement *pair)
{
    this->pair = pair;
}

TreeElement *TreeElement::getPair() const
{
    return pair;
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

void TreeElement::deleteAllChildren()
{
    qDeleteAll(children);
    children.clear();
}

bool TreeElement::isLeaf() const
{
    return !(children.count());
}
bool TreeElement::isImportant() const
{
    return childCount() != 1 || selectable || floating;
}
bool TreeElement::isNewline() const
{
    return (parent != 0 && parent->getType() == NEWLINE_EL);
}
bool TreeElement::isWhite() const
{
    return (parent != 0 && parent->getType() == WHITE_EL);
}
bool TreeElement::isUnknown() const
{
    return type == UNKNOWN_EL;
}
bool TreeElement::hasSiblings() const
{
    if (parent != 0)
        return parent->childCount() != 1;
    else
        return false;
}

void TreeElement::setSpaces(int number)
{
    spaces = qMax(0, number);
}
void TreeElement::addSpaces(int number)
{
    setSpaces(spaces + number);
}
int TreeElement::getSpaces() const
{
    return spaces;
}
void TreeElement::adjustSpaces(int offset)
{
    bool newLineComming = true;
    bool lb = false;
    offset += spaces;
    foreach (TreeElement *child, children) {
        lb = child->isLineBreaking();
        while (!child->isImportant()) {
            child->setLineBreaking(false);
            child = (*child)[0];
            lb = lb || child->isLineBreaking();
        }
        child->setLineBreaking(lb);
        if (newLineComming) {
            child->addSpaces(-offset);
            newLineComming = false;
        }
        child->adjustSpaces(offset);
        if (child->isLineBreaking())
            newLineComming = true;
    }
}

bool TreeElement::setLineBreaking(bool flag)
{
    if (lineBreaking == flag) return false;
    lineBreaking = flag;
    return true;
}

bool TreeElement::isLineBreaking() const
{
    return lineBreaking;
}

bool TreeElement::isFloating() const
{
    return floating;
}

void TreeElement::setFloating(bool floating)
{
    this->floating = floating;
}

bool TreeElement::isSelectable() const
{
    return selectable || floating;
}

bool TreeElement::allowsParagraphs() const
{
    if (parent != 0)
        return paragraphsAllowed || parent->allowsParagraphs();
    else
        return paragraphsAllowed;
}

bool TreeElement::isPaired() const
{
    return paired;
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

int TreeElement::indexOfBranch(const TreeElement *desc) const
{
    int i = indexOfChild(desc);
    if (i > -1) {
        return i;
    } else {
        for (i = 0; i < children.size(); i++) {
            if (children[i]->indexOfBranch(desc) > -1)
                return i;
        }
    }
    return -1;
}

QList<TreeElement*> TreeElement::getChildren() const
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

QList<TreeElement*> TreeElement::getAllLeafs() const
{
    QList<TreeElement*> list;
    foreach (TreeElement *child, children) {
        if (child->isLeaf())
            list << child;
        else
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
    DocBlock *docBl = 0;
    if (isFloating()) {
        docBl = qgraphicsitem_cast<DocBlock*>(myBlock);
    }

    QString text;
    QString spacesStr = QString().fill(' ', spaces);
    if (isLeaf()) {
        text = type;                            // get my text
        if (docBl != 0)
            text = docBl->convertToText();      // get text of docblock
    } else {
        if (docBl != 0)
            text.append(docBl->convertToText());// get text of docblock
        foreach (TreeElement *e, children) {
            text.append(e->getText());          // get child texts
        }
        text.replace("\n", "\n"+spacesStr);     // indent after each line break
    }
    text.prepend(spacesStr);                    // indent my text
    if (lineBreaking) {
        text.append("\n");                      // add line break if needed
    }
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
    return parent->hasNext(this->index() + 1);
}

TreeElement *TreeElement::next(int index)
{
    if (index < childCount())
        return children.at(index);
    if (parent == 0) return 0;
    return parent->next(this->index() + 1);
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

TreeElement *TreeElement::clone() const
{
    TreeElement *el = new TreeElement(type, selectable, paragraphsAllowed,
                                     lineBreaking, paired);
    el->parent = 0;
    el->spaces = spaces;
    el->pair = pair;
    el->floating = floating;
    foreach(TreeElement *child, children) {
        el->appendChild(child->clone());
    }
    return el;
}
