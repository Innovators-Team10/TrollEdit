/**
 * analyzer.cpp
 *  ---------------------------------------------------------------------------
 * Contains the defintion of class Analyzer and it's funtions and identifiers
 *
 */

#include "analyzer.h"
#include "tree_element.h"

#include <QDebug>

const char *Analyzer::EXTENSIONS_FIELD = "extensions";
const char *Analyzer::LANGUAGE_FIELD = "language";
const char *Analyzer::MAIN_GRAMMAR_FIELD = "full_grammar";
const char *Analyzer::SUB_GRAMMARS_FIELD = "other_grammars";
const char *Analyzer::PAIRED_TOKENS_FIELD = "paired";
const char *Analyzer::SELECTABLE_TOKENS_FIELD = "selectable";
const char *Analyzer::MULTI_TEXT_TOKENS_FIELD = "multi_text";
const char *Analyzer::FLOATING_TOKENS_FIELD = "floating";
const char *Analyzer::MULTILINE_SUPPORT_FIELD = "multiline_support";
const char *Analyzer::LINE_COMMENT_TOKENS_FIELD = "line_tokens";
const char *Analyzer::MULTILINE_COMMENT_TOKENS_FIELD = "multiline_tokens";
const char *Analyzer::CONFIG_KEYS_FIELD = "cfg_keys";
const QString Analyzer::TAB = "    ";

QString exception;

static void stackDump (lua_State *L) {
    int i;
    int top = lua_gettop(L);
    qDebug("-------STACK--------|");
    for (i = 1; i <= top; i++) { /* repeat for each level */
        int t = lua_type(L, i);
        switch (t) {
        case LUA_TSTRING: { /* strings */
            qDebug("%d. string: '%s'\t|", i, lua_tostring(L, i));
            break;
        }
        case LUA_TBOOLEAN: { /* booleans */
            qDebug("%d. %s\t|", i, lua_toboolean(L, i) ? "true" : "false");
            break;
        }
        case LUA_TNUMBER: { /* numbers */
            qDebug("%d. numbers %g\t|", i, lua_tonumber(L, i));
            break;
        }
        case LUA_TFUNCTION: { /* numbers */
            qDebug("%d. function %s\t|", i, lua_tostring(L, i) );
            break;
        }
        default: { /* other values*/
            qDebug("%d. other %s\t|", i, lua_typename(L, t));
            break;
        }
        }
  //          qDebug("--------------------|"); /* put a separator */
    }
    qDebug("");
}

/**
 * Analyzer class contructor, that initializes Lua and
 * loads Lua base libraries
 *
 * @param script script as a string
 */
Analyzer::Analyzer(QString script)
{
    msgBox = new QMessageBox();
    scriptName = script;
    L = lua_open();             //! initialize Lua
    luaL_openlibs(L);           //! load Lua base libraries
    try
    {
        setupConstants();
    }
    catch (QString exMsg)
    {
        msgBox->critical(0, "Script error", exMsg,QMessageBox::Ok,QMessageBox::NoButton);
    }
}

/**
 * Set up the constants
 *
 */
