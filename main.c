#include "platform/platform.h"

int main() {
  Platform_Mem_Init(MB(2));

  string content = Platform_IO_ListDirectory(Platform_String_Create("."));
  Platform_IO_PrintString(content);

  return 0;
}
