#include "language_manager.h"
#include "analyzer.h"
#include <QDir>
#include <QErrorMessage>
#include <QMessageBox>

const QString LanguageManager::GRAMMAR_DIR = "\\grammars";
const QString LanguageManager::DEFAULT_GRAMMAR = "\\grammars\\default_grammar.lua";

LanguageManager::LanguageManager(QString programPath)
{
    QStringList files;
    QDir dir = QDir(programPath + GRAMMAR_DIR);
    files = dir.entryList(QStringList("*.lua"), QDir::Files | QDir::NoSymLinks);
    foreach (QString file, files) {
        try {
            Analyzer *a = new Analyzer(programPath + GRAMMAR_DIR+ "\\" + file);
            analyzers.insert(a->getExtension(), a);
        } catch(...) {
            // analyzer is not inserted, messages were already displayed in Analyzer class
        }
    }
    defaultAnalyzer = new Analyzer(programPath + DEFAULT_GRAMMAR);
}

LanguageManager::~LanguageManager()
{
    qDeleteAll(analyzers);
    delete defaultAnalyzer;
}

Analyzer *LanguageManager::getAnalyzerFor(QString suffix) const {
    if (analyzers.contains(suffix))
        return analyzers.value(suffix);
    else
        return defaultAnalyzer;
}
