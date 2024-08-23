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
      p_string read_str = {slice.buffer, read};
      P_Mem_ExpandString(read_str);
      slice.buffer += read;
      slice.length -= read;
      slice = P_Markdown_Italics(slice, PARSE_NONE);
      read = 0;
    } else {
      read++;
    }
  }

  read++;

  p_string read_str = {slice.buffer, read-1};
  P_Mem_ExpandString(read_str);
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
      p_string read_str = {slice.buffer, read};
      P_Mem_ExpandString(read_str);
      slice.buffer += read;
      slice.length -= read;
      slice = P_Markdown_Bold(slice, PARSE_NONE);
      read = 0;
    } else {
      read++;
    }
  }

  read++;

  p_string read_str = {slice.buffer, read-1};
  P_Mem_ExpandString(read_str);
  P_Mem_ExpandString(P_String_Create("</em>"));

  p_string rem = {slice.buffer + read, slice.length - read};
  return rem;
}

p_string P_Markdown_Paragraph(p_string slice) {
  P_Mem_ExpandString(P_String_Create("<p>"));

  size_t read = 0;
  while (slice.buffer[read] != '\n') {
    if (slice.buffer[read] == '*') {
      p_string read_str = {slice.buffer, read};
      P_Mem_ExpandString(read_str);
      slice.buffer += read;
      slice.length -= read;
      slice = P_Markdown_Bold(slice, PARSE_ITALICS);
      read = 0;
    } else if (slice.buffer[read] == '_') {
      p_string read_str = {slice.buffer, read};
      P_Mem_ExpandString(read_str);
      slice.buffer += read;
      slice.length -= read;
      slice = P_Markdown_Italics(slice, PARSE_BOLD);
      read = 0;
    } else {
      read++;
    }
  }

  read++;

  p_string read_str = {slice.buffer, read-1};
  P_Mem_ExpandString(read_str);

  P_Mem_ExpandString(P_String_Create("</p>"));
  p_string rem = {slice.buffer + read, slice.length - read};
  return rem;
}

p_string P_Markdown_Header(p_string slice) {
  char level = 0;

  while (slice.buffer[0] == '#') {
    level++;
    slice.buffer++;
    slice.length--;
  }

  P_Mem_ExpandString(P_String_Create("<h"));
  P_Mem_ExpandStringC(level + '0');
  P_Mem_ExpandStringC('>');

  while (slice.buffer[0] == ' ') {
    slice.buffer++;
    slice.length--;
  }

  p_string header_content = {slice.buffer, 0};
  while (slice.buffer[0] != '\n') {
    slice.buffer++;
    slice.length--;
    header_content.length++;
  }

  P_Mem_ExpandString(header_content);
  
  P_Mem_ExpandString(P_String_Create("</h"));
  P_Mem_ExpandStringC(level + '0');
  P_Mem_ExpandStringC('>');

  slice.buffer++;
  slice.length--;
  return slice;
}

p_string P_Markdown_Title(p_string slice) {
  p_string title_content = {slice.buffer, 0};
  while (*slice.buffer != '\n') {
    slice.buffer++;
    slice.length--;
    title_content.length++;
  }

  slice.buffer++;
  slice.length--;
  P_Mem_ExpandString(P_String_Create("<h1 class=\"title\">"));
  P_Mem_ExpandString(title_content);
  P_Mem_ExpandString(P_String_Create("</h1>"));

  return slice;
}

p_string P_Markdown_Date(p_string slice) {
  p_string date_content = {slice.buffer, 0};
  while (*slice.buffer != '\n') {
    slice.buffer++;
    slice.length--;
    date_content.length++;
  }

  slice.buffer++;
  slice.length--;
  P_Mem_ExpandString(P_String_Create("<h2 class=\"date\">"));
  P_Mem_ExpandString(date_content);
  P_Mem_ExpandString(P_String_Create("</h2>"));

  return slice;
}

p_string *P_Markdown_Parse(p_string file_name) {
  p_string content = *P_IO_ReadFile(file_name);
  p_string *html = P_Mem_PushString(P_String_Create("<!doctype><html><body>"));

  // first two lines have to be title and date
  content = P_Markdown_Title(content);
  content = P_Markdown_Date(content);

  while (content.length > 1) {
    if (*content.buffer == '\n') {
      content.buffer++;
      content.length--;
      continue;
    } else if (*content.buffer == '#') {
      content = P_Markdown_Header(content);
    } else {
      content = P_Markdown_Paragraph(content);
    }
  }

  html = P_Mem_ExpandString(P_String_Create("</body></html>"));
  return html;
}
