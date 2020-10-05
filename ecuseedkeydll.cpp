#include "ecuseedkeydll.h"
#include <QDir>
#include <QFileInfo>
#include <QTimer>
#include <QDebug>
#include <signal.h>

auto tmp_ret_def_seedkey_len = [](int){return 4;};

QString GetLastErrorAsString()
{
    //Get the error message, if any.
    auto errorMessageID = ::GetLastError();
    if(errorMessageID == 0)return QStringLiteral(""); //No error message has been recorded

    LPWSTR messageBuffer = Q_NULLPTR;
    size_t size = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), messageBuffer, 0, NULL);

    auto message = QString::fromWCharArray(messageBuffer, size);
    LocalFree(messageBuffer);

    return message;
}

ECUSeedKeyDLL::ECUSeedKeyDLL(QObject *parent) : QObject(parent),
    GetConfiguredAccessTypes(Q_NULLPTR), GetSeedLength(Q_NULLPTR), GetKeyLength(Q_NULLPTR),  GetECUName(Q_NULLPTR),
    GenerateKeyExOpt(Q_NULLPTR),
    p_dllHandle(Q_NULLPTR)
{

}

ECUSeedKeyDLL::ECUSeedKeyDLL(QString dll_path, QObject *parent) : QObject(parent),
    GetConfiguredAccessTypes(Q_NULLPTR), GetSeedLength(Q_NULLPTR), GetKeyLength(Q_NULLPTR),  GetECUName(Q_NULLPTR),
    GenerateKeyExOpt(Q_NULLPTR),
    p_dllHandle(Q_NULLPTR),
    p_dllPath(dll_path)
{
    if(dll_path.isEmpty())
    {
        this->setErrorMsg(tr("DLL path is empty!"));
        return;
    }

    QFileInfo info(this->p_dllPath);
    if(!info.exists() || !info.isFile() || info.isSymLink())
    {
        this->setErrorMsg(tr("DLL file doesn't exist or wrong."));
        return;
    }

    this->p_ecu_name = info.baseName();
    this->p_dll_name = info.fileName();
    emit DLLNameChanged();

    QString dll_info_str = "[" + info.fileName() + "] ";
    qInfo() <<  dll_info_str << "Loading ...";

    this->p_dllHandle = LoadLibraryW(QDir::toNativeSeparators(info.absoluteFilePath()).toStdWString().data());
    if(!this->p_dllHandle)
    {
        auto error_str = GetLastErrorAsString();
        this->setErrorMsg(tr("Could not load DLL file. Reason: ")+error_str);
        qInfo() << dll_info_str << this->errorMsg();
        return;
    }    
    this->loadDllfuncs();
}

ECUSeedKeyDLL::~ECUSeedKeyDLL()
{
    if(this->p_dllHandle != Q_NULLPTR)::FreeLibrary(this->p_dllHandle);
}

void ECUSeedKeyDLL::loadDllfuncs()
{

    this->GetECUName = (_f_GetECUName)GetProcAddress(this->p_dllHandle, "GetECUName");
    if(this->GetECUName != Q_NULLPTR)
    {
        this->p_ecu_name = this->GetECUName();
        emit this->ECUNameChanged();
    }

    this->GetComment = (_f_GetComment)GetProcAddress(this->p_dllHandle, "GetComment");
    if(this->GetComment != Q_NULLPTR)
    {
        this->p_comment = this->GetComment();
        emit this->CommentChanged();
    }

    this->GetSeedLength = (_f_GetSeedLength)GetProcAddress(this->p_dllHandle, "GetSeedLength");
    if(!this->GetSeedLength)
    {
       qDebug() << "Function GetSeedLength not found";
       this->GetSeedLength = (_f_GetSeedLength)&tmp_ret_def_seedkey_len;
    }

    this->GetKeyLength = (_f_GetKeyLength)GetProcAddress(this->p_dllHandle, "GetKeyLength");
    if (!this->GetKeyLength)
    {
       qDebug() << "Function GetKeyLength not found";
       this->GetKeyLength = (_f_GetKeyLength)&tmp_ret_def_seedkey_len;
    }

    this->GetConfiguredAccessTypes = (_f_GetConfiguredAccessTypes)GetProcAddress(this->p_dllHandle, "GetConfiguredAccessTypes");
    if(this->GetConfiguredAccessTypes != Q_NULLPTR)
     {
        int data[256] = {0};
        auto ret = this->GetConfiguredAccessTypes(data);
        if(ret > 0)
        {
            do
            {
                auto atype = data[--ret];
                ///WARNING: Seed and Key length functions could not be called in paralell or asynchrone!
                /// make sure compiler can't optimize this calls and call this funcs one by one
                ECUSeedKeyLenPairs skpair;
                skpair.seed_len = this->GetSeedLength(atype);
                skpair.key_len  = this->GetKeyLength(atype);
                //qDebug() << " seed len: " << skpair.seed_len << " keylen " << skpair.key_len;
                this->p_access_types.insert(atype, skpair);
            }
            while(ret > 0);
            emit this->AccessTypesChanged();

        }
     }

    this->GenerateKeyExOpt = (_f_GenerateKeyExOpt)GetProcAddress(this->p_dllHandle, "GenerateKeyExOpt");
    if(this->GenerateKeyExOpt == Q_NULLPTR)
    {
        this->setErrorMsg(tr("GenerateKeyExOpt not found in DLL ") + this->p_dllPath);
        qWarning() << this->errorMsg();
        return;
    }
}

QString ECUSeedKeyDLL::AccessTypesString()
{
    if(this->AccessTypes().isEmpty())return QStringLiteral("not defined in dll");
    auto types = this->AccessTypes();
    QString str;
    foreach(auto t, types)
    {
        str.append(QString::number(t) + " ");
    }
    return str;
}

QList<qint32> ECUSeedKeyDLL::GenerateKeyFromSeed(QList<qint32> seed, qint32 access_type)
{
    QList<qint32> key;
    if(this->GenerateKeyExOpt == Q_NULLPTR)
    {
        qWarning() << this->errorMsg();
        return key;
    }

    unsigned char * seed_data = new unsigned char[seed.length()];
    auto i = 0;
    foreach(auto d, seed)seed_data[i++] = d;

    unsigned char key_data[256] = {0};
    unsigned int key_data_len = 0;

    ///FIX: some DLL's return zero but key is filled out
    auto ret = this->GenerateKeyExOpt(seed_data, seed.length(), access_type, Q_NULLPTR, Q_NULLPTR, key_data, 256,  key_data_len);
    Q_UNUSED(ret)
    if(key_data_len <= 0 && (key_data[0] != 0 && key_data[1] != 0))
    {
        qWarning() << "GenerateKeyExOpt returned zero size, but data buf is set. Try to copy data";
        key_data_len = this->GetKeyLength(access_type);
    }

    unsigned int c = 0;
    do
    {
        key.append(key_data[c++]);
    }
    while(c < key_data_len);
    return key;
}
