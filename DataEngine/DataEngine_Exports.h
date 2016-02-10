
#ifdef WIN32
#ifdef DataEngine_EXPORTS
#define DATAENGINE __declspec(dllexport)
#else
#define DATAENGINE __declspec(dllimport)
#endif

#else
#define DATAENGINE 
#endif