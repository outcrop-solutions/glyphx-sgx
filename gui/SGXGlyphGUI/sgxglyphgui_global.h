#pragma once
#ifndef SGXGLYPHGUI_GLOBAL_H
#define SGXGLYPHGUI_GLOBAL_H

#if defined(_MSC_VER)
#pragma warning(disable: 4251)
#pragma warning(disable: 4996) 
#define _SCL_SECURE_NO_WARNINGS 
//4996 - "Function call with parameters that may be unsafe" used in Boost
//4251 - "class needs to have dll - interface.." we will need to deal with this eventually
#endif

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
