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

static void stackDump (lua_State *L) {          //! print stack to debug
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
    lua_close(L); //comment
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

    //resetAST();
    glob_index = 0;
    while(lua_gettop(L) > 9 ){ //pre zachovanie konzistencie zasobnika mazeme stary AST
        lua_remove(L, -2);
    }

    if(lua_istable(L, -1))
    {
      qDebug() << "before--DYNAMIC";
      bool DYNAMIC =  TreeElement::DYNAMIC;
      if(DYNAMIC){
          root = nextElementAST();
          root->analyzer = this;
          stackDump(L);
          qDebug() << "------------DYNAMIC--------------";
      }else{
          root = createTreeFromLuaStack();
      }

//      TreeElement *iter1;// = nextElementAST();
 //     root = iter1;
/*
      qDebug() <<"1. type L: " << iter1->getType();
      stackDump(iter1->analyzer->L);
      TreeElement *iter2 = iter1->next();
      qDebug() <<"2. type L: " << iter2->getType();
      stackDump(iter2->analyzer->L);
      iter1 = iter1->next();
      qDebug() <<"1. type L: " << iter1->getType();
      stackDump(iter1->analyzer->L);
      qDebug() <<"2. type L: " << iter2->getType();
      stackDump(iter2->analyzer->L);
*/

 /*   qDebug() << "1. TreeElement: " << iter1->getType();
      stackDump(L);
      iter1 = nextElementAST();
      qDebug() << "2. TreeElement: " << iter1->getType();
      stackDump(L);
      iter1 = getParentElementAST();
      qDebug() << "3. TreeElement: " << iter1->getType();
      stackDump(L);
      iter1 = nextElementAST();
      qDebug() << "4. TreeElement: " << iter1->getType();
      stackDump(L);
*/

      TreeElement *iter = root;
/*      qDebug() << "1. TreeElement: " << iter->getType();
      iter = iter->next();
      qDebug() << "2. TreeElement: " << iter->getType();
      iter = iter->getParent();
      qDebug() << "3. TreeElement: " << iter->getType();
      iter = iter->next();
      qDebug() << "4. TreeElement: " << iter->getType();
*/
/*      TreeElement* ped;
      int i =0;
      int k =0;
      iter1 = nextElementAST();
      qDebug() << "------------ " << hasNextElementAST() << " ----- " << iter->hasNext();
      while(hasNextElementAST() && iter->hasNext()){
          k++;

           qDebug() << "------------Element--------------";
           iter1 = nextElementAST();                            //index pre pocet nextov
           QString string = iter1->getType();//->getText(false);
//           stackDump(L);
           qDebug() << k <<". TreeElement: " << string;
//           qDebug() << k <<". HasNext..(): " << hasNextElementAST();
//           qDebug() << k <<". isLeaf...(): " << isLeafElementAST();
//           qDebug() << k <<". childAST.(): " << getCountElementChildrenAST();           
//           qDebug() << k <<". listChild(): " << getElementChildrenAST();
           qDebug() << k <<". glob_index: " << glob_index;
           qDebug() << k <<". local_index: " << iter1->local_index;
           qDebug() << k <<". getParent(): " << getParentElementAST()->getType();
           qDebug() << k <<". parent_index: " << glob_index;
           resetAST();
           setIndexAST(iter1->local_index);

           if(k == 66 ){
               ped = iter1;
               stackDump(L);
               QList<TreeElement *> list  = getElementChildrenAST();
               qDebug() << "listChild(): " << list.count();
               stackDump(L);
               resetAST();
               setIndexAST(iter1->local_index);
               for(int i = 0; i < list.count();i++){
                   qDebug() << i <<". child: " << list.at(i)->getType();
                   qDebug() << i <<". ch index: " << list.at(i)->local_index;
               }
           }
//           stackDump(L);

//           QList<TreeElement*> aaa = getElementChildrenAST();
//           qDebug() << k <<". listChild(): " << aaa;
//           for(int a = 0 ; a < aaa.count(); a++){
//              qDebug() << a <<". ChildElement: " <<  aaa[a]->getType();
//           }

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
//               qDebug() << "__" << i <<". isLeaf...(): " << iter->isLeaf();
//               qDebug() << "__" << i <<". childAST.(): " << iter->childCount();
               qDebug() << "__" << i <<". getParent(): " << iter->getParent()->getType();
//             qDebug() << "__" << i <<". listChild(): " << iter->getChildren();
<<<<<<< HEAD
//               for(int a = 0 ; a < iter->childCount(); a++){
//                  qDebug() << "__" << a <<". ChildElement: " <<  iter->getChildren()[a]->getType();
//               }
<<<<<<< HEAD

               qDebug() << "__" << i <<". children...(): " << iter->childCount();

               iter = iter->next();

//           TreeElement *iter1P = parentElementAST();            //index - 1 z povodneho stavu a je to
//           QString stringP = iter1P->getType();
//           qDebug() << k <<". TreeElementP: " << stringP;
//           stackDump(L);
        }
=======
=======
               for(int a = 0 ; a < iter->childCount(); a++){
                  qDebug() << "__" << a <<". ChildElement: " <<  iter->getChildren()[a]->getType();
               }
>>>>>>> 108904fbc2dbcb9c050f90eb584c3e238205f605
        }*/
//>>>>>>> bf28cfd8b3c2a64f6cba55646b8c30cd8a0fdc57

//      qDebug() <<"50. TreeElement: " << ped->getType();
//      qDebug() <<"50. glob_index: " << glob_index;
//      qDebug() <<"50. local_index: " << ped->local_index;
//      setIndexAST(ped->local_index);
//      ped = getParentElementAST();
//      qDebug() <<"50. getParent(): " << ped->getType();
//      qDebug() <<"50. parent_index: " << ped->local_index;

//      qDebug() <<"1. resetAST(): " << nextElementAST()->getType();
//      qDebug() <<"1. glob_index: " << glob_index;
//      qDebug() <<"2. resetAST(): " << nextElementAST()->getType();
//      qDebug() <<"2. glob_index: " << glob_index;
//      qDebug() <<"3. resetAST(): " << nextElementAST()->getType();
//      qDebug() <<"3. glob_index: " << glob_index;
//      qDebug() <<"3. parent(): " << getParentElementAST()->getType();
//      qDebug() <<"3. glob_index: " << glob_index;

//      TreeElement *aaa = setIndexAST(55);
//      qDebug() <<"50. TreeElement: " << aaa->getType();
//      qDebug() <<"50. local_index: " << aaa->local_index;
//      qDebug() <<"50. glob_index: " << glob_index;
//      qDebug() <<"50. getParent(): " << getParentElementAST()->getType();  ///spravny?index!!!!
//      qDebug() <<"50. getParent(): " << nextElementAST()->getType();  ///spravny?index!!!!

//      qDebug() <<"1. resetAST(): " << nextElementAST()->getType();
//      qDebug() <<"1. glob_index: " << glob_index;
//      qDebug() <<"2. resetAST(): " << nextElementAST()->getType();
//      qDebug() <<"2. glob_index: " << glob_index;
//      qDebug() <<"3. resetAST(): " << nextElementAST()->getType();
//      qDebug() <<"3. glob_index: " << glob_index;

    }

    if(root != 0)
    {
      //  processWhites(root);
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

void Analyzer::resetAST(){
    while(lua_gettop(L) > 9 ){ //pre zachovanie konzistencie zasobnika mazeme stary AST
        lua_remove(L, -1);
    }
    nextElementAST();
    glob_index = 1;
}

TreeElement *Analyzer::setIndexAST(int index){
    TreeElement *pom;
   // if(glob_index >= index){
        resetAST();
   // }
    while(index > glob_index){
        pom  = nextElementAST();
    }
    return pom;
}

TreeElement *Analyzer::nextElementAST()
{
//    qDebug("nextElementAST()");
    TreeElement *root = 0;
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
            glob_index++; //count for global index in AST
            root->local_index = glob_index;

            qDebug() << "nextElementAST(): " << nodeName << " local " << root->local_index ;

            if (floatingTokens.contains(nodeName))
                root->setFloating(true);
            root->analyzer = this;
        lua_pop(L, 1); //! removes 'value'; keeps 'key' for next iteration
    }else{
        lua_pop(L, 1); //! removes 'value'; keeps 'key' for next iteration
        root = nextElementAST();
    }

    return root;
}

bool Analyzer::hasNextElementAST()
{
    qDebug("hasNextElementAST()");
 //   stackDump(L);
int identifier = 0;
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
     qDebug() << "isLeafElementAST()";
    if(lua_istable(L,-2) && lua_objlen(L, -2)==1){
        return true;
    }else{
        return false;
    }
}

int Analyzer::getCountElementChildrenAST()
{
    qDebug() << "getCountElementChildrenAST()";
 //   stackDump(L);
    if(lua_istable(L,-2)){
        return lua_objlen(L, -2)-1;
    }else{
        return 0;
    }
}

/*
int Analyzer::getCountElementChildrenAST()
{
   if(lua_istable(L,-2)){
          int number;
          luaL_dofile(L, "C:\\Test\\util.lua");                 //! load the script
          stackDump(L);
          lua_getfield(L, LUA_GLOBALSINDEX ,"getCount");    // function to be called: 'lenght'
          stackDump(L);
          lua_insert(L, -3);
          stackDump(L);
          lua_insert(L, -3);
          stackDump(L);
          int err = lua_pcall(L, 1, 2, 0);            // call with 2 arguments and 1 result, no error function
          stackDump(L);
          lua_insert(L, -3);
          stackDump(L);
           if(lua_isnumber(L, -1)){
               qDebug() << "lenght(): " << lua_tonumber(L, -1);
                stackDump(L);
               number = lua_tonumber(L, -1);
               lua_pop(L, 1); // lua_remove(L, -1);
               stackDump(L);
           }
            //   qDebug() << "table: ";
            return number;
    }else{
        return 0;
    }
*/

/*
// treba celkovy redesing a odstranit parenta
QList<TreeElement*> Analyzer::getElementChildrenAST(){
//QList<TreeElement*> getElementChildrenAST(TreeElement* parent){
    QList<TreeElement*> children;
//    if(!parent->isLeaf()){
//        for(int i = 0; i < parent->getChildren(); i++ ){
    if( !isLeafElementAST() ){
        for(int i = 0; i < getCountElementChildrenAST(); i++ ){
            //pri prvom decku asi bude treba oznacit ako pairing first a poslednom paired Last
            //next ktory je teraz sa hned vnara do deti... treba to premysliet

    //      child->parent = this;
            TreeElement* child;
            child = nextElementAST();
            qDebug() << "getElementChildrenAST() for" << child->getType();
            children.append(child);
        }
    }
    return children;
}
*/

QList<TreeElement*> Analyzer::getElementChildrenAST(){
    QList<TreeElement*> children;
    qDebug() << "getElementChildrenAST()";

    int hlbka_child = lua_gettop(L);
    int rozbalenie_child = lua_tonumber(L, -3);

    int limit = getCountElementChildrenAST();
    if( limit > 0 ){
            TreeElement* child;
            while( (limit >= lua_tonumber(L, -3)) ){
                child = nextElementAST();
              //      qDebug() << "b1): " << child->getType() << "number:" << lua_tonumber(L, -3) << " top:" << lua_gettop(L);
              //      stackDump(L);
                if( (hlbka_child+2) == lua_gettop(L) ){
                       stackDump(L);
                       qDebug() << "getElementChildrenAST(): " << child->getType();
                       //refactor -> create function createTreeElement(QString nodeName)
                      children.append(child);               //! add children to list
                }
            }
    }
    stackDump(L);
    return children;
}

/*
QList<TreeElement*> Analyzer::getElementChildrenAST(){
    QList<TreeElement*> children;
    qDebug() << "getElementChildrenAST()";
    int limit = getCountElementChildrenAST();
    if( limit > 0 ){
            TreeElement* child;
            for(int i = 0; i < limit ; i++ ){
                lua_next(L, -2);                    //! iterate on child

                QString nodeName;
                   nodeName = getChildAST();        //! function
//                   stackDump(L);
                   qDebug() << "getElementChildrenAST(): " << nodeName;
                   //refactor -> create function createTreeElement(QString nodeName)
                   bool paired = false;
                   if (pairedTokens.indexOf(nodeName, 0) >= 0) //! pairing needed
                   {
                       paired = true;
                   }
                   child = new TreeElement(nodeName,
                                      selectableTokens.contains(nodeName),
                                      multiTextTokens.contains(nodeName),
                                      false, paired);
                   child->local_index = glob_index;

                   if (floatingTokens.contains(nodeName))
                        child->setFloating(true);
                   child->analyzer = this;

              children.append(child);               //! add children to list
            }
         lua_pop(L, 1);
         lua_pushnumber(L, 1);
    }
    return children;
}

QString Analyzer::getChildAST(){
    QString child;
      if(lua_isstring(L, -1)){
          child = QString(lua_tostring(L, -1));
          lua_pop(L, 1);
      }else{
              lua_pushnil(L);
              while(lua_next(L, -2) != 0)
              {
                  if(lua_isstring(L, -1)){
                      child = getChildAST();                                        
                      break;
                  }else{
                    child = getChildAST();
                    lua_pop(L, 2);
                  }
              }
              lua_pop(L, 2);
       }
      return child;
}
*/

/* tento sposob pracoval nad duplikovanou tabulkou
QList<TreeElement*> Analyzer::getElementChildrenAST(){
    QList<TreeElement*> children;
    if( getCountElementChildrenAST() > 0 ){

        //stackDump(L);
        luaL_dofile(L, "C:\\Test\\util.lua");                 //! load the script
        lua_getfield(L, LUA_GLOBALSINDEX ,"getChild");    // function to be called: 'lenght'
        lua_insert(L, -3);
        lua_insert(L, -3);
        int err = lua_pcall(L, 1, 2, 0);            // call with 2 arguments and 1 result, no error function
        lua_insert(L, -3);

            TreeElement* child;
            lua_pushnumber(L, lua_tonumber(L, -2) );
            int limit = getCountElementChildrenAST();

        //    stackDump(L);
            for(int i = 0; i < limit ; i++ ){
                lua_next(L, -2);

                QString nodeName;
                   nodeName = getChildAST();

                   bool paired = false;

                   if (pairedTokens.indexOf(nodeName, 0) >= 0) //! pairing needed
                   {
                       paired = true;
                   }
                   child = new TreeElement(nodeName,
                                      selectableTokens.contains(nodeName),
                                      multiTextTokens.contains(nodeName),
                                      false, paired);

                   if (floatingTokens.contains(nodeName))
                   child->setFloating(true);
                   child->analyzer = this;

              children.append(child);
            }
          //  stackDump(L);
            lua_pop(L, 2);
    }
    return children;
}
*/


TreeElement* Analyzer::getParentElementAST(){
    qDebug() << "getParentElementAST()";

    if(lua_gettop(L) > 9){
    TreeElement* parent;
    QString nodeName;

    if(lua_isnumber(L,-1)){         //zisti ci naozaj treba podmienku???
        nodeName = getParentAST();
    }

    bool paired = false;
    if (pairedTokens.indexOf(nodeName, 0) >= 0) //! pairing needed
    {
        paired = true;
    }
    parent = new TreeElement(nodeName,
                       selectableTokens.contains(nodeName),
                       multiTextTokens.contains(nodeName),
                       false, paired);
    //qDebug() << "ParentToLocal " << glob_index;
    parent->local_index = glob_index;

    if (floatingTokens.contains(nodeName))
        parent->setFloating(true);
    parent->analyzer = this;

    return parent;
    }else{
    return 0;
    }
}

QString Analyzer::getParentAST(){
    QString parent;

    //zisti aktualny element/////////
//    qDebug() << "a1)parent: ";
    int zac_glob_index = glob_index;
    int pom_i = lua_tonumber(L,-1)-1;
    lua_pop(L, 1);
    if(pom_i == 0){
        lua_pushnil(L);
    }else{
        lua_pushnumber(L,pom_i); //! nastane niekedy? over
    }
    lua_next(L,-2);
    QString child  = lua_tostring(L,-1);
    lua_pop(L, 1);
    int hlbka_child = lua_gettop(L);
    int rozbalenie_child = lua_tonumber(L, -3);
//    qDebug() << "a2)parent child: " << child << " number:" << rozbalenie_child << " top:" << hlbka_child;
//    stackDump(L);
    /////////////////////////////////

    do{
        if(lua_isnumber(L, -1)){
            int actual = lua_tonumber(L, -1);
            if(actual > 1){
                lua_pushnumber(L,actual-1);
                lua_next(L,-2);
            }else{
                if(lua_tonumber(L,-3)==1){
                    //glob_index-=lua_tonumber(L,-3);
                    //qDebug() << "pop-3: -n: " << lua_tonumber(L,-3);
                }else{
                    //glob_index-=lua_tonumber(L,-3)-1;
                    //qDebug() << "pop-3: -n+1: " << lua_tonumber(L,-3)-1 << "  glob: " << glob_index ;
                    //stackDump(L);
                }
                if(glob_index < 1) //odstran
                    glob_index = 1;
                lua_pop(L,3);       //check pushnumber?
                lua_pushnil(L);
                lua_next(L,-2);
            }
        }
    }while(!(lua_isstring(L,-1)));
    parent = lua_tostring(L, -1);
    lua_pop(L,1);
//    qDebug() << "pop child_count: " <<  getCountElementChildrenAST();

    //urob next aby si dostal Parent a pocitaj to
   // qDebug() << "b1)parent: ";
   // if(child != parent){
        int pocet = 1;
        while( (nextElementAST()->getType() != child) || (rozbalenie_child != lua_tonumber(L, -3)) || (hlbka_child != lua_gettop(L)) ){
           // qDebug() << "b1): number:" << lua_tonumber(L, -3) << " top:" << lua_gettop(L);
           // stackDump(L);
                pocet++;
        }
        glob_index = zac_glob_index - pocet;
        //nastavime sa zase na parenta
        lua_pop(L,3);       //check pushnumber?
        lua_pushnil(L);
        lua_next(L,-2);
//        qDebug() << "b2)parent: " << pocet;
//        stackDump(L);
        lua_pop(L,1);
//    }else{
//    }
    //////////////////////////////////////////////

    return parent;
}

/*---OLD--- nefunguje dobre, lebo rozbaluje prvky
TreeElement* Analyzer::getParentElementAST(){
    qDebug() << "getParentElementAST()";

    TreeElement* parent;
    QString nodeName;

    if(lua_isnumber(L,-1)){         //zisti ci naozaj treba podmienku???
        nodeName = getParentAST();
    }

    bool paired = false;
    if (pairedTokens.indexOf(nodeName, 0) >= 0) //! pairing needed
    {
        paired = true;
    }
    parent = new TreeElement(nodeName,
                       selectableTokens.contains(nodeName),
                       multiTextTokens.contains(nodeName),
                       false, paired);
    parent->local_index = glob_index;

    if (floatingTokens.contains(nodeName))
        parent->setFloating(true);
    parent->analyzer = this;

    return parent;
}

QString Analyzer::getParentAST(){
    QString parent;

    if(lua_isstring(L,-1) && !lua_isnumber(L,-1)){
//        stackDump(L);
        parent = lua_tostring(L, -1);
//        qDebug() << "parent: " << parent;
//        lua_pop(L, 3);
//        lua_pushnumber(L, 1);
//        stackDump(L);
    }else{
        if(lua_istable(L, -4)){
            lua_pushnil(L);
        }
        lua_next(L, -5);
        parent = getParentAST();
        lua_pop(L, 2);
        //lua_pop(L, 5);
        //lua_pushnumber(L, 1);
    }
    return parent;
}
*/

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
