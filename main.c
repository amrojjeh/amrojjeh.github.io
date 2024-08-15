#include "platform/platform.h"

int main() {
  P_Mem_Init(MB(2));

  p_string *one = P_Mem_PushString(P_String_Create("this is a test\n"));
  p_string *two = P_Mem_PushString(P_String_Create("Wow this is a test\n"));
  P_IO_PrintString(*one);
  P_IO_PrintString(*two);
  two = P_Mem_PopString();
  p_string *three = P_IO_ReadFile(P_String_Create("main.c"));
  three = P_Mem_PopString();
  P_IO_PrintString(*two);
  P_IO_PrintString(*three);
  return 0;
}
