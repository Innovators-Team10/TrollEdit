#include "analyzer.h"

const char *Analyzer::EXTENSION_FIELD = "extension";
const char *Analyzer::MAIN_GRAMMAR_FIELD = "full_grammar";
const char *Analyzer::SUB_GRAMMARS_FIELD = "other_grammars";
const char *Analyzer::PAIRED_TOKENS_FIELD = "paired";
const QString Analyzer::TAB = "    ";

QString exception;

Analyzer::Analyzer(QString script_name)
{
    msgBox = new QMessageBox();
    file_name = script_name;
    L = lua_open();             // initialize Lua
    luaL_openlibs(L);           // load Lua base libraries
    try {
        setupConstants();
    } catch (QString exMsg) {
        msgBox->critical(0, "Compilation error", exMsg,QMessageBox::Ok,QMessageBox::NoButton);
    }
}

void Analyzer::setupConstants()
{
    if (luaL_loadfile(L, qPrintable(file_name))
        || lua_pcall(L, 0, 0, 0)) {
        throw "Error loading script \"" + file_name + "\"";
    }

    // get extension
    lua_getfield(L, LUA_GLOBALSINDEX, EXTENSION_FIELD);
    extension = QString(lua_tostring(L, -1));
    lua_pop(L, 1);

    // get grammars
    lua_getfield(L, LUA_GLOBALSINDEX, MAIN_GRAMMAR_FIELD);
    mainGrammar = QString(lua_tostring(L, -1));
    lua_pop(L, 1);
    if (mainGrammar.isEmpty())
        throw "No main grammar specified in script \"" + file_name + "\"";

    lua_getfield(L, LUA_GLOBALSINDEX, SUB_GRAMMARS_FIELD);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        subGrammars[QString(lua_tostring(L, -2))] = QString(lua_tostring(L, -1));
        lua_pop(L, 1);
    }

    // get paired tokens (e.g BEGIN/END, {/}, </>)
    lua_getfield(L, LUA_GLOBALSINDEX, PAIRED_TOKENS_FIELD);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        pairedTokens.append(QString(lua_tostring(L, -1)));
        lua_pop(L, 1);
    }

/* // get tokens representing whitespace characters
    lua_getfield(L, LUA_GLOBALSINDEX, "white_chars");
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        whiteSpaces.append(QString(lua_tostring(L, -1)));
        lua_pop(L, 1);
    }

    // get token representing new line
    lua_getfield(L, LUA_GLOBALSINDEX, "new_line");
    endLineToken = QString(lua_tostring(L, -1));
    lua_pop(L, 1);*/
}

Analyzer::~Analyzer()
{
    lua_close(L);               // cleanup Lua
}

QString Analyzer::getExtension() const {
        return extension;
}

// analyze string by provided grammar
TreeElement *Analyzer::analyzeString(QString grammar, QString input)
{
    luaL_dofile(L, qPrintable(file_name));  // load the script
    lua_getfield(L, LUA_GLOBALSINDEX, "lpeg");        // table to be indexed
    lua_getfield(L, -1, "match");                     // function to be called: 'lpeg.match'
    lua_remove(L, -2);                                // remove 'lpeg' from the stack
    lua_getfield(L, LUA_GLOBALSINDEX, qPrintable(grammar));     // 1st argument
    lua_pushstring(L, qPrintable(input));   // 2nd argument
    int err = lua_pcall(L, 2, 1, 0);                  // call with 2 arguments and 1 result, no error function
    if (err != 0) {
        throw "Error in grammar \"" + grammar + "\" in script \"" + file_name + "\"";
    }

    TreeElement *root = NULL;
    if(lua_istable(L, -1)) {
        root = createTreeFromLuaStack();                    // print result
    }
    return root;
}

// analyze string, creates AST and returns root
TreeElement* Analyzer::analyzeFull(QString input)
{
    try {
        return analyzeString(mainGrammar, input);
    } catch(QString exMsg) {
        msgBox->critical(0, "Runtime error", exMsg,QMessageBox::Ok,QMessageBox::NoButton);
        return NULL;
    }
}

