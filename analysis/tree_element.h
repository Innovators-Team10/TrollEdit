#ifndef TREEELEMENT_H
#define TREEELEMENT_H

#include <QList>
#include <QString>

class TreeElement
{
public:
     TreeElement(QString type = "", bool multilineAllowed = false, bool lineBreaking = false);
     ~TreeElement();

     void setType(QString type);
     void appendChild(TreeElement *child);
     void appendChildren(QList<TreeElement *> children);
     void insertChild(int index, TreeElement *child);
     void insertChildren(int index, QList<TreeElement *> children);
     bool removeChild(TreeElement *child);
     bool removeDescendant(TreeElement *child);
     bool deleteBranchTo(TreeElement *desc);
     bool removeAllChildren();
     int childCount() const;
     int index() const;
     int indexOfChild(const TreeElement* child) const;
     int indexOfDescendant(const TreeElement *desc) const;
     bool isLeaf() const;
     bool isNewline() const;
     bool setLineBreaking(bool flag);
     bool isLineBreaking() const;
     bool allowsMultiline() const;
     bool isWhite() const;
     bool isUnknown() const;
     bool hasSiblings() const;

     TreeElement *operator<<(TreeElement *child);
     TreeElement *operator<<(QList<TreeElement *> children);
     TreeElement *operator[](int index);
     int operator[](TreeElement* child);

     QList<TreeElement *> getChildren() const;
     QList<TreeElement *> getAncestors() const;
     QList<TreeElement *> getDescendants() const;
     TreeElement *getRoot();
     TreeElement *getParent() const;
     QString getType() const;
     QString getText() const;

     bool isImportant() const;

     bool hasNext();
     TreeElement *next();

 protected:
     TreeElement *parent;

 private:
     static const char *WHITE_EL;
     static const char *UNKNOWN_EL;

     QList<TreeElement *> children;
     QString type;
     bool lineBreaking;
     bool multiLineAllowed;
     bool hasNext(int index);
     TreeElement *next(int index);
};

#endif // TREEELEMENT_H
