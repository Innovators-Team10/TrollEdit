#ifndef ANALYZER_H
#define ANALYZER_H

#include <QList>
#include <QString>
#include <QMessageBox>
#include <QMap>

class TreeElement;
class PairedTreeElement;

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
private:
    static const char *EXTENSION_FIELD;
    static const char *MAIN_GRAMMAR_FIELD;
    static const char *SUB_GRAMMARS_FIELD;
    static const char *PAIRED_TOKENS_FIELD;
    static const char *MULTI_LINE_TOKENS_FIELD;
    static const char *MULTI_TEXT_TOKENS_FIELD;
    static const QString TAB;

    lua_State *L;           // the Lua interpreter
    QString extension;      // type of files to be analyzed
    QString file_name;      // script name
    QString mainGrammar;    // name of complete gramar
    QMap<QString, QString> subGrammars; // names of partial grammars
    QStringList pairedTokens;   // list, example: "{", "}", "begin", "end"...
    QStringList multiLineTokens;   // list of tokens which can contain line-breaking children
    QStringList multiTextTokens;   // list of tokens which can contain more lines of text

    void setupConstants();
    TreeElement* analyzeString(QString grammar, QString input);
    TreeElement* createTreeFromLuaStack();
    void checkPairing(TreeElement *element);

    void processWhites(TreeElement *element); // move all whites as high as possible without changing tree text
    void splitNewlines(TreeElement *element); // find all newlines in whites and split to independent elements

    QMessageBox *msgBox; // for (error) mesasage
};

#endif // ANALYZER_H
