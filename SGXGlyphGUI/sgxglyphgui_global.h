#pragma once
#ifndef SGXGLYPHGUI_GLOBAL_H
#define SGXGLYPHGUI_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef WIN32
#ifdef SGXGlyphGUI_EXPORTS
# define SGXGLYPHGUI_EXPORT Q_DECL_EXPORT
#else
# define SGXGLYPHGUI_EXPORT Q_DECL_IMPORT
#endif

#else
# define SGXGLYPHGUI_EXPORT
#endif

#endif // SGXGLYPHGUI_GLOBAL_H