void Analyzer::setupConstants()
{
    if (luaL_loadfile(L, qPrintable(scriptName)) || lua_pcall(L, 0, 0, 0))
    {
        throw "Error loading script \"" + scriptName + "\"";
    }

    // get extensions
    lua_getglobal (L, EXTENSIONS_FIELD);
    lua_pushnil(L);

    while (lua_next(L, -2) != 0)
    {
        extensions.append(QString(lua_tostring(L, -1)));
        lua_pop(L, 1);
    }

    // get language name
    lua_getglobal (L, LANGUAGE_FIELD);
    langName = QString(lua_tostring(L, -1));
    lua_pop(L, 1);

    // get grammars
    lua_getglobal (L, MAIN_GRAMMAR_FIELD);
    mainGrammar = QString(lua_tostring(L, -1));
    lua_pop(L, 1);

    if (mainGrammar.isEmpty())
        throw "No main grammar specified in script \"" + scriptName + "\"";

    lua_getglobal (L, SUB_GRAMMARS_FIELD);
    lua_pushnil(L);

    while (lua_next(L, -2) != 0)
    {
        subGrammars[QString(lua_tostring(L, -2))] = QString(lua_tostring(L, -1));
        lua_pop(L, 1);
    }

    // get paired tokens (e.g BEGIN/END, {/}, </>)
    lua_getglobal (L, PAIRED_TOKENS_FIELD);
    lua_pushnil(L);

    while (lua_next(L, -2) != 0)
    {
        pairedTokens.append(QString(lua_tostring(L, -1)));
        lua_pop(L, 1);
    }

    // get selectable tokens (can be selected and moved)
    lua_getglobal (L, SELECTABLE_TOKENS_FIELD);
    lua_pushnil(L);

    while (lua_next(L, -2) != 0)
    {
        selectableTokens.append(QString(lua_tostring(L, -1)));
        lua_pop(L, 1);
    }

    // get multi-text tokens (can contain more lines of text)
    lua_getglobal (L, MULTI_TEXT_TOKENS_FIELD);
    lua_pushnil(L);

    while (lua_next(L, -2) != 0)
    {
        multiTextTokens.append(QString(lua_tostring(L, -1)));
        lua_pop(L, 1);
    }

    // get floating tokens
    lua_getglobal (L, FLOATING_TOKENS_FIELD);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0)
    {
        floatingTokens.append(QString(lua_tostring(L, -1)));
        lua_pop(L, 1);
    }

    // get multiline comment support
    lua_getglobal (L, MULTILINE_SUPPORT_FIELD);
    multilineSupport = QString(lua_tostring(L, -1));
    lua_pop(L, 1);

    QStringList tokens;

    // get line tokens
    lua_getglobal (L, LINE_COMMENT_TOKENS_FIELD);
    lua_pushnil(L);

    while (lua_next(L, -2) != 0)
    {
        tokens.append(QString(lua_tostring(L, -1)));
        lua_pop(L, 1);
    }

    commentTokens["line"] = tokens;
    tokens.clear();

    // get multiline tokens
    lua_getglobal (L, MULTILINE_COMMENT_TOKENS_FIELD);
    lua_pushnil(L);

    while (lua_next(L, -2) != 0)
    {
        tokens.append(QString(lua_tostring(L, -1)));
        lua_pop(L, 1);
    }

    commentTokens["multiline"] = tokens;
}

/**
 * Analyzer class destructor, that closes Lua connection
 */
Analyzer::~Analyzer()
{
    lua_close(L);
}

/**
 * Reads a snippet of code, which is represented as a String
 * @param fileName the name of the file that contains a snippet
 */
void Analyzer::readSnippet(QString fileName)
{
    try
    {
        if (luaL_loadfile(L, qPrintable(fileName)) || lua_pcall(L, 0, 0, 0))
        {
            throw "Error loading script \"" + fileName + "\"";
        }

        lua_getglobal (L, qPrintable(extensions.first()));
        defaultSnippet = QString(lua_tostring(L, -1));
        lua_pop(L, 1);

        if (defaultSnippet.isEmpty())
            defaultSnippet = "blank";
    }
    catch (QString exMsg)
    {
        msgBox->critical(0, "Snippet file error", exMsg,QMessageBox::Ok,QMessageBox::NoButton);
    }
}

/**
 * Reads a config from a specified config file
 * @param fileName the specified config file
 * @return a list of key and value pairs
 */
QList<QPair<QString, QHash<QString, QString> > > Analyzer::readConfig(QString fileName)
{
    QList<QPair<QString, QHash<QString, QString> > > tables;
    try
    {
        if (luaL_loadfile(L, qPrintable(fileName)) || lua_pcall(L, 0, 0, 0))
        {
            throw "Error loading script \"" + fileName + "\"";
        }

        QStringList keys;

        // get keys
        lua_getglobal (L, CONFIG_KEYS_FIELD);
        lua_pushnil(L);

        while (lua_next(L, -2) != 0)
        {
            keys.append(QString(lua_tostring(L, -1)));
            lua_pop(L, 1);
        }

        // get values
        foreach (QString key, keys)
        {
            lua_getglobal (L, qPrintable(key));
            lua_pushnil(L);
            QHash<QString, QString> pairs;

            while (lua_next(L, -2) != 0)
            {
                QString key = QString(lua_tostring(L, -2));
                QString value = QString(lua_tostring(L, -1));
                lua_pop(L, 1);
                pairs[key] = value;
            }
            tables << QPair<QString, QHash<QString, QString> >(key, pairs);
        }
        return tables;
    }
    catch (QString exMsg)
    {
        msgBox->critical(0, "Config file error", exMsg,QMessageBox::Ok,QMessageBox::NoButton);
        return tables;
    }
}

/**
 * Analyze input string by provided grammar
 * @param grammar the specified grammar
 * @param input String to be analyzed
 * @return analyzed String as a TreeElement
 */
