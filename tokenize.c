#include "9cc.h"

// 入力文字列 p をトークナイズしてそれを返す。
Token *tokenize(char *p) {
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p) {
    // 空白文字をスキップ
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (
      *p == '=' && *(p + 1) == '=' ||
      *p == '!' && *(p + 1) == '=' ||
      *p == '<' && *(p + 1) == '=' ||
      *p == '>' && *(p + 1) == '='
    ) {
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    } else if (
      *p == ',' ||
      *p == ';' ||
      *p == '{' ||
      *p == '}' ||
      *p == '=' ||
      *p == '<' ||
      *p == '>' ||
      *p == '+' ||
      *p == '-' ||
      *p == '*' ||
      *p == '&' ||
      *p == '/' ||
      *p == '(' ||
      *p == ')'
    ) {
      cur = new_token(TK_RESERVED, cur, p, 1);
      ++p;
      continue;
    } else if (strncmp(p, "return", 6) == 0 && !is_alnum(p[6])) {
      cur = new_token(TK_RETURN, cur, p, 6);
      p += 6;
      continue;
    } else if (strncmp(p, "if", 2) == 0 && !is_alnum(p[2])) {
      cur = new_token(TK_IF, cur, p, 2);
      p += 2;
      continue;
    } else if (strncmp(p, "else", 4) == 0 && !is_alnum(p[4])) {
      cur = new_token(TK_ELSE, cur, p, 4);
      p += 4;
      continue;
    } else if (strncmp(p, "while", 5) == 0 && !is_alnum(p[5])) {
      cur = new_token(TK_WHILE, cur, p, 5);
      p += 5;
      continue;
    } else if (strncmp(p, "for", 3) == 0 && !is_alnum(p[3])) {
      cur = new_token(TK_FOR, cur, p, 3);
      p += 3;
      continue;
    } else if (strncmp(p, "int", 3) == 0 && !is_alnum(p[3])) {
      cur = new_token(TK_INT, cur, p, 3);
      p += 3;
      continue;
    } else if ('a' <= *p && *p <= 'z') {
      // `if ('a' <= *p && *p <= 'z')` なので len >= 1 ではある
      cur = new_token(TK_IDENT, cur, p, 0);
      char *q = p;

      while ('a' <= *p && *p <= 'z') {
        ++p;
      }

      cur->len = p - q;
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p, 0);
      char *q = p;
      cur->val = strtol(p, &p, 10);
      cur->len = p - q;
      continue;
    }

    error_at(token->str, "トークナイズできません。");
  }

  new_token(TK_EOF, cur, p, 0);

  return head.next;
}

// 新しいトークンを作成して cur に繋げる。
Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
  Token *tok = calloc(1, sizeof(Token));

  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  cur->next = tok;

  return tok;
}

int is_alnum(char c) {
  return ('0' <= c && c <= '9') ||
         ('A' <= c && c <= 'Z') ||
         ('a' <= c && c <= 'z') ||
         (c == '_');
}
