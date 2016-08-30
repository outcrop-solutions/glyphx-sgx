
#ifdef WIN32
#ifdef SGXGraphics_EXPORTS
#define SGXGRAPHICS_API __declspec(dllexport)
#else
#define SGXGRAPHICS_API __declspec(dllimport)
#endif
#else
#define SGXGRAPHICS_API
#endif

