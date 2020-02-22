#ifndef TCUA
/* Tucian的User-agent */
#define TCUA "Tucian 0"
#endif

#ifndef FETREL
#define FETREL
#include <json-glib/json-glib.h>
#include <curl/curl.h>

JsonNode *today_img();

/* 传入PID,NATIVE_PATH,LOCAL_URL，返回一个指向最后缓存文件的路径的指针，若失败则返回NULL */
char *cache_image(const char *PID, const char *NATIVE_PATH, const char *LOCAL_URL);

#endif