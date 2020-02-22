#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/fetchrm.h"

struct fetch_temp
{
    char *temp;
    size_t size;
};

static size_t result_to_string(void *buffer, size_t size, size_t nmemb, void *userp)
{
    size_t rsize = size * nmemb;
    struct fetch_temp *tmem = (struct fetch_temp *)userp;
    char *ptemp = realloc(tmem->temp, tmem->size + rsize + 1);
    if (!ptemp)
    {
        return 0;
    }
    tmem->temp = ptemp;
    memcpy(&(tmem->temp[tmem->size]), buffer, size * nmemb);
    tmem->size += rsize;
    tmem->temp[tmem->size] = '\0';
    return size * nmemb;
}

JsonNode *today_img()
{
    CURL *handle = curl_easy_init();
    struct fetch_temp t;
    t.temp = malloc(1);
    if (!t.temp)
    {
        return NULL;
    }
    t.size = 0;
    curl_easy_setopt(handle, CURLOPT_URL, "https://v2.api.dailypics.cn/today");
    curl_easy_setopt(handle, CURLOPT_USERAGENT, TCUA);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, result_to_string);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&t);
    CURLcode code = curl_easy_perform(handle);
    curl_easy_cleanup(handle);
    if (code)
    {
        fprintf(stderr, "CURL ERROR CODE: %d\n", code);
        return NULL;
    } else
    {
        JsonParser *parsed_json = json_parser_new();
        if (!json_parser_load_from_data(parsed_json, t.temp, t.size, NULL))
        {
            return NULL;
        }
        return json_parser_get_root(parsed_json);
    }
}