#ifndef SGXZSPACEGUI_GLOBAL_H
#define SGXZSPACEGUI_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef SGXZSPACEGUI_LIB
# define SGXZSPACEGUI_EXPORT Q_DECL_EXPORT
#else
# define SGXZSPACEGUI_EXPORT Q_DECL_IMPORT
#endif

#endif // SGXZSPACEGUI_GLOBAL_H