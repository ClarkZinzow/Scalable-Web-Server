#ifndef __REQUEST_H__

typedef struct {
  int id, count, statics, dynamics;
} thread;

void requestHandle(int fd, thread *worker);
long requestFileSize(int fd);

#endif
