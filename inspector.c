#include "inspector.h"

char root_path[128];// 监视的文件根目录

int fd = 0;// 监视的实例
int watch_id[128];// 最多128个子目录
int total_watch = 0;// 初始化为0个目录

int main(int argc, char *argv[])
{
  if (argc <= 1) {// 没有参数
    getcwd(root_path, sizeof(root_path));
  } else {
    sprintf(root_path, "%s", argv[1]);
  }
  GREEN("%s", root_path);

  // GREEN("access: %d", IN_ACCESS);
  // GREEN("attrib: %d", IN_ATTRIB);
  // GREEN("closew: %d", IN_CLOSE_WRITE);
  // GREEN("closen: %d", IN_CLOSE_NOWRITE);
  // GREEN("create: %d", IN_CREATE);
  // GREEN("delete: %d", IN_DELETE);
  // GREEN("delets: %d", IN_DELETE_SELF);
  // GREEN("modity: %d", IN_MODIFY);
  // GREEN("mvselt: %d", IN_MOVE_SELF);
  // GREEN("mvfrom: %d", IN_MOVED_FROM);
  // GREEN("moveto: %d", IN_MOVED_TO);

  fd = inotify_init();// 创建inotify实例
  assert(fd >= 0);
  add_watch(root_path);// 开始递归监视
  signal(SIGTERM, end_watch);// ctrl + c
  inspect();

  return 0;
}

void add_watch(const char *dir_path)
{
  assert(dir_path != NULL);
  if (total_watch >= 127) {// 到达上限
    GREEN("can't inspect");
    return;
  }
  GREEN("%d: %s", total_watch, dir_path);

  DIR *dir = opendir(dir_path);
  struct dirent *dir_item;
  watch_id[total_watch] = inotify_add_watch(fd, dir_path, IN_CREATE | IN_DELETE | IN_MODIFY);// 对指定路径进行监视
  total_watch ++;
  while (dir_item = readdir(dir)) {
    if (dir_item->d_name[0] == '.') {// 不监视隐藏文件
      continue;
    }
    // GREEN("%s %d", dir_item->d_name, dir_item->d_type);
    if (dir_item->d_type == 4) {// 是目录
      char absolute_path[MAX_NAME];
      sprintf(absolute_path, "%s/%s", dir_path, dir_item->d_name);
      add_watch(absolute_path);// 递归
    }
  }
}

void remove_watch()
{
  for (int i = 0; i < total_watch; i ++) {// 清除所有监视
    inotify_rm_watch(fd, watch_id[i]);
  }
  total_watch = 0;
}

void inspect()
{
  int last_time = time(NULL), cur_time = 0;
  char buf[EVENT_BUF_LEN];

  while (1) {
    int length = read(fd, buf, EVENT_BUF_LEN);
    GREEN("length: %d", length);

    cur_time = time(NULL);
    if (difftime(cur_time , last_time) < 2) {// 间隔过短
      last_time = cur_time;
      continue;
    } else {
      last_time = cur_time;
    }

    for (int i = 0; i < length;) {
      struct inotify_event *event = (struct inotify_event *)&buf[i];
      if (event->len) {
        if (event->mask & IN_ACCESS) {
          GREEN("access");
        } else if (event->mask & IN_ATTRIB) {
          GREEN("attrib");
        } else if (event->mask & IN_CLOSE_WRITE) {
          GREEN("close write");
        } else if (event->mask & IN_CLOSE_NOWRITE) {
          GREEN("close no write");
        } else if (event->mask & IN_CREATE) {
          GREEN("create %s", event->name);
          remove_watch();
          add_watch(root_path);
        } else if (event->mask & IN_DELETE) {
          GREEN("delete");
        } else if (event->mask & IN_DELETE_SELF) {
          GREEN("delete self");
        } else if (event->mask & IN_MODIFY) {
          GREEN("modity");
        } else if (event->mask & IN_MOVE_SELF) {
          GREEN("move self");
        } else if (event->mask & IN_MOVED_FROM) {
          GREEN("move from");
        } else if (event->mask & IN_MOVED_TO) {
          GREEN("move to");
        }
      }
      i += EVENT_SIZE + event->len;
    }
  }
}

void end_watch(int signum)
{
  // GREEN("end");
  remove_watch();
  close(fd);
  exit(signum);
}