// reanalyze text from element and it's descendants, modifies AST and returns root
TreeElement *Analyzer::analyzeElement(TreeElement* source)
{
    TreeElement *element = source;
    QString grammar;
    while (element->getParent() != NULL) {
        if (subGrammars.contains(element->getType())) {
            grammar = subGrammars[element->getType()];
            break;
        }
        element = element->getParent();
    }
    if (element->getParent() != NULL && !grammar.isNull()) {
        TreeElement *parent = element->getParent();
        int index = (*parent)[element];
        parent->removeChild(element);
        try{
            TreeElement *subRoot = analyzeString(grammar, element->getText());
            parent->insertChild(index, subRoot);
        } catch(QString exMsg) {
            msgBox->critical(0, "Runtime error", exMsg,QMessageBox::Ok,QMessageBox::NoButton);
            return source;
        }
        delete element;
        return parent->getRoot();
    } else {
        TreeElement *root = analyzeFull(element->getRoot()->getText());
        delete element->getRoot();
        return root;
    }

}

int indentLevel = 0;

// formats text from tree, original whitespaces supressed
//QString Analyzer::formatTree(TreeElement *source)
//{
//    QString text;
//    if (source->isLeaf()) {
//        QString parentType = source->getParent()->getType();
//        if (whiteSpaces.contains(parentType)) text.append(" ");
//        else {
//            text.append(source->getText());
//            if (parentType == endLineToken) {
//                text.append("\n");
//                for(int i=0; i<indentLevel; i++) text.append(TAB);
//            } /*else if (parentType == indentTokens[0])
//                indentLevel++;
//            else if (parentType == indentTokens[1])
//                indentLevel--;*/
//        }
//    } else {
//        foreach (TreeElement *e, source->getChildren()) {
//            text.append(formatTree(e));
//        }
//
//    }
//    return text;
//}

// creates AST from recursive lua tables (from stack), returns root(s)
TreeElement *Analyzer::createTreeFromLuaStack()
{
    TreeElement *root = NULL;
    lua_pushnil(L);               // first key
    while (lua_next(L, -2) != 0) {// uses 'key' (at index -2) and 'value' (at index -1)
        if(lua_istable(L, -1)) {
            TreeElement *child = createTreeFromLuaStack();
            if (root == NULL) { // should not happen when tables are properly nested
                root = child;
            } else {
                root->appendChild(child);
                checkPairing(child);
            }
        } else {
            QString nodeName = QString(lua_tostring(L, -1));
            int pairIndex = pairedTokens.indexOf(nodeName, 0);

            if (pairIndex >= 0) {               // pairing needed
                root = new PairedTreeElement(nodeName);
            } else {
                root = new TreeElement(nodeName);
            }
        }
        lua_pop(L, 1); // removes 'value'; keeps 'key' for next iteration
    }
    return root;
}

void Analyzer::checkPairing(TreeElement *element)
{
    QString nodeName = element->getType();
    int pairIndex = pairedTokens.indexOf(nodeName, 0);
    if (pairIndex >= 0) {               // pairing needed
        PairedTreeElement *pairedEl = (PairedTreeElement *)element;

        if (pairIndex % 2 == 1) {   // closing element found
            QList<TreeElement *> siblings = pairedEl->getParent()->getChildren();
            siblings.removeOne(pairedEl);
            int index = siblings.size()-1;

            QString openString = pairedTokens[pairIndex-1];
            while (index >= 0) {
                // find closest matching unused opening element
                TreeElement *item = siblings[index];
                if (item->getType() == openString) {    // is matching?
                    PairedTreeElement *openEl = (PairedTreeElement *)siblings[index];
                    if (openEl->getPair() == NULL) {  // is unused?
                        openEl->setPair(pairedEl);
                        pairedEl->setPair(openEl);
                        break;
                    }
                }
                index--;
            }
        }
    }
}
