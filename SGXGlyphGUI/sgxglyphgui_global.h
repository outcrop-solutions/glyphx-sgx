#pragma once
#ifndef SGXGLYPHGUI_GLOBAL_H
#define SGXGLYPHGUI_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef SGXGLYPHGUI_LIB
# define SGXGLYPHGUI_EXPORT Q_DECL_EXPORT
#else
# define SGXGLYPHGUI_EXPORT Q_DECL_IMPORT
#endif

#endif // SGXGLYPHGUI_GLOBAL_H
