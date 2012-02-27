#include "language_manager.h"
#include "analyzer.h"
#include <QDir>
#include <QErrorMessage>
#include <QMessageBox>
#include <QDebug>

const QString GRAMMAR_DIR = "/../share/trolledit/grammars";
const QString DEFAULT_GRAMMAR = "/../share/trolledit/grammars/default_grammar.lua";
const QString CONFIG_FILE = "/../share/trolledit/grammars/config.lua";
const QString SNIPPET_FILE = "/../share/trolledit/grammars/snippets.lua";

LanguageManager::LanguageManager(QString programPath)
{
    QFileInfoList grammars;
    QDir dir = QDir(programPath + GRAMMAR_DIR);
    grammars = dir.entryInfoList(QStringList("*.lua"), QDir::Files | QDir::NoSymLinks);
    QFileInfo defaultGrammar(programPath + DEFAULT_GRAMMAR);
    QFileInfo configFile(programPath + CONFIG_FILE);
    QFileInfo snippetFile(programPath + SNIPPET_FILE);

    foreach (QFileInfo file, grammars)
    {
        if (file != defaultGrammar && file != configFile && file != snippetFile)
        {
            try
            {
                Analyzer *a = new Analyzer(file.absoluteFilePath());
                a->readSnippet(snippetFile.absoluteFilePath());
                QStringList extensions = a->getExtensions();

                foreach (QString ext, extensions)
                {
                    analyzers.insert(ext, a);
                }

                languages.insert(a->getLanguageName(), extensions.first());
            }
            catch(...)
            {
                // analyzer is not inserted, messages were already displayed in Analyzer class
            }
        }
    }
    defaultAnalyzer = new Analyzer(defaultGrammar.absoluteFilePath());
    languages.insert(defaultAnalyzer->getLanguageName(), defaultAnalyzer->getExtensions().first());

    // TODO: we assume grammars are there; but if there is none, what happens?
    //       make some test for this case and throw an exception, catch it in main
    //       window and terminate program; we have to have at least one grammar - ideally default

    configData = defaultAnalyzer->readConfig(configFile.absoluteFilePath());
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

Analyzer *LanguageManager::getAnalyzerForLang(QString language) const
{
    qDebug() << "getAnalyzerForLang";
    return getAnalyzerFor(languages.value(language));
}

QList<QPair<QString, QHash<QString, QString> > > LanguageManager::getConfigData()
{
    return configData;
}

QStringList LanguageManager::getLanguages() const
{
    QStringList lang(languages.keys());
    lang.removeOne(defaultAnalyzer->getLanguageName());
    lang.sort();
    lang.append(defaultAnalyzer->getLanguageName());

    return lang;
}