TreeElement *Analyzer::analyzeString(QString grammar, QString input)
{
    luaL_dofile(L, qPrintable(scriptName));     //! load the script
    lua_getglobal (L, "lpeg");                  //! table to be indexed
    lua_getfield(L, -1, "match");               //! function to be called: 'lpeg.match'
    lua_remove(L, -2);                          //! remove 'lpeg' from the stack
    lua_getglobal (L, qPrintable(grammar));     //! 1st argument
    lua_pushstring(L, qPrintable(input));       //! 2nd argument
    int err = lua_pcall(L, 2, 1, 0);            //! call with 2 arguments and 1 result, no error function

    if (err != 0)
    {
        throw "Error in grammar \"" + grammar + "\" in script \"" + scriptName + "\"";
    }

    TreeElement *root = 0;

    if(lua_istable(L, -1))
    {
       root = createTreeFromLuaStack();

     TreeElement *iter = root->next();
       int i =0;
/*       while(iter->hasNext()){
           i++;
               QString string = iter->getType();//->getText(false);
               qDebug() << i <<". TreeElement: " << string;
               qDebug() << i <<". isLeaf...(): " << iter->isLeaf();
               iter = iter->next();
       }
*/

       qDebug() << "------------DYNAMIC--------------";
       TreeElement *root1 = nextElementAST();
//       root = root1;
//       root->analyzer = this;
    qDebug() <<". TreeElement: " << hasNextElementAST();
      TreeElement *iter1 = root1;//root1->next();
      int k =0;
      while(hasNextElementAST() && iter->hasNext()){
          k++;
          stackDump(L);
           iter1 = nextElementAST();                            //index pre pocet nextov
           QString string = iter1->getType();//->getText(false);
           qDebug() << k <<". TreeElement: " << string;
//           qDebug() << k <<". trHasNext(): " << hasNextElementAST();
           qDebug() << k <<". isLeaf...(): " << isLeafElementAST();

           i++;

                   qDebug() << k <<". getCountElementChildrenAST() " << getCountElementChildrenAST();

/*           luaL_dofile(L, "C:\\Test\\test.lua");     //! load the script
           stackDump(L);
           lua_getfield(L, LUA_GLOBALSINDEX ,"table_lenght");                  // function to be called: 'lenght'
           stackDump(L);
           //lua_getfield(L, );
           lua_insert(L, -3);
           stackDump(L);
           lua_insert(L, -3);
           stackDump(L);
           int err = lua_pcall(L, 1, 1, 0);            // call with 2 arguments and 1 result, no error function
           stackDump(L);
            if(lua_isnumber(L, -1)){
                qDebug() << "lenght(): " << lua_tonumber(L, -1);
            }
*/

               QString string1 = iter->getType();//->getText(false);
               qDebug() << "__" << i <<". TreeElement: " << string1;
               qDebug() << "__" << i <<". isLeaf...(): " << iter->isLeaf();
               qDebug() << "__" << i <<". children...(): " << iter->childCount();

               iter = iter->next();

//           TreeElement *iter1P = parentElementAST();            //index - 1 z povodneho stavu a je to
//           QString stringP = iter1P->getType();
//           qDebug() << k <<". TreeElementP: " << stringP;
//           stackDump(L);
        }

    }

    if(root != 0)
    {
        processWhites(root);
    }
    else
    {
        qWarning("No output from string analysis!");
    }
    return root;
}

/**
 * Analyze string, creates AST and returns root
 * @param input String to be analyzed
 * @return analyzed String as a TreeElement
 */
TreeElement* Analyzer::analyzeFull(QString input)
{
    try
    {
        TreeElement *root = analyzeString(mainGrammar, input);
        root->setFloating();
        return root;
    }
    catch(QString exMsg)
    {
        msgBox->critical(0, "Runtime error", exMsg,QMessageBox::Ok,QMessageBox::NoButton);
        return 0;
    }
}

/**
 * Reanalyze text from element and it's descendants, updates AST and returns first modified node
 * @param element input TreeElement to be reanlyzed
 * @return first modified node
 */
TreeElement *Analyzer::analyzeElement(TreeElement* element)
{
    QString grammar = "";
    TreeElement *subRoot = 0;

    if (element != 0)
        grammar = subGrammars[element->getType()];

    if (!grammar.isEmpty())
    {
        try
        {
            subRoot = analyzeString(grammar, element->getText());
        }
        catch(QString exMsg)
        {
            msgBox->critical(0, "Runtime error", exMsg,QMessageBox::Ok,QMessageBox::NoButton);
            subRoot = 0;
        }
    }
    return subRoot;
}

/**
 * Returns the analysable ancestor of the element
 * @param element input TreeElement
 * @return the analysable ancestor if exists otherwise 0
 */
