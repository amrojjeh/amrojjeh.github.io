#include "platform.h"

#define PARSE_BOLD 1
#define PARSE_ITALICS 1
#define PARSE_NONE 0

p_string P_Markdown_Bold(p_string slice, int parseItalics);
p_string P_Markdown_Italics(p_string slice, int parseBold);

p_string P_Markdown_Bold(p_string slice, int parseItalics) {
  P_Mem_ExpandString(P_String_Create("<strong>"));

  // skip *
  slice.buffer += 1;
  slice.length -= 1;
  size_t read = 0;

  while (slice.buffer[read] != '*') {
    if (slice.buffer[read] == '_' && parseItalics) {
      p_string readStr = {slice.buffer, read};
      P_Mem_ExpandString(readStr);
      slice.buffer += read;
      slice.length -= read;
      slice = P_Markdown_Italics(slice, PARSE_NONE);
      read = 0;
    } else {
      read++;
    }
  }

  read++;

  p_string readStr = {slice.buffer, read-1};
  P_Mem_ExpandString(readStr);
  P_Mem_ExpandString(P_String_Create("</strong>"));

  p_string rem = {slice.buffer + read, slice.length - read};
  return rem;
}

p_string P_Markdown_Italics(p_string slice, int parseBold) {
  P_Mem_ExpandString(P_String_Create("<em>"));

  // skip _
  slice.buffer += 1;
  slice.length -= 1;
  size_t read = 0;

  while (slice.buffer[read] != '_') {
    if (slice.buffer[read] == '*' && parseBold) {
      p_string readStr = {slice.buffer, read};
      P_Mem_ExpandString(readStr);
      slice.buffer += read;
      slice.length -= read;
      slice = P_Markdown_Bold(slice, PARSE_NONE);
      read = 0;
    } else {
      read++;
    }
  }

  read++;

  p_string readStr = {slice.buffer, read-1};
  P_Mem_ExpandString(readStr);
  P_Mem_ExpandString(P_String_Create("</em>"));

  p_string rem = {slice.buffer + read, slice.length - read};
  return rem;
}

p_string P_Markdown_Paragraph(p_string slice) {
  P_Mem_ExpandString(P_String_Create("<p>"));

  size_t read = 0;
  while (slice.buffer[read] != '\n') {
    if (slice.buffer[read] == '*') {
      p_string readStr = {slice.buffer, read};
      P_Mem_ExpandString(readStr);
      slice.buffer += read;
      slice.length -= read;
      slice = P_Markdown_Bold(slice, PARSE_ITALICS);
      read = 0;
    } else if (slice.buffer[read] == '_') {
      p_string readStr = {slice.buffer, read};
      P_Mem_ExpandString(readStr);
      slice.buffer += read;
      slice.length -= read;
      slice = P_Markdown_Italics(slice, PARSE_BOLD);
      read = 0;
    } else {
      read++;
    }
  }

  read++;

  p_string readStr = {slice.buffer, read-1};
  P_Mem_ExpandString(readStr);

  P_Mem_ExpandString(P_String_Create("</p>"));
  p_string rem = {slice.buffer + read, slice.length - read};
  return rem;
}

// TODO(Amr Ojjeh): frontmatter, header

p_string *P_Markdown_Parse(p_string file_name) {
  p_string content = *P_IO_ReadFile(file_name);
  p_string *html = P_Mem_PushString(P_String_Create("<!doctype><html><body>"));

  while (content.length > 1) {
      content = P_Markdown_Paragraph(content);
  }

  html = P_Mem_ExpandString(P_String_Create("</body></html>"));
  return html;
}
