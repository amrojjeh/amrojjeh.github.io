#include "platform/platform.h"

int main() {
  P_Mem_Init(MB(2));

  p_slice entries = P_IO_ListDirectory(P_String_Create("."));
  for (size_t x = 0; x < entries.length; ++x) {
    P_IO_PrintString(entries.base[x]);
    P_IO_PrintString(P_String_Create("\n"));
  }
  return 0;
}
