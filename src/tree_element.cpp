/** 
* @file tree_element.cpp
* @author Team 04 Ufopak + Team 10 Innovators
* @version 
* 
* @section DESCRIPTION
* Contains the defintion of class TreeElement.
*/

#include "tree_element.h"
#include "block.h"
#include "doc_block.h"


const char *TreeElement::WHITE_EL = "whites";
const char *TreeElement::UNKNOWN_EL = "unknown";
const char *TreeElement::NEWLINE_EL = "nl";
const bool TreeElement::DYNAMIC = false;        //! dynamicke spracovanie AST - now work with nodes and deep

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

    analyzer = 0;
}

TreeElement::~TreeElement()
{
    if(DYNAMIC){
                                            //! todo dopracuj zmazanie pri dynamickom spracovani
    }else{
    if (pair != 0)
    {
        pair->setPair(0);
        pair = 0;
    }

    if (!isLeaf())
        removeAllChildren();

    if (getParent() != 0)
    {
        if (!getParent()->isImportant())
            delete parent;
            //delete getParent();             //! todo otestuj mazanie
        else
            getParent()->removeChild(this);
    }
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
    if(DYNAMIC){
        return pair;
    }else{
    return pair;
    }
}

void TreeElement::appendChild(TreeElement *child)
{
    children.append(child);
    child->parent = this;                           //! prerob cez funkciu napriklad setParent(this)
}

void TreeElement::appendChildren(QList<TreeElement*> children)
{
    foreach (TreeElement *child, children)
    {
        appendChild(child);
        child->parent = this;
    }
}

void TreeElement::insertChild(int index, TreeElement *child)
{
    children.insert(index, child);                 //! prerob aby fungovalo cez funkciu
    child->parent = this;                          //! prerob cez funkciu napriklad setParent(this)
}

void TreeElement::insertChildren(int index, QList<TreeElement*> children)
{
    for (int i = 0; i < children.size(); i++)
    {
        insertChild(index+i, children.at(i));
    }
}

bool TreeElement::removeChild(TreeElement *child)
{
    child->parent = 0;                            //! prerob cez funkciu napriklad setParent(this)
    return children.removeOne(child);
}

bool TreeElement::removeDescendant(TreeElement *desc) { //! not used?
    if (removeChild(desc))
    {
        return true;
    }
    else
    {
        foreach (TreeElement *el, getChildren())
        {
            if (el->removeDescendant(desc)) return true;

        }
    }

    return false;
}

bool TreeElement::removeAllChildren()           //! todo otestuj mazanie
{
    if (getChildren().isEmpty()) return false;

    foreach (TreeElement *child, getChildren())
        removeChild(child);

    return true;
}

void TreeElement::deleteAllChildren()           //! todo otestuj mazanie
{
    if(DYNAMIC){
        qDebug() << "deleteAllChildren()";
    }else{
    qDeleteAll(getChildren());
    getChildren().clear();
    }
}

bool TreeElement::isLeaf() const
{
    if(DYNAMIC){
        if( this->analyzer != 0 ){
            this->analyzer->checkLocationAST(this->local_deep_AST,this->local_nodes_AST);

            return this->analyzer->isLeafElementAST();  //! new iterator
        }else{
            return true;
        }
    }else{
        return !(children.count());
    }
}
bool TreeElement::isImportant() const
{
    return childCount() != 1 || selectable || floating;
}
bool TreeElement::isNewline() const
{
    return (getParent() != 0 && getParent()->getType() == NEWLINE_EL);
}
bool TreeElement::isWhite() const
{
    return (getParent() != 0 && getParent()->getType() == WHITE_EL);
}
bool TreeElement::isUnknown() const
{
    return type.contains(UNKNOWN_EL);
}
bool TreeElement::hasSiblings() const
{
    if (getParent() != 0)
        return getParent()->childCount() != 1;
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

    foreach (TreeElement *child, getChildren())
    {
        lb = child->isLineBreaking();

        while (!child->isImportant())
        {
            child->setLineBreaking(false);
            child = (*child)[0];
            lb = lb || child->isLineBreaking();
        }

        child->setLineBreaking(lb);

        if (newLineComming)
        {
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
    if (getParent() != 0)
        return paragraphsAllowed || getParent()->allowsParagraphs();
    else
        return paragraphsAllowed;
}

bool TreeElement::isPaired() const
{
    return paired;
}

int TreeElement::childCount() const
{
    if(DYNAMIC){
        if( this->analyzer != 0 ){
            this->analyzer->checkLocationAST(this->local_deep_AST,this->local_nodes_AST);

            return this->analyzer->getCountElementChildrenAST();
        }else{
//            qDebug() << "chlidCount() null";
            return 0;
        }
    }else{
        return children.count();
    }
}

int TreeElement::index() const
{
    //if(DYNAMIC){ //zisti priamo z AST lua_tonumber(L,-1)-1
    if (getParent() == 0)
        return -1;
    else
        return getParent()->indexOfChild(this);
}

int TreeElement::indexOfChild(const TreeElement *child) const
{
    int p = getChildren().indexOf(const_cast<TreeElement*>(child), 0);
    return p;
}

int TreeElement::indexOfBranch(const TreeElement *desc) const
{
    int i = indexOfChild(desc);

    if (i > -1)
    {
        return i;
    }
    else
    {
        for (i = 0; i < getChildren().size(); i++)
        {
            if (getChildren()[i]->indexOfBranch(desc) > -1) return i;
        }
    }

    return -1;
}

QList<TreeElement*> TreeElement::getChildren() const
{
    if(DYNAMIC){
       this->analyzer->checkLocationAST(this->local_deep_AST,this->local_nodes_AST);

        return this->analyzer->getElementChildrenAST();
    }else{
        return children;
    }
}

QList<TreeElement*> TreeElement::getAncestors() const
{
    QList<TreeElement*> list;
    TreeElement *e = getParent();

    while (e != 0)
    {
        list << e;
        e = e->getParent();
    }

    return list;
}

QList<TreeElement*> TreeElement::getDescendants() const
{
    QList<TreeElement*> list;

    foreach (TreeElement *child, getChildren())
    {
        list << child;
        list << child->getDescendants();
    }

    return list;
}

QList<TreeElement*> TreeElement::getAllLeafs() const
{
    QList<TreeElement*> list;

    foreach (TreeElement *child, getChildren())
    {
        if (child->isLeaf())
            list << child;
        else
            list << child->getDescendants();
    }

    return list;
}

TreeElement *TreeElement::getAncestorWhereFirst() const
{
    TreeElement *el = const_cast<TreeElement*>(this);

    if (el->isFloating()) return el;

    while (el->getParent() != 0 && el->getParent()->indexOfChild(el) == 0)
        el = el->getParent();

    while (!el->isImportant())
        el = el->getChildren()[0];

    return el;
}

TreeElement *TreeElement::getAncestorWhereLast() const
{
    TreeElement *el = const_cast<TreeElement*>(this);

    if (el->isFloating()) return el;

    while (el->getParent() != 0 && el->getParent()->indexOfChild(el) == el->getParent()->childCount()-1)
        el = el->getParent();

    while (!el->isImportant())
        el = el->getChildren()[el->childCount()-1];

    return el;
}


TreeElement *TreeElement::getRoot()
{
    if (getParent() == 0)
        return this;
    else
        return getParent()->getRoot();
}

TreeElement *TreeElement::getParent() const
{
    if(DYNAMIC){
        if(this != 0){
            if( this->analyzer != 0 ){
                this->analyzer->checkLocationAST(this->local_deep_AST,this->local_nodes_AST);

                return this->analyzer->getParentElementAST();
            }else{
                return 0;
            }
        }else{
            return 0;
        }
    }else{
        return parent;
    }
}

QString TreeElement::getType() const
{
    return type;
}

// returns all text in this element and it's descendants
QString TreeElement::getText(bool noComments) const
{
    QString text;
    DocBlock *docBl = 0;

    if (isFloating()) docBl = qgraphicsitem_cast<DocBlock*>(myBlock);

    QString spacesStr = QString().fill(' ', spaces);

    if (isLeaf())
    {
        text = type;                            //! get my text
        if (docBl != 0)
        {
            if (noComments)
                text = "";
            else
                text = docBl->convertToText();      //! get text of docblock
        }
    }
    else
    {
        if (docBl != 0 && !noComments)
            text.append(docBl->convertToText());    //! get text of docblock

        foreach (TreeElement *e, getChildren())
        {
            text.append(e->getText(noComments));          //! get child texts
        }

        text.replace("\n", "\n"+spacesStr);     //! indent after each line break
    }

    text.prepend(spacesStr);                    //! indent my text

    if (lineBreaking)
    {
        if (docBl == 0 || !noComments)
            text.append("\n");                      //! add line break if needed
    }
//qDebug() << text;
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
    if(DYNAMIC){
//        qDebug() << "TreeElement->hasNext()";
        if(this != 0){
            this->analyzer->checkLocationAST(this->local_deep_AST,this->local_nodes_AST);

            for(int i = 0; i < index; i++){
                this->analyzer->nextElementAST_void();
            }
            //return getParent()->analyzer->hasNextElementAST(); //! new iterator
            return this->analyzer->hasNextElementAST(); //! new iterator
        }else{
            return false;
        }
    }else{
        if (index < childCount()) return true;
        if (getParent() == 0) return false;
        return getParent()->hasNext(this->index() + 1);
    }
}

TreeElement *TreeElement::next(int index)
{
    if(DYNAMIC){

        this->analyzer->checkLocationAST(this->local_deep_AST,this->local_nodes_AST);
        for(int i = 0; i < index; i++){
            this->analyzer->nextElementAST_void();
        }

        if(getParent() == 0)
            return 0;

         return this->analyzer->nextElementAST();  //! new iterator

    }else{
        if (index < childCount())
            return getChildren().at(index);
        if (getParent() == 0) return 0;

        return getParent()->next(this->index() + 1);
    }
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
    return getChildren()[index];
}
int TreeElement::operator[](TreeElement* child)
{
    return indexOfChild(child);
}

TreeElement *TreeElement::clone() const
{
    TreeElement *el = new TreeElement(type, selectable, paragraphsAllowed,
                                     lineBreaking, paired);
    // set parent & pair to 0, copy rest of the fields
    el->parent = 0;
    el->pair = 0;
    el->spaces = spaces;
    el->floating = floating;

    // if element belongs to docblock, create child with docblock data
    DocBlock *docBl = qgraphicsitem_cast<DocBlock*>(myBlock);

    if (docBl != 0)
    {
        el->type = "doc_comment";
        el->floating = true;
        el->appendChild(new TreeElement(docBl->convertToText()));
    }

    // append cloned children (this sets their parent field)
    foreach (TreeElement *child, children)
    {
        el->appendChild(child->clone());
    }

    // resolve pairing:
    for (int i = 0; i < getChildren().size(); i++) //! iterate through original children
    {
        TreeElement *child;
        TreeElement *origPair = getChildren()[i]->pair;

        if (origPair == 0)                   //! if original has no pair continue
            continue;

        if (el->getChildren()[i]->getPair() != 0) //! if clone has pair already set continue
            continue;

        int j = i;
        // otherwise find pair-of-original's index j
        do
        {
            child = getChildren()[j];
            j++;
        }
        while (child != origPair && j < getChildren().size());      //refacktoruj na chlidCount?!?

        if (child != origPair || j >= getChildren().size())
            continue;

        el->getChildren()[i]->setPair(el->getChildren()[j]); //! set pair of clone to clone at index j
        el->getChildren()[j]->setPair(el->getChildren()[i]); //! and vice versa
    }

    return el;
}
