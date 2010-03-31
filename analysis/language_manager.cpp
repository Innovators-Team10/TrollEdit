#include "language_manager.h"
#include "analyzer.h"
#include <QDir>
#include <QErrorMessage>
#include <QMessageBox>

const QString LanguageManager::GRAMMAR_DIR = "\\grammars";
const QString LanguageManager::DEFAULT_GRAMMAR = "\\grammars\\default_grammar.lua";
const QString LanguageManager::CONFIG_FILE = "\\grammars\\config.lua";

LanguageManager::LanguageManager(QString programPath)
{
    QFileInfoList grammars;
    QDir dir = QDir(programPath + GRAMMAR_DIR);
    grammars = dir.entryInfoList(QStringList("*.lua"), QDir::Files | QDir::NoSymLinks);
    QFileInfo defaultGrammar(programPath + DEFAULT_GRAMMAR);
    QFileInfo configFile(programPath + CONFIG_FILE);

    foreach (QFileInfo file, grammars) {
        if (file != defaultGrammar && file != configFile) {
            try {
                Analyzer *a = new Analyzer(file.absoluteFilePath());
                analyzers.insert(a->getExtension(), a);
            } catch(...) {
                // analyzer is not inserted, messages were already displayed in Analyzer class
            }
        }
    }
    defaultAnalyzer = new Analyzer(defaultGrammar.absoluteFilePath());

    // TODO: we assume grammars are there; but if there is none, what happens?
    //       make some test for this case and throw an exception, catch it in main
    //       window and terminate program; we have to have at least one grammar - ideally default

    configData = defaultAnalyzer->readFile(configFile.absoluteFilePath());
}

LanguageManager::~LanguageManager()
{
    qDeleteAll(analyzers);
    delete defaultAnalyzer;
}

Analyzer *LanguageManager::getAnalyzerFor(QString suffix) const
{
    if (analyzers.contains(suffix))
        return analyzers.value(suffix);
    else
        return defaultAnalyzer;
}

QMap<QString, QStringList> LanguageManager::getConfigData()
{
    return configData;
}
