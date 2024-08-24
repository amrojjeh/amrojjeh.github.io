#include "platform/platform.h"

p_string *htmlFileName(p_string file_path) {
  for (int x = 0; x < file_path.length; ++x) {
    if (file_path.buffer[x] == '/') {
      file_path.buffer += x + 1;
      file_path.length -= x + 1;
      x = 1;
    }
  }
  p_string name = {file_path.buffer, file_path.length-3};
  p_string *ret = P_Mem_PushString(name);
  P_Mem_ExpandString(P_String_Create("html\0"));
  return ret;
}

int main() {
  P_Mem_Init(MB(2));

  p_slice entries = P_IO_ListDirectory(P_String_Create("articles"));
  for (int x = 0; x < entries.length; ++x) {
    p_string *file = P_IO_ReadFile(entries.base[x]);
    p_string *markdown = P_Markdown_Parse(*file);
    P_IO_WriteFile(*htmlFileName(entries.base[x]), *markdown);
    P_Mem_PopString(); // Unread htmlFileName
    P_Mem_PopString(); // Unread markdown
    P_Mem_PopString(); // Unread file
  }

  return 0;
}
