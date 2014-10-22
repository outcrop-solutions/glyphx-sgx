#ifndef ANTZGUI_GLOBAL_H
#define ANTZGUI_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef ANTZGUI_LIB
# define ANTZGUI_EXPORT Q_DECL_EXPORT
#else
# define ANTZGUI_EXPORT Q_DECL_IMPORT
#endif

#endif // ANTZGUI_GLOBAL_H
