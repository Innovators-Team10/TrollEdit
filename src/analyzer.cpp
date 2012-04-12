/** 
* @file analyzer.cpp
* @author Team 04 Ufopak + Team 10 Innovators
* @version
* 
* @section DESCRIPTION
* Contains the defintion of class Analyzer and it's functions and identifiers
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

const int Analyzer::DEFAULT_STACK_DEEP = 8;

QString exception;

//static void stackDump (lua_State *L) {          //! print stack to debug
//    int i;
//    int top = lua_gettop(L);
//    qDebug("-------STACK--------|");
//    for (i = 1; i <= top; i++) { /* repeat for each level */
//        int t = lua_type(L, i);
//        switch (t) {
//        case LUA_TSTRING: { /* strings */
//            qDebug("%d. string: '%s'\t|", i, lua_tostring(L, i));
//            break;
//        }
//        case LUA_TBOOLEAN: { /* booleans */
//            qDebug("%d. %s\t|", i, lua_toboolean(L, i) ? "true" : "false");
//            break;
//        }
//        case LUA_TNUMBER: { /* numbers */
//            qDebug("%d. numbers %g\t|", i, lua_tonumber(L, i));
//            break;
//        }
//        case LUA_TFUNCTION: { /* numbers */
//            qDebug("%d. function %s\t|", i, lua_tostring(L, i) );
//            break;
//        }
//        default: { /* other values*/
//            qDebug("%d. other %s\t|", i, lua_typename(L, t));
//            break;
//        }
//        }
//  //          qDebug("--------------------|"); /* put a separator */
//    }
//    qDebug("");
//}

/**
 * Analyzer class contructor, that initializes Lua and loads Lua base libraries
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
    qDebug() << "scriptName=" << scriptName;
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
    //stackDump(L);
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
    lua_close(L); //comment
}

/**
 * Reads a snippet of code, which is represented as a String
 * @param fileName the name of the file that contains a snippet
 */
