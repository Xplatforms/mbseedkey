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

struct ECUSeedKeyLenPairs
{
    qint32 seed_len;
    qint32 key_len;
}typedef ECUSeedKeyLenPairs;

class ECUSeedKeyDLL : public QObject
{
    Q_OBJECT
public:
    explicit ECUSeedKeyDLL(QObject *parent = nullptr);
    explicit ECUSeedKeyDLL(QString dll_path, QObject *parent = nullptr);
    ~ECUSeedKeyDLL();

    bool isSeedKeyDll(){return this->GenerateKeyExOpt != Q_NULLPTR;}

    Q_PROPERTY(QString ECUName READ ECUName NOTIFY ECUNameChanged);
    Q_PROPERTY(QString DLLName READ DLLName NOTIFY DLLNameChanged);
    Q_PROPERTY(QString Comment READ Comment NOTIFY CommentChanged);
    Q_PROPERTY(QList<qint32> AccessTypes READ AccessTypes NOTIFY AccessTypesChanged);
    Q_PROPERTY(QString AccessTypesString READ AccessTypesString NOTIFY AccessTypesChanged);
    Q_PROPERTY(QString errorMsg READ errorMsg WRITE setErrorMsg NOTIFY errorMsgChanged);

    QString DLLName() const {return this->p_dll_name;}
    QString ECUName() const {return this->p_ecu_name;}
    QString Comment() const {return this->p_comment;}
    QList<qint32> AccessTypes() {return this->p_access_types.keys();}
    QString AccessTypesString();
    QString errorMsg(){return this->p_errorMsg;}
    void setErrorMsg(const QString & msg ){if(this->p_errorMsg.compare(msg) == 0)return; this->p_errorMsg = msg; emit errorMsgChanged();}

    Q_INVOKABLE qint32 SeedLength(qint32 access_type) {return this->p_access_types.contains(access_type)?
                    this->p_access_types.value(access_type).seed_len:4; }
    Q_INVOKABLE qint32 KeyLength(qint32 access_type) {return this->p_access_types.contains(access_type)?
                    this->p_access_types.value(access_type).key_len:4; }

    Q_INVOKABLE QList<qint32> GenerateKeyFromSeed(QList<qint32> seed, qint32 access_type);

signals:
    void ECUNameChanged();
    void DLLNameChanged();
    void CommentChanged();
    void SeedLengthChanged();
    void KeyLengthChanged();
    void AccessTypesChanged();
    void errorMsgChanged();

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
    QString     p_dll_name;
    QString     p_comment;

    QHash<qint32, ECUSeedKeyLenPairs> p_access_types;
};

#endif // ECUSEEDKEYDLL_H
