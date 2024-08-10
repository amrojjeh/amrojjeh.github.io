#if defined(__unix__)

#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <errno.h>

#include "platform.h"

static alloc allocator;
static const char* NOT_INITIALIZED = "ERROR: memory was not initialized\n";

size_t strlen(const char* c_str) {
  size_t len = 0;
  while (*c_str != 0) {
    c_str++;
    len++;
  }
  return len;
}

void panic(const char* error_msg) {
  printf("%s", error_msg);
  exit(1);
}

void Platform_Mem_Init(size_t bytes) {
  void *buffer = mmap(NULL, bytes, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (buffer == MAP_FAILED) {
    panic("ERROR: could not initialize memory\n");
  }

  allocator.buffer = buffer;
  allocator.capacity = bytes;
  allocator.length = 0;
}

void Platform_Mem_Reset() {
  allocator.length = 0;
}

void* Platform_Mem_Seek() {
  return allocator.buffer + allocator.length;
}

size_t Platform_Mem_Available() {
  return allocator.capacity - allocator.length;
}

int Platform_Mem_HasInit() {
  return allocator.buffer != NULL;
}

size_t Platform_Mem_Write(string str) {
  if (Platform_Mem_Available() < str.length) {
    panic("ERROR: not enough memory to write string");
  }

  // NOTE: does not write null terminator
  for (size_t x = 0; x < str.length; ++x) {
    *((char*)(Platform_Mem_Seek())) = str.buffer[x];
    allocator.length++;
  }
  return str.length;
}

string Platform_IO_ListDirectory(string directory) {
  if (!Platform_Mem_HasInit()) {
    panic(NOT_INITIALIZED);
  }
  DIR *dir = opendir(directory.buffer);
  if (dir == NULL) {
    perror(__FILE__);
    panic("ERROR: could not open directory\n");
  }

  errno = 0;
  struct dirent *entry;
  struct stat file_stat;

  char *start = Platform_Mem_Seek();
  size_t output_length = 0;
  while ((entry = readdir(dir)) != NULL) {
    // NOTE: skip hidden files
    if (entry->d_name[0] == '.') {
      continue;
    }

    if (stat(entry->d_name, &file_stat) == -1) {
      perror(__FILE__);
      printf("%s\n", entry->d_name);
      panic("ERROR: could not fetch stats of a file\n");
    };

    string name = Platform_String_Create(entry->d_name);
    output_length += Platform_Mem_Write(name);
    if ((file_stat.st_mode & S_IFMT) == S_IFDIR) {
      output_length += Platform_Mem_Write(Platform_String_Create("/"));
    }
    output_length += Platform_Mem_Write(Platform_String_Create(";"));
  }

  if (errno != 0) {
    panic("ERROR: failed to list directories\n");
  }

  if (closedir(dir) == -1) {
    panic("ERROR: could not close directory\n");
  };

  return Platform_String_CreateWithLength(start, output_length);
}

string Platform_IO_ReadFile(string file_name) {
  string ret;
  if (allocator.buffer == NULL) {
    panic(NOT_INITIALIZED);
  }

  int fd = open(file_name.buffer, O_RDONLY);
  ssize_t length = read(fd, Platform_Mem_Seek(), Platform_Mem_Available());
  if (length == -1) {
    panic("ERROR: could not read file\n");
  }
  if (length > SSIZE_MAX || length >= allocator.capacity) {
    panic("ERROR: the file was too big to read\n");
  }

  allocator.length = length;

  ret.buffer = allocator.buffer;
  ret.length = allocator.length;
  return ret;
}

void Platform_IO_PrintString(string str) {
  for (size_t x = 0; x < str.length; ++x) {
    putchar(str.buffer[x]);
  }
}

string Platform_String_Create(const char* c_str) {
  string ret = {c_str, strlen(c_str)};
  return ret;
}

string Platform_String_CreateWithLength(char* c_str, size_t len) {
  string ret = {c_str, len};
  return ret;
}

#else
#error platform not supported
#endif // defined(__unix__)

