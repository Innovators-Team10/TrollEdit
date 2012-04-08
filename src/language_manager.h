/**
 * language_manager.h
 *  ---------------------------------------------------------------------------
 * Contains the declaration of class LanguageManager and it's funtions and identifiers
 *
 */
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

    QString getLanguage(QString extens);

    QString snippetFile;

    QHash<QString, QString> languages;          //! <language_name, file_extension>
    Analyzer *getAnalyzerFor(QString suffix) const;
    Analyzer *getAnalyzerForLang(QString language) const;
    QList<QPair<QString, QHash<QString, QString> > > getConfigData();
    QStringList getLanguages() const;

private:
    QString programPath;
    QHash<QString, Analyzer *> analyzers;       //! <file_extension, analyzer>
    Analyzer *defaultAnalyzer;
    QList<QPair<QString, QHash<QString, QString> > > configData;
};

#endif // LANGUAGE_MANAGER_H