TreeElement *Analyzer::getAnalysableAncestor(TreeElement *element)
{
    while (element->getParent() != 0)
    {
        if (subGrammars.contains(element->getType())) break;

        element = element->getParent();
    }

    if (element->getParent() == 0)
        return 0;
    else
        return element;
}

/**
 * Creates AST from recursive lua tables (from stack), returns root(s)
 * @return root of AST
 */
TreeElement *Analyzer::createTreeFromLuaStack()
{
    TreeElement *root = 0;
    lua_pushnil(L);               //! first key

    while (lua_next(L, -2) != 0) //! uses 'key' (at index -2) and 'value' (at index -1)
    {
        if(lua_istable(L, -1))
        {
            TreeElement *child = createTreeFromLuaStack();

            if (root == 0)  //! should not happen when tables are properly nested
            {
                root = child;
            }
            else
            {
                root->appendChild(child);
                checkPairing(child);
            }
        }
        else
        {
            QString nodeName = QString(lua_tostring(L, -1));
            bool paired = false;

            if (pairedTokens.indexOf(nodeName, 0) >= 0) //! pairing needed
            {
                paired = true;
            }
            root = new TreeElement(nodeName,
                                   selectableTokens.contains(nodeName),
                                   multiTextTokens.contains(nodeName),
                                   false, paired);

            if (floatingTokens.contains(nodeName))
                root->setFloating(true);
        }
        lua_pop(L, 1); //! removes 'value'; keeps 'key' for next iteration
    }
    return root;
}

TreeElement *Analyzer::nextElementAST()
{
  //  qDebug("nextElementAST()");
  //  stackDump(L);

    TreeElement *root = 0;
    if(!lua_isnumber(L,-1) ){
        lua_pushnil(L);               //! first key
 //       qDebug("pushnil");
 //       stackDump(L);
    }

    if(lua_next(L, -2) != 0)  //! uses 'key' (at index -2) and 'value' (at index -1)
    {
 //       qDebug("luanext");
 //       stackDump(L);

        if(!lua_isstring(L, -1)){
            do{
 //               qDebug("isTable()");
 //               stackDump(L);
                if(!lua_isnumber(L,-1)){
                    lua_pushnil(L);               //! first key
 //                   qDebug("pushnil");
 //                   stackDump(L);
                }
            }while((lua_next(L, -2) != 0) && (lua_istable(L, -1))  );
        }

       // lua_pop(L,1);
 //           qDebug("isString()");
 //           stackDump(L);
            QString nodeName = QString(lua_tostring(L, -1));
            bool paired = false;

            if (pairedTokens.indexOf(nodeName, 0) >= 0) //! pairing needed
            {
                paired = true;
            }
            root = new TreeElement(nodeName,
                                   selectableTokens.contains(nodeName),
                                   multiTextTokens.contains(nodeName),
                                   false, paired);

            if (floatingTokens.contains(nodeName))
                root->setFloating(true);
        lua_pop(L, 1); //! removes 'value'; keeps 'key' for next iteration
    }else{
        lua_pop(L, 1); //! removes 'value'; keeps 'key' for next iteration
        root = nextElementAST();
    }

    return root;
}

