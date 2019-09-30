#include <assert.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/inotify.h>

#define GREEN(format, ...) \
  printf("\033[1;32m" format "\33[0m\n", ## __VA_ARGS__)
#define EVENT_SIZE (sizeof(struct inotify_event))
#define EVENT_BUF_LEN (1024 * (EVENT_SIZE + 16))

char root_path[128];// 监视的文件根目录

void test();

int main(int argc, char *argv[])
{
  if (argc <= 1) {// 没有参数
    getcwd(root_path, sizeof(root_path));
  } else {
    sprintf(root_path, "%s", argv[1]);
  }
  GREEN("%s", root_path);

  test();

  return 0;
}

void test()
{
  int fd = inotify_init();// 创建inotify实例
  assert(fd >= 0);

  char buf[EVENT_BUF_LEN];
  int watch_id = inotify_add_watch(fd, root_path, IN_CREATE | IN_DELETE);// TODO

  while (1) {
    int length = read(fd, buf, EVENT_BUF_LEN);
    GREEN("length: %d", length);

    for (int i = 0; i < length;) {
      struct inotify_event *event = (struct inotify_event *)&buf[i];
      if (event->len) {
        if (event->mask & IN_CREATE) {
          GREEN("create");
        } else if (event->mask & IN_DELETE) {
          GREEN("delete");
        } else if (event->mask & IN_MODIFY) {
          GREEN("modity");
        }
      }
      i += EVENT_SIZE + event->len;
    }
  }

  inotify_rm_watch(fd, watch_id);
  close(fd);
}
