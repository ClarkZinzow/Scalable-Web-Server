# Scalable-Web-Server

A simple, multithreaded scalable web server written in C.

### Getting Started

1. Clone or download this repository.
2. Navigate to the repository directory.
```
$ cd /path/to/repository/Scalable-Web-Server
```
3. Clean the repository of temporary files, object files, and executables.
```
$ make clean
```
4. Compile Scalable-Web-Server.
```
$ make
```
5. Run the server.
```
$ ./server [port] [threads] [buffers]
```
Set port to 2000 or above to avoid conflicts, and set number of threads and buffers to whatever you
want.
