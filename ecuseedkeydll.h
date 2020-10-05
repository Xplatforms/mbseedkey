#ifndef ECUSEEDKEYDLL_H
#define ECUSEEDKEYDLL_H

#include <QObject>
#include <QString>
#include <QHash>
#include <Windows.h>

typedef int (__cdecl *_f_GetConfiguredAccessTypes)(int *);
typedef int (__cdecl *_f_GetSeedLength)(int);
typedef int (__cdecl *_f_GetKeyLength)(int);
typedef const char * (__cdecl *_f_GetECUName)();
typedef const char * (__cdecl *_f_GetComment)();

typedef int (__cdecl * _f_GenerateKeyExOpt )( const unsigned char* ipSeedArray,
                                                                  unsigned int iSeedArraySize,
                                                                  const unsigned int iSecurityLevel,
                                                                  const char * ipVariant,
                                                                  const char* ipOptions,
                                                                  unsigned char* iopKeyArray,
                                                                  unsigned int iMaxKeyArraySize,
                                                                  unsigned int& oActualKeyArraySize );

class ECUSeedKeyDLL : public QObject
{
    Q_OBJECT
public:
    explicit ECUSeedKeyDLL(QString dll_path, QObject *parent = nullptr);
    ~ECUSeedKeyDLL();
    
    Q_INVOKABLE QList<qint32> GenerateKeyFromSeed(QList<qint32> seed, qint32 access_type);

private slots:
    void loadDllfuncs();

private:
    _f_GetConfiguredAccessTypes     GetConfiguredAccessTypes;
    _f_GetSeedLength                GetSeedLength;
    _f_GetKeyLength                 GetKeyLength;
    _f_GetECUName                   GetECUName;
    _f_GetComment                   GetComment;
    _f_GenerateKeyExOpt             GenerateKeyExOpt;

private:
    HMODULE     p_dllHandle;
    QString     p_dllPath;

    QString     p_errorMsg;
    QString     p_ecu_name;
    QString     p_comment;
};


#endif // ECUSEEDKEYDLL_H
