#include "networks.h"
#include "request.h"

/*
 * server.c: A very, very simple web server
 *
 * To run:
 *   server <portnum (above 2000)> <threads> <buffers>
 *
 * Repeatedly handles HTTP requests sent to this port number.
 * Most of the work is done within routines written in request.c
 */

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t fill = PTHREAD_COND_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;

pthread_t **cid;

// Request struct.
typedef struct {
    int fd;
    long size;
} request;

request **buffer;
int fillptr, useptr, max, numfull, threadid;

/*
 * Get port, number of threads, and number of buffers arguments.
 */
void getargs(int *port, int *threads, int *buffers, int argc, char *argv[]) {
    if (argc != 4) {
      fprintf(stderr, "Usage: %s <port> <threads> <buffers>\n", argv[0]);
      exit(1);
    }
    *port = atoi(argv[1]);
    *threads = atoi(argv[2]);
    *buffers = atoi(argv[3]);
}

/*
 * Compare request sizes.
 *
 * Returns 0 if equal, greater than 0 if first request is larger, and less than 0 if first
 * request is smaller.
 */
int requestcmp(const void *first, const void *second) {
  return ((*(request **)first)->size - (*(request **)second)->size);
}

/*
 * Consumer thread function.
 */
void *consumer(void *arg) {
  thread worker;
  worker.id = -1;
  worker.count = 0;
  worker.statics = 0;
  worker.dynamics = 0;
  while(1) {
    pthread_mutex_lock(&lock);
    // Wait for buffer to be ready for consumption.
    while(numfull == 0) {
      pthread_cond_wait(&fill, &lock);
    }
    
    if(worker.id < 0) {
      worker.id = threadid;
      threadid++;
    }

    worker.count++;

    request *req;

    req = (request *) buffer[useptr];
    buffer[0] = buffer[fillptr - 1];
    // If more than one request in buffer, sort buffer by request size.
    if(fillptr > 1) {
      qsort(buffer, fillptr, sizeof(*buffer), requestcmp);
    }
    fillptr--;
    numfull--;

    pthread_cond_signal(&empty);
    pthread_mutex_unlock(&lock);

    // Handle request.
    requestHandle(req->fd, &worker);
    Close(req->fd);
  }
}

/*
 * Main function.
 */
int main(int argc, char *argv[]) {
  int listenfd, connfd, port, threads, buffers, clientlen;
  struct sockaddr_in clientaddr;

  // Get port, # of threards, and # of buffers.
  getargs(&port, &threads, &buffers, argc, argv);

  max = buffers;
  numfull = fillptr = useptr = 0;
  buffer = malloc(buffers * sizeof(*buffer));
  cid = malloc(threads * sizeof(*cid));

  // Create threads.
  int i;
  for(i = 0; i < threads; i++) {
    cid[i] = malloc(sizeof(pthread_t));
    pthread_create(cid[i], NULL, consumer, NULL);
  }

  // Open port for listening.
  listenfd = Open_listenfd(port);

  // Main loop.
  while (1) {
    clientlen = sizeof(clientaddr);
    // Accept connection on socket.
    connfd = Accept(listenfd, (SA *) &clientaddr, (socklen_t *) &clientlen);

    pthread_mutex_lock(&lock);
    // Wait for buffer to be ready for producing.
    while(numfull == max) {
      pthread_cond_wait(&empty, &lock);
    }

    request *req = malloc(sizeof(request));

    // Get file size.
    req->size = requestFileSize(connfd);
    buffer[fillptr] = req;
    fillptr++;

    // If more than one request in buffer, sort buffer by request size.
    if(fillptr > 1) {
      qsort(buffer, fillptr, sizeof(*buffer), requestcmp);
    }
    req->fd = connfd;
    numfull++;

    pthread_cond_signal(&fill);
    pthread_mutex_unlock(&lock);
  }

}


    


 
