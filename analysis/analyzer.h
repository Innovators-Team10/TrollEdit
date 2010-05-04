#ifndef ANALYZER_H
#define ANALYZER_H

#include <QMessageBox>
#include <QHash>
#include <QList>

class TreeElement;

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

class Analyzer
{
public:
    Analyzer(QString script_name);
    ~Analyzer();
    TreeElement *analyzeFull(QString input);
    TreeElement *analyzeElement(TreeElement *element);
    TreeElement *getAnalysableAncestor(TreeElement *element);
    QString getExtension() const;
    QList<QPair<QString, QHash<QString, QString> > > readConfig(QString fileName);
    static const QString TAB;

private:
    static const char *EXTENSION_FIELD;
    static const char *MAIN_GRAMMAR_FIELD;
    static const char *SUB_GRAMMARS_FIELD;
    static const char *PAIRED_TOKENS_FIELD;
    static const char *SELECTABLE_TOKENS_FIELD;
    static const char *MULTI_TEXT_TOKENS_FIELD;
    static const char *FLOATING_TOKENS_FIELDS;
    static const char *CONFIG_KEYS_FIELD;

    lua_State *L;           // the Lua interpreter
    QString extension;      // type of files to be analyzed
    QString script_name;      // script name
    QString mainGrammar;    // name of complete gramar
    QHash<QString, QString> subGrammars; // names of partial grammars
    QStringList pairedTokens;   // list, example: "{", "}", "begin", "end"...
    QStringList selectableTokens;  // list of tokens which can contain line-breaking children
    QStringList multiTextTokens;   // list of tokens which can contain more lines of text
    QStringList floatingTokens;    // list of tokenskj

    void setupConstants();
    TreeElement* analyzeString(QString grammar, QString input);
    TreeElement* createTreeFromLuaStack();
    void checkPairing(TreeElement *element);

    void processWhites(TreeElement *root); // move all whites as high as possible without changing tree text

    QMessageBox *msgBox; // for (error) mesasage
};

#endif // ANALYZER_H
