#ifndef LANGUAGE_MANAGER_H
#define LANGUAGE_MANAGER_H

#include <QString>
#include <QHash>
#include <QMap>

class Analyzer;

class LanguageManager
{
public:
    LanguageManager(QString programPath);
    ~LanguageManager();

    Analyzer *getAnalyzerFor(QString suffix) const;
    Analyzer *getAnalyzerForLang(QString language) const;
    QList<QPair<QString, QHash<QString, QString> > > getConfigData();
    QStringList getLanguages() const;

private:
    QHash<QString, Analyzer *> analyzers;       // <file_extension, analyzer>
    QHash<QString, QString> languages;          // <language_name, file_extension>
    Analyzer *defaultAnalyzer;
    QList<QPair<QString, QHash<QString, QString> > > configData;
};

#endif // LANGUAGE_MANAGER_H
