#ifndef DOC_BLOCK_H
#define DOC_BLOCK_H

#include "block.h"

class DocBlock : public Block
{
public:
    DocBlock(TreeElement *element, Block *parentBlock, QGraphicsScene *parentScene = 0);
};

#endif // DOC_BLOCK_H