void Analyzer::readSnippet(QString fileName)
{
    qDebug("reading snippet...");
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

    if(TreeElement::DYNAMIC) resetAST();

    if(lua_istable(L, -1))
    {
      bool DYNAMIC =  TreeElement::DYNAMIC;
      qDebug() << "DYNAMIC " << DYNAMIC ;
      if(DYNAMIC){
          root = nextElementAST();
          root->analyzer = this;
//          stackDump(L);
          qDebug() << "------------DYNAMIC--------------";
      }else{
          root = createTreeFromLuaStack();
/*
      TreeElement *iter1 = nextElementAST();
      TreeElement *iter = root;

      TreeElement* parent = getParentElementAST(); // CHECK
      if( parent != 0 ){
          qDebug() << "Parent: " << parent->getText();
      }else{
           qDebug() << "Parent: null";
      }

      TreeElement* ped;
      int i =0;
      int k =0;
      qDebug() << "------------ " << hasNextElementAST() << " ----- " << iter->hasNext();
      while(hasNextElementAST() && iter->hasNext()){
          k++;

           qDebug() << "------------Element--------------";
           iter1 = nextElementAST();                            //index pre pocet nextov
           QString string = iter1->getType();//->getText(false);
//           stackDump(L);
           qDebug() << k <<". TreeElement: " << string;
//           qDebug() << k <<". HasNext..(): " << hasNextElementAST();
           qDebug() << k <<". isLeaf...(): " << isLeafElementAST();
//           qDebug() << k <<". childAST.(): " << getCountElementChildrenAST();           
//           qDebug() << k <<". listChild(): " << getElementChildrenAST();
           TreeElement* parent = getParentElementAST();
           if( parent != 0){
                qDebug() << k <<". getParent(): " << parent->getType();
           }else{
                qDebug() << k <<". getParent(): null";
           }
           resetAST();

           qDebug() << "getElem: " << getElementAST(getDeepAST(),getNodesAST())->getType();

           QList<TreeElement*> aaa = getElementChildrenAST();
           qDebug() << k <<". listChild(): " << aaa;
           for(int a = 0 ; a < aaa.count(); a++){
              qDebug() << a <<". ChildElement: " <<  aaa[a]->getType();
           }
           resetAST();

           i++;

//           luaL_dofile(L, "C:\\Test\\util.lua");                 //! load the script
//           lua_getfield(L, LUA_GLOBALSINDEX ,"getCount");    // function to be called: 'lenght'
//           lua_insert(L, -3);
//           lua_insert(L, -3);
//           int err = lua_pcall(L, 1, 2, 0);            // call with 2 arguments and 1 result, no error function
//           lua_insert(L, -3);
//            if(lua_isnumber(L, -1)){
//                qDebug() << "lenght(): " << lua_tonumber(L, -1)-1;
//                lua_pop(L, 1); // lua_remove(L, -1);
//            }

              iter = iter->next();

             QString string1 = iter->getType();//->getText(false);
             qDebug() << "__" << i <<". TreeElement: " << string1;
//               qDebug() << "__" << i <<". HasNext..(): " << iter->hasNext();
               qDebug() << "__" << i <<". isLeaf...(): " << iter->isLeaf();
//               qDebug() << "__" << i <<". childAST.(): " << iter->childCount();
             qDebug() << "__" << i <<". getParent(): " << iter->getParent()->getType();
             qDebug() << "__" << i <<". listChild(): " << iter->getChildren();
             for(int a = 0 ; a < iter->childCount(); a++){
                  qDebug() << "__" << a <<". ChildElement: " <<  iter->getChildren()[a]->getType();
             }

             if(iter->childCount()==aaa.size())
                qDebug() << "Compare: " << i <<". child: " << iter->childCount() << " " << aaa.size() << " " << "true" ;
             else
                qDebug() << "Compare: " << i <<". child: " << iter->childCount() << " " << aaa.size() << " " << "false" ;
        }
//*/
    }
    }

    if(root != 0)
    {
        if(TreeElement::DYNAMIC){

        }else{
        processWhites(root);
        }
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
            if(TreeElement::DYNAMIC){
            //!dopln
                subRoot = reanalyzeString(element, grammar, element->getText());
            }else{
            subRoot = analyzeString(grammar, element->getText());
            }
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
        qDebug() << "reanlys el:" << element->getType();
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

/**
 * Reanalyze element and replace him in AST (lua stack)
 * @param el from element get position in AST (lua stack)
 * @param grammar use to analyse input string in LPeg
 * @param input text for analyse
 * @return reanalysed element
 */
TreeElement *Analyzer::reanalyzeString(TreeElement *el, QString grammar, QString input)
{
//    stackDump(L);
    setIndexAST(el->local_deep_AST,el->local_nodes_AST);
//    stackDump(L);
    int last = lua_tonumber(L, -3) - 1;
    lua_pop(L, 3);
    //lua_pushstring(L, "reanalys_table");
    //luaL_dofile(L, qPrintable(scriptName));     //! load the script
//    stackDump(L);
//    lua_getglobal (L, "lpeg");                  //! table to be indexed
//    stackDump(L);
//    lua_getfield(L, -1, "match");               //! function to be called: 'lpeg.match'
//    stackDump(L);
//    lua_remove(L, -2);                          //! remove 'lpeg' from the stack
//    stackDump(L);
//    lua_getglobal (L, qPrintable(grammar));     //! 1st argument
//    stackDump(L);
//    lua_pushstring(L, qPrintable(input));       //! 2nd argument
//    stackDump(L);
    int err = lua_pcall(L, 2, 1, 0);            //! call with 2 arguments and 1 result, no error function
//    stackDump(L);
    if (err != 0)
    {
        throw "Error in grammar \"" + grammar + "\" in script \"" + scriptName + "\"";
    }
    lua_rawseti(L, -2, last);
//    stackDump(L);
    //lua_rawset(L, -3);
    //stackDump(L);
//    lua_pushnumber(L, last);
//    stackDump(L);
//    lua_next(L, -2);
//    stackDump(L);
//    lua_pushnil(L);
//    stackDump(L);
//    lua_next(L,-2);
//    stackDump(L);

    TreeElement *root = 0;

    root = getElementAST();
    return root;
}

/**
 * Reset AST (lua stack) to size DEFAULT_STACK_DEEP
 * @return void
 */
void Analyzer::resetAST(){     //! TODO: add Analyzer::DEFAULT_STACK_DEEP
    while(lua_gettop(L) > 9 ){ //! pre zachovanie konzistencie zasobnika mazeme stary AST
        lua_remove(L, -1);
    }
//    nextElementAST();
}

/**
 * Set global position in AST at deep and order
 * @see getElementAST(int deep, int* nodes)
 * @param deep
 * @param nodes
 * @return element at deep and in order from nodes
 */
TreeElement *Analyzer::setIndexAST(int deep, int *nodes){
    return getElementAST(deep, nodes);
}

/**
 * Set local position to global in AST
 * @param deep
 * @param nodes
 * @return void
 */
void Analyzer::checkLocationAST(int deep, int* nodes){
    if( deep != glob_deep_AST  ){ //!
        setIndexAST(deep, nodes);
        return;
    }

    for(int i = 0; i < deep-1; i++){
        if(nodes[i] != glob_nodes_AST[i]){
            setIndexAST(deep, nodes);
            return;
        }
    }
}

/**
 * Next element from actual AST
 * @see getElementAST()
 * @return element next element
 */
TreeElement *Analyzer::nextElementAST()
{
//    qDebug("nextElementAST()");
    TreeElement *root = 0;
    if(!lua_isnumber(L,-1) ){
        lua_pushnil(L);                 //! first key
    }
    if(lua_next(L, -2) != 0)            //! uses 'key' (at index -2) and 'value' (at index -1)
    {
        if(!lua_isstring(L, -1)){
            do{
                if(!lua_isnumber(L,-1)){
                    lua_pushnil(L);             //! first key
                }
            }while((lua_next(L, -2) != 0) && (lua_istable(L, -1))  );
        }
        root = getElementAST();
        lua_pop(L, 1);                  //! odstrani text
    }else{
        lua_pop(L, 1);                  //! removes 'value'; keeps 'key' for next iteration
        root = nextElementAST();
    }

    return root;
}

/**
 * Return element from actual AST
 * @return element
 */
TreeElement *Analyzer::getElementAST()
{
    TreeElement *root = 0;

    QString nodeName = QString(lua_tostring(L, -1));
    bool paired = false;
    if (pairedTokens.indexOf(nodeName, 0) >= 0) //! pairing needed
    {
        paired = true;
    }
    bool lineBreak = checkLineBreak();
    nodeName = nodeName.replace("\t", TAB);     //!
//    if(nodeName.isEmpty()){
//        nodeName = TAB;
//    }
    if(nodeName.contains("\n")){
//        qDebug() << "\\n";
//        nodeName = nodeName.replace("\n", "", Qt::CaseInsensitive);
        nodeName = "";
    }
//    qDebug() << "after: " << nodeName;
    root = new TreeElement(nodeName,
                           selectableTokens.contains(nodeName),
                           multiTextTokens.contains(nodeName),
                           lineBreak, paired);
    if (floatingTokens.contains(nodeName)){
        root->setFloating(true);
//        qDebug() << "node " << nodeName <<" floating true" ;
    }
    if(nodeName == TreeElement::NEWLINE_EL || nodeName == TreeElement::WHITE_EL){
        root->setSpaces(nodeName.length());
//        qDebug() << "space " << nodeName.length();
    }

    root->local_deep_AST = getDeepAST();
    root->local_nodes_AST = getNodesAST();
    root->analyzer = this;

    return root;
}

/**
 * Return element from actual AST
 * @return element at deep and in order from nodes
 */
TreeElement *Analyzer::getElementAST(int deep, int* nodes )
{
    TreeElement *root = 0;

    resetAST();
    for(int i = 0; i < deep-1; i++){
        lua_pushnumber(L, nodes[i]-1);
        lua_next(L, -2);
    }
    lua_pushnil(L);
    lua_next(L, -2);
    root = getElementAST();
    lua_pop(L,1);

    return root;
}

int Analyzer::getDeepAST()
{
    //stackDump(L);
    int deep = (lua_gettop(L) - DEFAULT_STACK_DEEP)/2;
    return deep;
}

int* Analyzer::getNodesAST()
{
//    stackDump(L);
    int size = getDeepAST();
    int* nodes = new int[size];
    for( ;size > 0; size--){
        nodes[size-1] = lua_tonumber(L, size*2 + DEFAULT_STACK_DEEP);
//        qDebug() << "."<<size << "nodes: " << size*2 +DEFAULT_STACK_DEEP << " nodes[size]: " << nodes[size-1];
    }

    return nodes;
}

bool Analyzer::checkLineBreak(){
    bool lineBreak = false;

    QString next = "";
    if( lua_gettop(L) > DEFAULT_STACK_DEEP + 3 ){
        int last = lua_tonumber(L, -4) - 1;
        lua_pop(L, 3);
        int s = lua_gettop(L);
        lua_next(L, -2);
        if(s <= lua_gettop(L) ){
            lua_pushnil(L);
            lua_next(L, -2);
            next = lua_tostring(L, -1);
            lua_pop(L, 4);
        }
        lua_pushnumber(L, last);
        lua_next(L, -2);
        lua_pushnil(L);
        lua_next(L, -2);
    }else{
        next = "";
    }
    QString node = lua_tostring(L, -1);
    if( next == "nl" || next == "line_comment"  || next == "multiline_comment" || node.contains("\n") ) {
        lineBreak = true;
    }else{
        lineBreak = false;
    }
    return lineBreak;
}

void Analyzer::nextElementAST_void()
{
    if(!lua_isnumber(L,-1) ){
        lua_pushnil(L);               //! first key
    }

    if(lua_next(L, -2) != 0)  //! uses 'key' (at index -2) and 'value' (at index -1)
    {
        if(!lua_isstring(L, -1)){
            do{
                if(!lua_isnumber(L,-1)){
                    lua_pushnil(L);               //! first key
                }
            }while((lua_next(L, -2) != 0) && (lua_istable(L, -1))  );
        }

        lua_pop(L, 1); //! removes 'value'; keeps 'key' for next iteration
    }else{
        lua_pop(L, 1); //! removes 'value'; keeps 'key' for next iteration
        nextElementAST_void();
    }
}

bool Analyzer::hasNextElementAST()
{
//    qDebug("hasNextElementAST()");
    if( ( lua_gettop(L)>=9 ) == true ) //! ak je v zasobniku viac ako 10 prvkov, tak existuje element AST
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
//    qDebug() << "isLeafElementAST()";
    if(lua_istable(L,-2) && lua_objlen(L, -2)==1){
//        qDebug() << "isLeafElementAST() true";
        return true;
    }else{
//        qDebug() << "isLeafElementAST() false";
        return false;
    }
}

int Analyzer::getCountElementChildrenAST()
{
//    qDebug() << "getCountElementChildrenAST()";
    if(lua_istable(L,-2)){
//        qDebug() << "getCountElementChildrenAST() " << lua_objlen(L, -2)-1;
        return lua_objlen(L, -2)-1;
    }else{
        return 0;
    }
}

QList<TreeElement*> Analyzer::getElementChildrenAST(){
    QList<TreeElement*> children;
   // qDebug() << "getElementChildrenAST()";
    int limit = getCountElementChildrenAST();
    if( limit > 0 ){
            int last = lua_tonumber(L, -1);
            for(int i = 0; i < limit; i++){
                lua_next(L,-2);
                lua_pushnil(L);
                lua_next(L,-2);
                //stackDump(L);
                children.append(getElementAST());               //! add children to list
                lua_pop(L,3);
            }
            lua_pop(L,1);
            lua_pushvalue(L,last);
    }
    return children;
}

TreeElement* Analyzer::getParentElementAST(){
//    qDebug() << "getParentElementAST()";
    if( lua_gettop(L) > 10 ){
        TreeElement* parent;
        int last = lua_tonumber(L, -3) - 1;
        lua_pop(L, 3);
        lua_pushnil(L);
        lua_next(L, -2);
        parent = getElementAST();
        lua_pop(L, 2);
        lua_pushnumber(L, last);
        lua_next(L, -2);
        lua_pushnumber(L, 1);

        return parent;
    }else{
        resetAST();
        return 0;
    }
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
