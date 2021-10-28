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

// 次のトークンが `return` の場合はトークンを1つ読み進めて真を返す。それ以外の場合は偽を返す。
bool consume_return() {
  if (token->kind == TK_RETURN) {
    token = token->next;

    return true;
  }

  return false;
}

// 次のトークンが `if` の場合はトークンを1つ読み進めて真を返す。それ以外の場合は偽を返す。
bool consume_if() {
  if (token->kind == TK_IF) {
    token = token->next;

    return true;
  }

  return false;
}

// 次のトークンが `else` の場合はトークンを1つ読み進めて真を返す。それ以外の場合は偽を返す。
bool consume_else() {
  if (token->kind == TK_ELSE) {
    token = token->next;

    return true;
  }

  return false;
}

// 次のトークンが `while` の場合はトークンを1つ読み進めて真を返す。それ以外の場合は偽を返す。
bool consume_while() {
  if (token->kind == TK_WHILE) {
    token = token->next;

    return true;
  }

  return false;
}

// 次のトークンが `for` の場合はトークンを1つ読み進めて真を返す。それ以外の場合は偽を返す。
bool consume_for() {
  if (token->kind == TK_FOR) {
    token = token->next;

    return true;
  }

  return false;
}

// 次のトークンが識別子の場合はトークンを1つ読み進めてそのトークンを返す。
Token *consume_ident() {
  if (token->kind == TK_IDENT) {
    Token *result = token;

    token = token->next;

    return result;
  } else {
    return NULL;
  }
}

// 次のトークンが期待している記号の場合はトークンを1つ読み進める。それ以外の場合はエラーを報告する。
void expect(char *op) {
  if (
    token->kind != TK_RESERVED ||
    strlen(op) != token->len ||
    memcmp(token->str, op, token->len)
  ) {
    error_at(token->str, "\"%s\" ではありません", op);
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

int is_alnum(char c) {
  return ('0' <= c && c <= '9') ||
         ('A' <= c && c <= 'Z') ||
         ('a' <= c && c <= 'z') ||
         (c == '_');
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
      *p == ';' ||
      *p == '{' ||
      *p == '}' ||
      *p == '=' ||
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

LVar *locals;

LVar *find_lvar(Token *tok) {
  for (LVar *var = locals; var; var = var->next) {
    if (var->len == tok->len && !memcmp(tok->str, var->name, var->len)) {
      return var;
    }

  }

  return NULL;
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

Node *new_node_ident(int offset) {
  Node *node = calloc(1, sizeof(Node));

  node->kind = ND_LVAR;
  node->offset = offset;

  return node;
}

Node *code[100];

// program = stmt*
Node *program() {
  int i = 0;

  while (!at_eof()) {
    code[i++] = stmt();
  }

  code[i] = NULL; // terminal?
}

// stmt = expr ";"
//      | "{" stmt* "}"
//      | "if" "(" expr ")" stmt ("else" stmt)?
//      | "while" "(" expr ")" stmt
//      | "for" "(" expr? ";" expr? ";" expr? ")" stmt
//      | "return" expr ";"
Node *stmt() {
  Node *node;

  if (consume_if()) {
    node = new_node(ND_IF, NULL, NULL);

    expect("(");
    node->cond = expr();
    expect(")");
    node->then = stmt();

    if (consume_else()) {
      node->otherwise = stmt();
    }
  } else if (consume_while()) {
    node = new_node(ND_WHILE, NULL, NULL);

    expect("(");
    node->cond = expr();
    expect(")");
    node->body = stmt();
  } else if (consume_for()) {
    node = new_node(ND_FOR, NULL, NULL);

    expect("(");
    node->init = expr();
    expect(";");
    node->cond = expr();
    expect(";");
    node->iter = expr();
    expect(")");
    node->body = stmt();
  } else if (consume_return()) {
    // node = calloc(1, sizeof(Node));
    // node->kind = ND_RETURN;
    // node->lhs = expr();
    node = new_node(ND_RETURN, expr(), NULL);
    expect(";");
  } else if (consume("{")) {
    int i = 0;

    node = calloc(1, sizeof(Node));
    node->kind = ND_BLOCK;

    while (!consume("}")) {
      node->stmts[i++] = stmt();
    }
  } else {
    node = expr();
    expect(";");
  }

  return node;
}

// expr = assign
Node *expr() {
  Node *node = assign();

  return node;
}

// assign = equality ("=" assign)?
Node *assign() {
  Node *node = equality();

  if (consume("=")) {
    node = new_node(ND_ASSIGN, node, assign());
  }

  return node;
}

// relational ("==" relational | "!=" relational)*
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

// add ("<" add | "<=" add | ">" add | ">=" add)*
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

// mul ("+" mul | "-" mul)*
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

// mul = unary ("*" unary | "/" unary)*
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

// unary = ("+" | "-")? primary
Node *unary() {
  if (consume("+")) {
    return primary();
  }

  if (consume("-")) {
    return new_node(ND_SUB, new_node_num(0), primary());
  }

  return primary();
}

// primary = "(" expr ")" | ident | num
Node *primary() {
  // 次のトークンが "(" なら "(" expr ")"
  if (consume("(")) {
    Node *node = expr();

    expect(")");

    return node;
  }

  // 可能なら識別子を消費
  Token *tok = consume_ident();

  if (tok) {
    LVar *lvar = find_lvar(tok);

    if (!lvar) {
      lvar = calloc(1, sizeof(LVar));
      lvar->next = locals;
      lvar->name = tok->str;
      lvar->len = tok->len;

      if (locals) {
        lvar->offset = locals->offset + 8;
      } else {
        lvar->offset = 0; // 8 かも
      }

      locals = lvar;
    }

    return new_node_ident(lvar->offset);
  }

  // そうでなければ数値
  return new_node_num(expect_number());
}
