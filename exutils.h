#ifndef EXUTILS_H
#define EXUTILS_H

#include <QObject>
#include <QSettings>
#include "ecuseedkeydll.h"

class ExUTILS : public QObject
{
    Q_OBJECT
public:
    explicit ExUTILS(QObject *parent = nullptr);

    Q_PROPERTY(QString foldername READ foldername WRITE setFoldername NOTIFY foldernameChanged)
    Q_PROPERTY(QStringList dlls READ dlls NOTIFY dllsChanged)
    Q_PROPERTY(ECUSeedKeyDLL * seedkeydll READ seedkeydll NOTIFY seedkeydllChanged)

    Q_INVOKABLE void selectDll(qint32 idx);

    QStringList dlls(){return this->dlls_list;}

    QString foldername();
    void setFoldername(QString fname);

    ECUSeedKeyDLL * seedkeydll();

    Q_INVOKABLE void searchFor(QString txt);

    Q_INVOKABLE QString getInputMaskForSeedLen(qint32 len);


signals:
    void foldernameChanged();
    void dllsChanged();
    void seedkeydllChanged();

private:
    void loadDllsList(QString search_txt = QStringLiteral(""));

private:
    QSettings *     p_sett;
    QStringList     dlls_list;
    QStringList     p_bak_dlls_list;
    ECUSeedKeyDLL * p_seedkey_dll;


};

#endif // EXUTILS_H
