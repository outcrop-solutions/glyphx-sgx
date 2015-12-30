#ifdef DATAENGINE_EXPORTS
#define DATAENGINE __declspec(dllexport)
#else
#define DATAENGINE __declspec(dllimport)
#endif