#include <assert.h>
#include <dirent.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <sys/inotify.h>

#define GREEN(format, ...) \
  printf("\033[1;32m[%s %d]" format "\33[0m\n", __func__, __LINE__, ## __VA_ARGS__)
#define EVENT_SIZE (sizeof(struct inotify_event))
#define EVENT_BUF_LEN (1024 * (EVENT_SIZE + 16))
#define MAX_NAME 512

void inspect();
void add_watch(const char *dir_path);// 递归添加所有子目录监视
void remove_watch();// 结束监视
void end_watch();
