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

#define PANIC(x) panic(x, __FILE__, __LINE__);

static p_alloc allocator;
static const char* NOT_INITIALIZED = "ERROR: memory was not initialized\n";

size_t strlen(const char* c_str) {
  size_t len = 0;
  while (*c_str != 0) {
    c_str++;
    len++;
  }
  return len;
}

void panic(const char* error_msg, char *_filename, int _fileline) {
  printf("%s:%d: %s", _filename, _fileline, error_msg);
  exit(1);
}

// NOTE: First kb of memory will be allocated to the stack
void P_Mem_Init(size_t bytes) {
  if (bytes < KB(2)) {
    PANIC("ERROR: not enough space\n");
  }

  void *buffer = mmap(NULL, bytes, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (buffer == MAP_FAILED) {
    PANIC("ERROR: could not initialize memory\n");
  }

  allocator.buffer = buffer;
  allocator.capacity = bytes;
  P_Mem_Reset();
}

void P_Mem_Reset() {
  allocator.table_ptr = allocator.buffer;
  allocator.body_ptr = allocator.buffer + KB(1);
}

size_t P_Mem_Available() {
  return (void*)allocator.body_ptr - (void*)allocator.table_ptr;
}

int P_Mem_HasInit() {
  return allocator.buffer != NULL;
}

p_string *_P_Mem_PushString(p_string str, char* _filename, int _line) {
  if (P_Mem_Available() < str.length) {
    panic("ERROR: not enough memory to write string\n", _filename, _line);
  }

  // NOTE: does not write null terminator
  char *start = allocator.body_ptr;
  for (size_t x = 0; x < str.length; ++x) {
    *allocator.body_ptr = str.buffer[x];
    allocator.body_ptr++;
  }
  *allocator.table_ptr = P_String_CreateWithLength(start, allocator.body_ptr - start);
  allocator.table_ptr++;
  return allocator.table_ptr-1;
}

p_string *_P_Mem_ExpandString(p_string str, char *file_name, int file_line) {
  if (P_Mem_Available() < str.length) {
    panic("ERROR: not enough memory to write string", file_name, file_line);
  }

  for (size_t x = 0; x < str.length; ++x) {
    *allocator.body_ptr = str.buffer[x];
    allocator.body_ptr++;
  }

  (allocator.table_ptr-1)->length += str.length;
  return allocator.table_ptr-1;
}

p_string *P_Mem_ExpandStringC(char c) {
  if (P_Mem_Available() == 0) {
    PANIC("ERROR: not enough memory to write string");
  }

  *allocator.body_ptr = c;
  allocator.body_ptr++;

  (allocator.table_ptr-1)->length += 1;
  return allocator.table_ptr-1;
}

p_string *P_Mem_PopString() {
  allocator.table_ptr--;
  allocator.body_ptr = allocator.table_ptr->buffer;
  return NULL;
}

p_slice P_IO_ListDirectory(p_string directory) {
  if (!P_Mem_HasInit()) {
    PANIC(NOT_INITIALIZED);
  }
  DIR *dir = opendir(directory.buffer);
  if (dir == NULL) {
    perror(__FILE__);
    PANIC("ERROR: could not open directory\n");
  }

  errno = 0;
  struct dirent *entry;
  struct stat file_stat;

  char *start = allocator.body_ptr;
  size_t output_length = 0;
  p_slice slice = {NULL, 0};
  while ((entry = readdir(dir)) != NULL) {
    // NOTE(Amr Ojjeh): skip hidden files
    if (entry->d_name[0] == '.') {
      continue;
    }

    p_string *name = P_Mem_PushString(directory);
    P_Mem_ExpandString(P_String_Create("/"));
    P_Mem_ExpandString(P_String_Create(entry->d_name));
    P_Mem_ExpandStringC('\0');

    if (stat(name->buffer, &file_stat) == -1) {
      perror(__FILE__);
      printf("%s\n", entry->d_name);
      PANIC("ERROR: could not fetch stats of a file\n");
    };

    if ((file_stat.st_mode & S_IFMT) == S_IFDIR) {
      P_Mem_ExpandString(P_String_Create("/"));
    }

    if (slice.base == NULL) {
      slice.base = name;
    }

    slice.length++;
  }

  if (errno != 0) {
    PANIC("ERROR: failed to list directories\n");
  }

  if (closedir(dir) == -1) {
    PANIC("ERROR: could not close directory\n");
  };

  return slice;
}

p_string *P_IO_ReadFile(p_string file_name) {
  p_string ret;
  if (allocator.buffer == NULL) {
    PANIC(NOT_INITIALIZED);
  }

  int fd = open(file_name.buffer, O_RDONLY);
  ssize_t length = read(fd, allocator.body_ptr, P_Mem_Available());
  ssize_t totalRead = length;
  if (length == -1) {
    P_IO_PrintString(file_name);
    printf("\n");
    PANIC("ERROR: could not read file\n");
  }
  
  while (length != 0) {
    length = read(fd, allocator.body_ptr+totalRead, P_Mem_Available());
    totalRead += length;
  }

  if (length > SSIZE_MAX || length >= allocator.capacity) {
    PANIC("ERROR: the file was too big to read\n");
  }

  ret = P_String_CreateWithLength(allocator.body_ptr, totalRead);
  allocator.body_ptr += totalRead;
  *allocator.table_ptr = ret;
  allocator.table_ptr++;

  return allocator.table_ptr - 1;
}

void P_IO_WriteFile(p_string file_name, p_string content) {
  int fd = open(file_name.buffer, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
  ssize_t length = write(fd, content.buffer, content.length);
  if (length == -1) {
    P_IO_PrintString(file_name);
    printf("\n");
    PANIC("ERROR: could not write file\n");
  }
  close(fd);
}

void P_IO_PrintString(p_string str) {
  for (size_t x = 0; x < str.length; ++x) {
    putchar(str.buffer[x]);
  }
}

p_string P_String_Create(char* c_str) {
  p_string ret = {c_str, strlen(c_str)};
  return ret;
}

p_string P_String_CreateWithLength(char* c_str, size_t len) {
  p_string ret = {c_str, len};
  return ret;
}

#else
#error platform not supported
#endif // defined(__unix__)