bool Analyzer::hasNextElementAST()
{
//    qDebug("hasNextElementAST()");
//    stackDump(L);
int identifier = 0;
    if( ( lua_gettop(L)>=10 ) == true ) //! ak je v zasobniku viac ako 10 prvkov, tak existuje element AST
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Analyzer::isLeafElementAST()
{
    if(lua_istable(L,-2) && lua_objlen(L, -2)==1){
        return true;
    }else{
        return false;
    }
}

int Analyzer::getCountElementChildrenAST()
{
    if(lua_istable(L,-2)){
        return lua_objlen(L, -2)-1;
    }else{
        return 0;
    }
}

TreeElement *Analyzer::parentElementAST()
{
    qDebug("getParentElementAST");
    stackDump(L);

    TreeElement *root = 0;
    if(!lua_isnumber(L,-1)){
        lua_pushnil(L);               //! first key
        qDebug("pushnil");
        stackDump(L);
    }

    if(lua_next(L, -2) != 0)  //! uses 'key' (at index -2) and 'value' (at index -1)
    {
        qDebug("luanext");
        stackDump(L);
        if(!lua_isstring(L, -1)){
            do{
                qDebug("isTable()");
                stackDump(L);
                if(!lua_isnumber(L,-1)){
                    lua_pushnil(L);               //! first key
                    qDebug("pushnil");
                    stackDump(L);
                }
            }while((lua_next(L, -2) != 0) && (lua_istable(L, -1)) );
        }
       // lua_pop(L,1);
            qDebug("isString()");
            stackDump(L);
            QString nodeName = QString(lua_tostring(L, -1));
            bool paired = false;

            if (pairedTokens.indexOf(nodeName, 0) >= 0) //! pairing needed
            {
                paired = true;
            }
            root = new TreeElement(nodeName,
                                   selectableTokens.contains(nodeName),
                                   multiTextTokens.contains(nodeName),
                                   false, paired);

            if (floatingTokens.contains(nodeName))
                root->setFloating(true);
        lua_pop(L, 1); //! removes 'value'; keeps 'key' for next iteration
    }else{
        lua_pop(L, 1); //! removes 'value'; keeps 'key' for next iteration
        root = nextElementAST();
    }

    return root;
}
/**
 * Check the pairing of the element
 * @param closeEl input TreeElement
 */
void Analyzer::checkPairing(TreeElement *closeEl)
{
    QString nodeName = closeEl->getType();
    int pairIndex = pairedTokens.indexOf(nodeName, 0);

    if (pairIndex >= 0) //! pairing needed
    {
        if (pairIndex % 2 == 1)  //! closing element found
        {
            QList<TreeElement *> siblings = closeEl->getParent()->getChildren();
            siblings.removeOne(closeEl);
            int index = siblings.size()-1;

            QString openString = pairedTokens[pairIndex-1];

            while (index >= 0)
            {
                // find closest matching unused opening element
                TreeElement *openEl = siblings[index];

                if (openEl->getType() == openString) //! is matching?
                {
                    if (openEl->getPair() == 0) //! is unused?
                    {
                        openEl->setPair(closeEl);
                        closeEl->setPair(openEl);
                        break;
                    }
                }
                index--;
            }
        }
    }
}

/**
 * Process white spaces of the tree of elements and
 * move all whites as high as possible without changing tree text
 * @param root the root of tree
 */
void Analyzer::processWhites(TreeElement* root)
{
    QList<TreeElement*> whites;
    QList<TreeElement*> newlines;
    TreeElement *element = root;

    while (element->hasNext())
    {
        element = element->next();

        if (element->isWhite())
            whites << element->getParent();

        if (element->isNewline())
            newlines << element->getParent();
    }
    // process newlines: shift right as far as possible, remove and set lineBreaking flag
    while (!newlines.isEmpty())
    {
        TreeElement *el = newlines.takeLast();  //! list is traversed backwards
        TreeElement *parent = el->getParent();
        int index;

        if (parent != 0)
        {
            index = parent->indexOfChild(el) - 1;   //! -1 because el will be removed before checking
            bool isLast = !(*el)[0]->hasNext();
            parent->removeChild(el);   //! remove & destroy newline element
            delete el;

            if (isLast) continue;     //! ignore newlines at the end of file

            while (index == parent->childCount()-1) //! el was the last child
            {
                if (parent->getParent() == 0)
                    break;

                index = parent->index();
                parent = parent->getParent();
            }

            TreeElement *nl = 0;

            if (index < 0) //! add an empty line-breaking element at index = 0;
            {
                index = 0;
                nl = new TreeElement("", false, false, true);
            }
            else
            {
                TreeElement *el = (*parent)[index];

                while (!el->isImportant())
                    el = (*el)[0];

                if (!el->setLineBreaking(true) || el->isNewline()) //! set newline flag
                {
                    // flag was already set -> add an empty line-breaking element at index+1
                    index++;
                    nl = new TreeElement("", false, false, true);
                }
            }

            if (nl != 0)
            {
                parent->insertChild(index, nl);
            }
        }
    }

    // process other whites: shift left as far as possible, don't shift when in line-breaking element
    foreach (TreeElement *el, whites)
    {
        TreeElement *parent = el->getParent();
        // substitute tabs
        (*el)[0]->setType((*el)[0]->getType().replace("\t", TAB));
        int index;

        if (parent != 0)
        {
            index = parent->indexOfChild(el);
            int spaces = (*el)[0]->getType().length();
            parent->removeChild(el);        // remove & destroy
            delete el;

            while (index == 0) // el was the first child
            {
                index = parent->index();

                if (parent->getParent() == 0)
                    break;

                parent = parent->getParent();
            }

            if (index >= 0)
            {
                if (parent->childCount() <= index) continue;
                el = (*parent)[index];
            }
            else
            {
                el = parent;
            }

            while (!el->isImportant())
                el = (*el)[0];

            el->setSpaces(spaces);
        }
    }
    root->adjustSpaces(0);
}
