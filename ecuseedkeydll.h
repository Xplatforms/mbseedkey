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



#endif // ECUSEEDKEYDLL_H
