#include "platform.h"

p_string *P_Markdown_Parse(p_string file_name) {
  p_string *content = P_IO_ReadFile(file_name);
  p_string *html = P_Mem_PushString(P_String_Create("<!doctype><html><body>"));
  html = P_Mem_ExpandString(P_String_Create("</body></html>"));
  return html;
}
