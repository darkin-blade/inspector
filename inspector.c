#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#define GREEN(format, ...) \
  printf("\033[1;32m" format "\33[0m\n", ## __VA_ARGS__)

int main()
{
  return 0;
}
