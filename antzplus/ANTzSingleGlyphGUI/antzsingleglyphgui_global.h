#pragma once
#ifndef ANTZSINGLEGLYPHGUI_GLOBAL_H
#define ANTZSINGLEGLYPHGUI_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef ANTZSINGLEGLYPHGUI_LIB
# define ANTZSINGLEGLYPHGUI_EXPORT Q_DECL_EXPORT
#else
# define ANTZSINGLEGLYPHGUI_EXPORT Q_DECL_IMPORT
#endif

#endif // ANTZSINGLEGLYPHGUI_GLOBAL_H