#ifndef LANGUAGE_MANAGER_H
#define LANGUAGE_MANAGER_H

#include<QString>
#include<QHash>

class Analyzer;

class LanguageManager
{
public:
    LanguageManager(QString programPath);
    ~LanguageManager();

    Analyzer *getAnalyzerFor(QString suffix) const;
    QHash<QString, QHash<QString, QString> > getConfigData();

private:
    static const QString GRAMMAR_DIR;
    static const QString DEFAULT_GRAMMAR;
    static const QString CONFIG_FILE;
    QHash<QString, Analyzer *> analyzers;
    Analyzer *defaultAnalyzer;
    QHash<QString, QHash<QString, QString> > configData;
};

#endif // LANGUAGE_MANAGER_H
