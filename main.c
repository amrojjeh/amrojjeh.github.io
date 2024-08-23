#include "platform/platform.h"

int main() {
  P_Mem_Init(MB(2));

  p_string *three = P_Markdown_Parse(P_String_Create("articles/test/en.md"));
  P_IO_PrintString(*three);
  return 0;
}
