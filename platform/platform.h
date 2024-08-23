#include <stdint.h>
#include <stddef.h>

#define KB(x) ((size_t)(x << 10))
#define MB(x) ((size_t)(x << 20))

// TYPES

typedef struct {
  char* buffer;
  size_t length;
} p_string;

typedef struct {
  void *buffer;
  p_string *table_ptr;
  char *body_ptr;
  size_t capacity;
} p_alloc;

typedef struct {
  p_string *base;
  size_t length;
} p_slice;

// === FILE IO ===
extern p_slice P_IO_ListDirectory(p_string directory);
extern p_string *P_IO_ReadFile(p_string file_name);
extern void P_IO_PrintString(p_string str);

// === STRING ===
extern p_string P_String_Create(char* c_str);
extern p_string P_String_CreateWithLength(char* c_str, size_t len);

// === MEMORY ===
extern void P_Mem_Init(size_t bytes);
extern int P_Mem_HasInit();
extern void P_Mem_Reset();
extern size_t P_Mem_Available();
extern p_string *P_Mem_PushString(p_string str);
extern p_string *P_Mem_ExpandString(p_string str);
extern p_string *P_Mem_ExpandStringC(char c);
extern p_string *P_Mem_PopString();

// === MARKDOWN ===
extern p_string *P_Markdown_Parse(p_string file_name);
