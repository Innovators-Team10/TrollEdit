#ifndef LANGUAGE_MANAGER_H
#define LANGUAGE_MANAGER_H

#include <QString>
#include <QHash>
#include "analyzer.h"

class LanguageManager
{
public:
    LanguageManager(QString programPath);
    ~LanguageManager();
    Analyzer *getAnalyzerFor(QString suffix) const;
private:
    static const QString GRAMMAR_DIR;
    static const QString DEFAULT_GRAMMAR;
    QHash<QString, Analyzer *> analyzers;
    Analyzer *defaultAnalyzer;
};

#endif // LANGUAGE_MANAGER_H
