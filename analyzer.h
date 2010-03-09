#ifndef ANALYZER_H
#define ANALYZER_H

#include <QList>
#include <QString>
#include <QMessageBox>
#include <QMap>
#include "tree_element.h"
#include "paired_tree_element.h"

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
    TreeElement* analyzeFull(QString input);
    TreeElement* analyzeElement(TreeElement *source);
//    QString formatTree(TreeElement *source);
    QString getExtension() const;
private:
    static const char *EXTENSION_FIELD;
    static const char *MAIN_GRAMMAR_FIELD;
    static const char *SUB_GRAMMARS_FIELD;
    static const char *PAIRED_TOKENS_FIELD;
    static const QString TAB;

    lua_State *L;           // the Lua interpreter
    QString extension;      // type of files to be analyzed
    QString file_name;      // script name
    QString mainGrammar;    // name of complete gramar
    QMap<QString, QString> subGrammars; // names of partial grammars
    QStringList pairedTokens;   // list, example: "{", "}", "begin", "end"...

    void setupConstants();
    TreeElement* analyzeString(QString grammar, QString input);
    TreeElement* createTreeFromLuaStack();
    void checkPairing(TreeElement *element);

    QMessageBox *msgBox; // for (error) mesasage
};

#endif // ANALYZER_H
