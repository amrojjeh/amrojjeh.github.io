#include "platform/platform.h"

int main() {
  P_Mem_Init(MB(2));

  p_string *one = P_Mem_PushString(P_String_Create("this is a test\n"));
  p_string *two = P_Mem_PushString(P_String_Create("Wow this is a test\n"));
  P_IO_PrintString(*two);
  P_Mem_PopString();
  P_IO_PrintString(*one);
  two = P_Mem_PushString(P_String_Create("cool!\n"));
  P_IO_PrintString(*one);
  P_IO_PrintString(*two);
  return 0;
}
