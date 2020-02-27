#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "../include/fetchrm.h"

/* 与strcat类似，但不改变传入值，连接两个字符串，参数l,r分别为连接后在左右两侧的字符串，返回一个指向连接后字符数组的指针 */
static char *cache_pathcat(const char *l, const char *r)
{
    char *d = malloc(strlen(l) + strlen(r) + 1);
    sprintf(d, "%s%s", l, r);
    return d;
}

static CURLcode fetch_image_to_file(const char *URL, const char *DEST_FILE)
{
    CURL *handle = curl_easy_init();
    curl_easy_setopt(handle, CURLOPT_URL, URL);
    curl_easy_setopt(handle, CURLOPT_USERAGENT, TCUA);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, fwrite);
    FILE *dest_pointer = fopen(DEST_FILE, "w+");
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, dest_pointer); 
    CURLcode status = curl_easy_perform(handle);
    curl_easy_cleanup(handle);
    fclose(dest_pointer);

    return status;
}

/* 检查dest是否存在，目录返回2,文件返回1,（可能）不存在返回0 */
static int check_dest(const char *dest)
{
    struct stat st_temp = { '0' };
    stat(dest, &st_temp);
    if (S_ISDIR(st_temp.st_mode))
    {
        return 2;
    } else if (S_ISREG(st_temp.st_mode))
    {
        return 1;
    } else
    {
        return 0;
    }    
}

/* 利用check_dest检查目标文件夹是否存在，如不存在则创建 */
static int check_and_create_dir(const char *dir)
{
    int test_var = check_dest(dir);
    if (test_var == 0)
    {
        if (mkdir(dir, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) != 0)
        {
            fprintf(stderr, "cache_image: mkdir() ERROR on %s", dir);
            return 1;
        }
    }
    return 0;
}

char *cache_image(const char *PID, const char *NATIVE_PATH, const char *LOCAL_URL)
{
    const char S1_SERVER[31] = "https://s1.images.dailypics.cn";
    const char S2_SERVER[31] = "https://s2.images.dailypics.cn";

    #if defined __MINGW32__
    const char DEST_DIR_R[9] = "\\tucian";
    char *dest_dir_l = getenv("TEMP");
    #else
    char *dest_dir_l = cache_pathcat(getenv("HOME"), "/.cache");
    const char DEST_DIR_R[9] = "/tucian";
    #endif
    char *dest_dir = cache_pathcat(dest_dir_l, DEST_DIR_R);
    if (check_and_create_dir(dest_dir_l) == 0)
    {
        if (check_and_create_dir(dest_dir) != 0)
        {
            fprintf(stderr, "Can not create cache dir\n");
            return NULL;
        }
    } else
    {
        fprintf(stderr, "Can not create cache dir\n");
        return NULL;
    }

    #if defined __MINGW32__
    char *dest_file = cache_pathcat(cache_pathcat(dest_dir, "\\"), PID);
    #else
    char *dest_file = cache_pathcat(cache_pathcat(dest_dir, "/"), PID);
    #endif

    switch (check_dest(dest_file))
    {
    case 1:
        return dest_file;
        break;
    
    case 0:
        break;

    default:
        fprintf(stderr, "Unknow Dest");
        return NULL;
        break;
    }

    uint try_times = 0;
    char *url = cache_pathcat(cache_pathcat(S1_SERVER, NATIVE_PATH), "!w360_jpg");
    char *localurlc = malloc(strlen(LOCAL_URL));
    strcpy(localurlc, LOCAL_URL);
    while (fetch_image_to_file(url, dest_file))
    {
        try_times += 1;
        switch (try_times)
        {
        case 1:
            url = cache_pathcat(S2_SERVER, NATIVE_PATH);
            break;

        case 2:
            url = localurlc;
            break;
        
        default:
            fprintf(stderr, "Can not cache image\n");
            return NULL;
            break;
        }
    }

    return dest_file;
}