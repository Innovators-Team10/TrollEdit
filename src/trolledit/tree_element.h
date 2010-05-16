#ifndef TREEELEMENT_H
#define TREEELEMENT_H

#include <QList>
#include <QString>

class Block;

class TreeElement
{
public:
     int spaces;

     TreeElement(QString type = "", bool selectable = false,
                 bool multiText = false, bool lineBreaking = false, bool paired = false);
     ~TreeElement();

     void setType(QString type);
     void appendChild(TreeElement *child);
     void appendChildren(QList<TreeElement *> children);
     void insertChild(int index, TreeElement *child);
     void insertChildren(int index, QList<TreeElement *> children);
     bool removeChild(TreeElement *child);
     bool removeDescendant(TreeElement *child);
     bool removeAllChildren();
     void deleteAllChildren();
     int childCount() const;
     int index() const;
     int indexOfChild(const TreeElement* child) const;
     int indexOfBranch(const TreeElement *desc) const;
     bool hasSiblings() const;

     bool isLeaf() const;
     bool isNewline() const;
     bool isWhite() const;
     bool isUnknown() const;

     void setSpaces(int number);
     void addSpaces(int number);
     int getSpaces() const;
     void adjustSpaces(int offset);

     bool setLineBreaking(bool flag = true);
     bool isLineBreaking() const;
     bool allowsParagraphs() const;
     bool isSelectable() const;
     bool isPaired() const;
     bool isFloating() const;
     void setFloating(bool floating = true);

     TreeElement *operator<<(TreeElement *child);
     TreeElement *operator<<(QList<TreeElement *> children);
     TreeElement *operator[](int index);
     int operator[](TreeElement* child);

     QList<TreeElement*> getChildren() const;
     QList<TreeElement*> getAncestors() const;
     QList<TreeElement*> getDescendants() const;
     QList<TreeElement*> getAllLeafs() const;
     TreeElement *getRoot();
     TreeElement *getParent() const;
     QString getType() const;
     QString getText() const;

     bool isImportant() const;
     void setBlock(Block *block);
     Block *getBlock() const;
     void setPair(TreeElement *pair);
     TreeElement *getPair() const;

     bool hasNext();
     TreeElement *next();

     TreeElement *clone() const;

 protected:
     TreeElement *parent;

 private:
     static const char *WHITE_EL;
     static const char *UNKNOWN_EL;
     static const char *NEWLINE_EL;

     QList<TreeElement*> children;
     QString type;
     Block *myBlock;
     TreeElement *pair;
     bool lineBreaking;
     bool selectable;
     bool paragraphsAllowed;
     bool paired;
     bool floating;

     bool hasNext(int index);
     TreeElement *next(int index);
};

#endif // TREEELEMENT_H
