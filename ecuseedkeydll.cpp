#include "ecuseedkeydll.h"
#include <QDir>
#include <QFileInfo>
#include <QTimer>
#include <QDebug>
#include <signal.h>

QString GetLastErrorAsString()
{
    //Get the error message, if any.
    auto errorMessageID = ::GetLastError();
    if(errorMessageID == 0)return QStringLiteral("No error message has been recorded");

    LPWSTR messageBuffer = Q_NULLPTR;
    size_t size = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), messageBuffer, 0, NULL);

    qDebug() << " Debug errmsg: " << size << errorMessageID;
    auto message = QString::fromWCharArray(messageBuffer, size);
    if(message.isEmpty())message.append(QString::number(errorMessageID));
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
       this->GetSeedLength = Q_NULLPTR;
    }

    this->GetKeyLength = (_f_GetKeyLength)GetProcAddress(this->p_dllHandle, "GetKeyLength");
    if (!this->GetKeyLength)
    {
       qDebug() << "Function GetKeyLength not found";
       this->GetKeyLength = Q_NULLPTR;
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
    else qDebug() << "Function GetConfiguredAccessTypes not found";

    this->GenerateKeyExOpt = (_f_GenerateKeyExOpt)GetProcAddress(this->p_dllHandle, "GenerateKeyExOpt");
    if(this->GenerateKeyExOpt == Q_NULLPTR)
    {
        this->setErrorMsg(tr("GenerateKeyExOpt not found in DLL ") + this->p_dllPath);
        qWarning() << this->errorMsg();
        this->p_ecu_name = tr("not seedkey dll");
        emit ECUNameChanged();
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
        str = str.append(QStringLiteral("%1 ").arg(t, 2, 16, QLatin1Char('0'))).toUpper();
    }
    return str;
}

QStringList ECUSeedKeyDLL::AccessTypesStringList()
{
    QStringList list;
    foreach(auto a, this->AccessTypes())list << QStringLiteral("%1 ").arg(a, 2, 16, QLatin1Char('0')).toUpper();
    return list;
}

Q_INVOKABLE qint32 ECUSeedKeyDLL::seedLength(QString access_type)
{
    auto acc = access_type.toInt(Q_NULLPTR,16);
    if(this->p_access_types.contains(acc))
    {
        return this->p_access_types.value(acc).seed_len;
    }

    return 0;
}

Q_INVOKABLE qint32 ECUSeedKeyDLL::keyLength(QString access_type)
{
    auto acc = access_type.toInt(Q_NULLPTR,16);
    if(this->p_access_types.contains(acc))
    {
        return this->p_access_types.value(acc).key_len;
    }

    return 0;
}

QList<qint32> ECUSeedKeyDLL::generateKeyFromSeed(QList<qint32> seed, qint32 access_type, qint32 key_len)
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
        if(key_len != 0 || this->GetKeyLength == Q_NULLPTR)key_data_len = key_len;
        else key_data_len = this->GetKeyLength(access_type);
    }

    unsigned int c = 0;
    do
    {
        key.append(key_data[c++]);
    }
    while(c < key_data_len);

    //qDebug() << "[KEYDATA]: " << QByteArray::fromRawData((const char *)key_data, 255).toHex(' ');
    return key;
}

QString ECUSeedKeyDLL::generateKeyFromSeed(QString seed, QString access_type, QString key_len)
{
    return this->generateKeyFromSeed(seed, access_type.toInt(Q_NULLPTR, 16), key_len.toInt(Q_NULLPTR, 10));
}

QString ECUSeedKeyDLL::generateKeyFromSeed(QString seed, qint32 access_type, qint32 key_len)
{
    Q_UNUSED(key_len);
    QList<qint32> seed_list;
    foreach(auto el, seed.split(QChar(' '), Qt::SkipEmptyParts))
    {
        bool ok;
        auto hex = el.toInt(&ok,16);
        seed_list.append(hex);
    }
    auto key = this->generateKeyFromSeed(seed_list, access_type, key_len);
    QString key_str;
    while(!key.isEmpty()){
        auto hex = key.takeFirst();if(hex < 16)key_str.append('0');
        key_str.append(QString::number(hex, 16)).append(QChar(' '));
    }
    return key_str.trimmed().toUpper();
}
