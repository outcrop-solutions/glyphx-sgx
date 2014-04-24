#ifndef SGXGUI_GLOBAL_H
#define SGXGUI_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef SGXGUI_LIB
# define SGXGUI_EXPORT Q_DECL_EXPORT
#else
# define SGXGUI_EXPORT Q_DECL_IMPORT
#endif

#endif // SGXGUI_GLOBAL_H
