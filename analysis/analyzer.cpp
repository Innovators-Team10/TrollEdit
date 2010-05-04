#include "analyzer.h"
#include "tree_element.h"

const char *Analyzer::EXTENSION_FIELD = "extension";
const char *Analyzer::MAIN_GRAMMAR_FIELD = "full_grammar";
const char *Analyzer::SUB_GRAMMARS_FIELD = "other_grammars";
const char *Analyzer::PAIRED_TOKENS_FIELD = "paired";
const char *Analyzer::SELECTABLE_TOKENS_FIELD = "selectable";
const char *Analyzer::MULTI_TEXT_TOKENS_FIELD = "multi_text";
const char *Analyzer::FLOATING_TOKENS_FIELDS = "floating";
const char *Analyzer::CONFIG_KEYS_FIELD = "cfg_keys";
const QString Analyzer::TAB = "    ";

QString exception;

Analyzer::Analyzer(QString script_name)
{
    msgBox = new QMessageBox();
    this->script_name = script_name;
    L = lua_open();             // initialize Lua
    luaL_openlibs(L);           // load Lua base libraries
    try {
        setupConstants();
    } catch (QString exMsg) {
        msgBox->critical(0, "Script error", exMsg,QMessageBox::Ok,QMessageBox::NoButton);
    }
    lua_close(L);
}

void Analyzer::setupConstants()
{
    if (luaL_loadfile(L, qPrintable(script_name))
        || lua_pcall(L, 0, 0, 0)) {
        throw "Error loading script \"" + script_name + "\"";
    }

    // get extension
    lua_getglobal (L, EXTENSION_FIELD);
    extension = QString(lua_tostring(L, -1));
    lua_pop(L, 1);

    // get grammars
    lua_getglobal (L, MAIN_GRAMMAR_FIELD);
    mainGrammar = QString(lua_tostring(L, -1));
    lua_pop(L, 1);
    if (mainGrammar.isEmpty())
        throw "No main grammar specified in script \"" + script_name + "\"";

    lua_getglobal (L, SUB_GRAMMARS_FIELD);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        subGrammars[QString(lua_tostring(L, -2))] = QString(lua_tostring(L, -1));
        lua_pop(L, 1);
    }

    // get paired tokens (e.g BEGIN/END, {/}, </>)
    lua_getglobal (L, PAIRED_TOKENS_FIELD);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        pairedTokens.append(QString(lua_tostring(L, -1)));
        lua_pop(L, 1);
    }

    // get selectable tokens (can be selected and moved)
    lua_getglobal (L, SELECTABLE_TOKENS_FIELD);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        selectableTokens.append(QString(lua_tostring(L, -1)));
        lua_pop(L, 1);
    }

    // get multi-text tokens (can contain more lines of text)
    lua_getglobal (L, MULTI_TEXT_TOKENS_FIELD);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        multiTextTokens.append(QString(lua_tostring(L, -1)));
        lua_pop(L, 1);
    }

    // get floating tokens
    lua_getglobal (L, FLOATING_TOKENS_FIELDS);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        floatingTokens.append(QString(lua_tostring(L, -1)));
        lua_pop(L, 1);
    }
}

Analyzer::~Analyzer()
{
}

QString Analyzer::getExtension() const
{
    return extension;
}
QList<QPair<QString, QHash<QString, QString> > > Analyzer::readConfig(QString fileName)
{
    L = lua_open();
    luaL_openlibs(L);
    QList<QPair<QString, QHash<QString, QString> > > tables;
    try {
        if (luaL_loadfile(L, qPrintable(fileName))
            || lua_pcall(L, 0, 0, 0)) {
            throw "Error loading script \"" + fileName + "\"";
        }
        QStringList keys;

        // get keys
        lua_getglobal (L, CONFIG_KEYS_FIELD);
        lua_pushnil(L);
        while (lua_next(L, -2) != 0) {
            keys.append(QString(lua_tostring(L, -1)));
            lua_pop(L, 1);
        }

        // get values
        foreach (QString key, keys) {
            lua_getglobal (L, qPrintable(key));
            lua_pushnil(L);
            QHash<QString, QString> pairs;
            while (lua_next(L, -2) != 0) {
                QString key = QString(lua_tostring(L, -2));
                QString value = QString(lua_tostring(L, -1));
                lua_pop(L, 1);
                pairs[key] = value;
            }
            tables << QPair<QString, QHash<QString, QString> >(key, pairs);
        }
        lua_close(L);
        return tables;
    } catch (QString exMsg) {
        msgBox->critical(0, "Config file error", exMsg,QMessageBox::Ok,QMessageBox::NoButton);
        lua_close(L);
        return tables;
    }
}

// analyze string by provided grammar
TreeElement *Analyzer::analyzeString(QString grammar, QString input)
{
    L = lua_open();
    luaL_openlibs(L);

    luaL_dofile(L, qPrintable(script_name));  // load the script
    lua_getglobal (L, "lpeg");        // table to be indexed
    lua_getfield(L, -1, "match");                     // function to be called: 'lpeg.match'
    lua_remove(L, -2);                                // remove 'lpeg' from the stack
    lua_getglobal (L, qPrintable(grammar));      // 1st argument
    lua_pushstring(L, qPrintable(input));       // 2nd argument
    int err = lua_pcall(L, 2, 1, 0);                  // call with 2 arguments and 1 result, no error function
    if (err != 0) {
        throw "Error in grammar \"" + grammar + "\" in script \"" + script_name + "\"";
    }

    TreeElement *root = 0;
    if(lua_istable(L, -1)) {
        root = createTreeFromLuaStack();
    }
    lua_close(L);

    processWhites(root);
    return root;
}

