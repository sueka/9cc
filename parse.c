#include "9cc.h"

// 現在着目しているトークン
Token *token;

// 次のトークンが期待している記号の場合はトークンを1つ読み進めて真を返す。それ以外の場合は偽を返す。
bool consume(char *op) {
  if (
    token->kind == TK_RESERVED &&
    strlen(op) == token->len &&
    !memcmp(token->str, op, token->len)
  ) {
    token = token->next;

    return true;
  } else {
    return false;
  }
}

// 次のトークンが期待している記号の場合はトークンを1つ読み進める。それ以外の場合はエラーを報告する。
void expect(char *op) {
  if (
    token->kind != TK_RESERVED ||
    strlen(op) != token->len ||
    memcmp(token->str, op, token->len)
  ) {
    error_at(token->str, "'%c' ではありません", op);
  }

  token = token->next;
}

// 次のトークンが数値の場合はトークンを1つ読み進めてその数値を返す。それ以外の場合はエラーを報告する。
int expect_number() {
  if (token->kind != TK_NUM) {
    error_at(token->str, "数ではありません。");
  }

  int val = token->val;

  token = token->next;

  return val;
}

bool at_eof() {
  return token->kind == TK_EOF;
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
      *p == '<' ||
      *p == '>' ||
      *p == '+' ||
      *p == '-' ||
      *p == '*' ||
      *p == '/' ||
      *p == '(' ||
      *p == ')'
    ) {
      cur = new_token(TK_RESERVED, cur, p, 1);
      ++p;
      continue;
    } else if ('a' <= *p && *p <= 'z') {
      cur = new_token(TK_IDENT, cur, p, 1);
      ++p;
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

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));

  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;

  return node;
}

Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));

  node->kind = ND_NUM;
  node->val = val;

  return node;
}

Node *expr() {
  Node *node = equality();

  return node;
}

Node *equality() {
  Node *node = relational();

  for (;;) {
    if (consume("==")) {
      node = new_node(ND_EQ, node, relational());
    } else if (consume("!=")) {
      node = new_node(ND_NE, node, relational());
    } else {
      return node;
    }
  }
}

Node *relational() {
  Node *node = add();

  // > は < の両辺を入れ替えて実装する。ノードは木なので優先度のことは気にしなくてよい。
  for (;;) {
    if (consume("<")) {
      node = new_node(ND_LT, node, add());
    } else if (consume("<=")) {
      node = new_node(ND_LTE, node, add());
    } else if (consume(">")) {
      node = new_node(ND_LT, add(), node);
    } else if (consume(">=")) {
      node = new_node(ND_LTE, add(), node);
    } else {
      return node;
    }
  }
}

Node *add() {
  Node *node = mul();

  for (;;) {
    if (consume("+")) {
      node = new_node(ND_ADD, node, mul());
    } else if (consume("-")) {
      node = new_node(ND_SUB, node, mul());
    } else {
      return node;
    }
  }
}

Node *mul() {
  Node *node = unary();

  for (;;) {
    if (consume("*")) {
      node = new_node(ND_MUL, node, unary());
    } else if (consume("/")) {
      node = new_node(ND_DIV, node, unary());
    } else {
      return node;
    }
  }
}

Node *unary() {
  if (consume("+")) {
    return primary();
  }

  if (consume("-")) {
    return new_node(ND_SUB, new_node_num(0), primary());
  }

  return primary();
}

Node *primary() {
  // 次のトークンが "(" なら "(" expr ")"
  if (consume("(")) {
    Node *node = expr();

    expect(")");

    return node;
  }

  // そうでなければ数値
  return new_node_num(expect_number());
}
