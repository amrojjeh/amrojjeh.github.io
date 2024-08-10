#include <stdint.h>
#include <stddef.h>

#define KB(x) ((size_t)(x << 10))
#define MB(x) ((size_t)(x << 20))

// TYPES

typedef struct {
  const char* buffer;
  size_t length;
} string;

typedef struct {
  void *buffer;
  size_t length;
  size_t capacity;
} alloc;

// === FILE IO ===
extern string Platform_IO_ListDirectory(string directory);
extern string Platform_IO_ReadFile(string file_name);
extern void Platform_IO_PrintString(string str);

// === STRING ===
extern string Platform_String_Create(const char* c_str);
extern string Platform_String_CreateWithLength(char* c_str, size_t len);

// === MEMORY ===
extern void Platform_Mem_Init(size_t bytes);
extern int Platform_Mem_HasInit();
extern void Platform_Mem_Reset();
extern void* Platform_Mem_Seek();
extern size_t Platform_Mem_Available();
extern size_t Platform_Mem_Write(string str);

// IDEA: Use a stack allocation so I can allocate a bunch of things and free the last chunk all at once
// NOTE: Not all these functions need to be exposted. I should make a p_internal.h
