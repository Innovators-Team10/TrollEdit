/**
 * analyzer.h
 *  ---------------------------------------------------------------------------
 * Contains the declaration of class Analyzer and it's funtions and identifiers
 *
 */

#ifndef ANALYZER_H
#define ANALYZER_H

#include <QMessageBox>
#include <QHash>
#include <QList>
#include <QDebug>

#include <QThreadPool>

class TreeElement;

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

class Analyzer
{
public:
    Analyzer(QString script);
    ~Analyzer();
    TreeElement *analyzeFull(QString input);
    TreeElement *analyzeElement(TreeElement *element);
    TreeElement *getAnalysableAncestor(TreeElement *element);
    QStringList getExtensions() const {return extensions;}
    QString getLanguageName() const {return langName;}
    QString getSnippet() const {return defaultSnippet;}
    QString queryMultilineSupport() const {return multilineSupport;}
    QHash<QString, QStringList> getCommentTokens() const {return commentTokens;}
    QList<QPair<QString, QHash<QString, QString> > > readConfig(QString fileName);
    void readSnippet(QString fileName);
    static const QString TAB;

    TreeElement* reanalyzeString(TreeElement *el, QString grammar, QString input); //! reanalyzovanie vybraneho textu
    TreeElement* nextElementAST();                  //! next()
    void nextElementAST_void();
    bool hasNextElementAST();                       //! hasNext()
    bool isLeafElementAST();                        //! isLeaf()
    int getCountElementChildrenAST();               //!
    QList<TreeElement*> getElementChildrenAST();    //!
    TreeElement* getParentElementAST();             //!
    void resetAST();                                //!
    TreeElement* setIndexAST(int deep, int* nodes); //!
    void checkLocationAST(int deep, int* nodes);    //! skontroluje lokaciu podla zadanych parametrov a nastavi sa na nu
    TreeElement* getElementAST();                   //! vrati Element z aktualnej pozicie
    TreeElement* getElementAST(int deep, int* nodes);//! vrati Element z urcenej pozicie
    bool checkLineBreak();                          //! skontroluje ci je sa ma dany Element zalomit na novy riadok
    int getDeepAST();                               //! vrati aktualnu hlbku AST
    int* getNodesAST();                             //! vrati pole index, ktore vedu k elementu
    int glob_deep_AST;                              //! hlbka Elementu v AST
    int* glob_nodes_AST;                            //! uzol v AST - musis si pametat postupnost rozbaleny - pole intov

    static const int DEFAULT_STACK_DEEP;

private:
    static const char *EXTENSIONS_FIELD;
    static const char *LANGUAGE_FIELD;
    static const char *MAIN_GRAMMAR_FIELD;
    static const char *SUB_GRAMMARS_FIELD;
    static const char *PAIRED_TOKENS_FIELD;
    static const char *SELECTABLE_TOKENS_FIELD;
    static const char *MULTI_TEXT_TOKENS_FIELD;
    static const char *FLOATING_TOKENS_FIELD;
    static const char *MULTILINE_SUPPORT_FIELD;
    static const char *LINE_COMMENT_TOKENS_FIELD;
    static const char *MULTILINE_COMMENT_TOKENS_FIELD;
    static const char *CONFIG_KEYS_FIELD;

    lua_State *L;               //! the Lua interpreter
    QStringList extensions;     //! types of files to be analyzed
    QString langName;           //! name/description of analyzed language(s)
    QString scriptName;        //! script name
    QString mainGrammar;        //! name of complete gramar
    QHash<QString, QString> subGrammars;//! names of partial grammars
    QStringList pairedTokens;           //! list of paired tokens, example: "{", "}", "begin", "end"...
    QStringList selectableTokens;       //! list of tokens which can contain line-breaking children
    QStringList multiTextTokens;        //! list of tokens which can contain more lines of text
    QStringList floatingTokens;         //! list of tokens allowed to say out of hierarchy
    QString defaultSnippet;             //! code that will be displayed in new file
    QString multilineSupport;           //! natural support of multiline comments
    QHash<QString, QStringList> commentTokens;      //! start & end tokens for comments

    void setupConstants();
    TreeElement* analyzeString(QString grammar, QString input);
    TreeElement* createTreeFromLuaStack();
    void checkPairing(TreeElement *element);

    void processWhites(TreeElement *root); //! move all whites as high as possible without changing tree text

    QString getChildAST();                          //! child of current element in AST
    QString getParentAST();                         //! parent of current element in AST

    QMessageBox *msgBox; //! for (error) mesasage
    
};

#endif // ANALYZER_H
