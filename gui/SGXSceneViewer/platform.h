
#pragma once

#ifdef WIN32
#include <QtCore/qglobal.h>
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#ifdef SGXSceneViewer_EXPORTS
# define SGXSCENEVIEWER_API Q_DECL_EXPORT
#else
# define SGXSCENEVIEWER_API Q_DECL_IMPORT
#endif
#else
#define SGXSCENEVIEWER_API
#endif