// analyze string, creates AST and returns root
TreeElement* Analyzer::analyzeFull(QString input)
{
    try {
        TreeElement *root = analyzeString(mainGrammar, input);
        root->setFloating();
        return root;
    } catch(QString exMsg) {
        msgBox->critical(0, "Runtime error", exMsg,QMessageBox::Ok,QMessageBox::NoButton);
        return 0;
    }
}

// reanalyze text from element and it's descendants, updates AST and returns first modified node
TreeElement *Analyzer::analyzeElement(TreeElement* element)
{
    QString grammar = "";
    TreeElement *subRoot = 0;
    if (element != 0)
        grammar = subGrammars[element->getType()];
    if (!grammar.isEmpty()) {
        try{
            subRoot = analyzeString(grammar, element->getText());
        } catch(QString exMsg) {
            msgBox->critical(0, "Runtime error", exMsg,QMessageBox::Ok,QMessageBox::NoButton);
            subRoot = 0;
        }
    }
    return subRoot;
}

TreeElement *Analyzer::getAnalysableAncestor(TreeElement *element)
{
    while (element->getParent() != 0) {
        if (subGrammars.contains(element->getType())) {
            break;
        }
        element = element->getParent();
    }
    if (element->getParent() == 0)
        return 0;
    else
        return element;
}

// creates AST from recursive lua tables (from stack), returns root(s)
TreeElement *Analyzer::createTreeFromLuaStack()
{
    TreeElement *root = 0;
    lua_pushnil(L);               // first key
    while (lua_next(L, -2) != 0) {// uses 'key' (at index -2) and 'value' (at index -1)
        if(lua_istable(L, -1)) {
            TreeElement *child = createTreeFromLuaStack();
            if (root == 0) { // should not happen when tables are properly nested
                root = child;
            } else {
                root->appendChild(child);
                checkPairing(child);
            }
        } else {
            QString nodeName = QString(lua_tostring(L, -1));
            bool paired = false;
            if (pairedTokens.indexOf(nodeName, 0) >= 0) { // pairing needed
                paired = true;
            }
            root = new TreeElement(nodeName,
                                   selectableTokens.contains(nodeName),
                                   multiTextTokens.contains(nodeName),
                                   false, paired);
            if (floatingTokens.contains(nodeName))
                root->setFloating(true);
        }
        lua_pop(L, 1); // removes 'value'; keeps 'key' for next iteration
    }
    return root;
}

void Analyzer::checkPairing(TreeElement *closeEl)
{
    QString nodeName = closeEl->getType();
    int pairIndex = pairedTokens.indexOf(nodeName, 0);
    if (pairIndex >= 0) {               // pairing needed

        if (pairIndex % 2 == 1) {   // closing element found
            QList<TreeElement *> siblings = closeEl->getParent()->getChildren();
            siblings.removeOne(closeEl);
            int index = siblings.size()-1;

            QString openString = pairedTokens[pairIndex-1];
            while (index >= 0) {
                // find closest matching unused opening element
                TreeElement *openEl = siblings[index];
                if (openEl->getType() == openString) {    // is matching?
                    if (openEl->getPair() == 0) {  // is unused?
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

void Analyzer::processWhites(TreeElement* root)
{
    QList<TreeElement*> whites;
    QList<TreeElement*> newlines;
    TreeElement *element = root;
    while (element->hasNext()) {
        element = element->next();
        if (element->isWhite())
            whites << element->getParent();
        if (element->isNewline())
            newlines << element->getParent();
    }
    // process newlines: shift right as far as possible, remove and set lineBreaking flag
    while (!newlines.isEmpty()) {
        TreeElement *el = newlines.takeLast();  // list is traversed backwards
        TreeElement *parent = el->getParent();
        int index;
        if (parent != 0) {
            index = parent->indexOfChild(el) - 1;   // -1 because el will be removed before checking
            bool isLast = !(*el)[0]->hasNext();
            parent->removeChild(el);   // remove & destroy newline element
            delete el;
            if (isLast) continue;     // ignore newlines at the end of file

            while(index == parent->childCount()-1) {// el was the last child
                if (parent->getParent() == 0)
                    break;
                index = parent->index();
                parent = parent->getParent();
            }
            TreeElement *nl = 0;
            if (index < 0) { // add an empty line-breaking element at index = 0;
                index = 0;
                nl = new TreeElement("", false, false, true);
            } else {
                TreeElement *el = (*parent)[index];
                while (!el->isImportant())
                    el = (*el)[0];
                if (!el->setLineBreaking(true)) { // set newline flag
                    // flag was already set -> add an empty line-breaking element at index+1
                    index++;
                    nl = new TreeElement("", false, false, true);
                }
            }
            if (nl != 0) {
                parent->insertChild(index, nl);
            }
        }
    }
    // process other whites: shift left as far as possible, don't shift when in line-breaking element
    foreach (TreeElement *el, whites) {
        TreeElement *parent = el->getParent();
        // substitute tabs
        (*el)[0]->setType((*el)[0]->getType().replace("\t", TAB));
        int index;
        if (parent != 0) {
            index = parent->indexOfChild(el);
            int spaces = (*el)[0]->getType().length();
            parent->removeChild(el);        // remove & destroy
            delete el;

            while (index == 0) {       // el was the first child
                index = parent->index();
                if (parent->getParent() == 0)
                    break;
                parent = parent->getParent();
            }
            if (index >= 0) {
                if (parent->childCount() <= index) continue;
                el = (*parent)[index];
            } else {
                el = parent;
            }
            while (!el->isImportant())
                el = (*el)[0];
            el->setSpaces(spaces);
        }
    }
    root->adjustSpaces(0);
}
