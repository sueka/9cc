#include "9cc.h"

// 現在着目しているトークン
Token *token;

// 次のトークンが期待しているトークンの場合はトークンを1つ読み進めて真を返す。それ以外の場合は偽を返す。
bool consume(char *op) {
  if (
    strlen(op) == token->len &&
    !memcmp(token->str, op, token->len)
  ) {
    token = token->next;

    return true;
  } else {
    return false;
  }
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

// 次のトークンが期待しているトークンの場合は真を、それ以外の場合は偽を返す。
bool next(char *op) {
  return (
    strlen(op) == token->len &&
    !memcmp(token->str, op, token->len)
  );
}

// 次のトークンが期待しているトークンの場合はトークンを1つ読み進める。それ以外の場合はエラーを報告する。
void expect(char *op) {
  if (
    strlen(op) != token->len ||
    memcmp(token->str, op, token->len)
  ) {
    error_at(token->str, "\"%s\" ではありません", op);
  }

  token = token->next;
}

// 次のトークンが識別子の場合はトークンを1つ読み進めてそのトークンを返す。それ以外の場合はエラーを報告する。
Token *expect_ident() {
  if (token->kind != TK_IDENT) {
    error_at(token->str, "識別子ではありません。");
  }

  Token *result = token;

  token = token->next;

  return result;
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

  switch (kind) {
    case ND_ASSIGN:
      node->ty = rhs->ty;
      break;
    case ND_EQ:
    case ND_NE:
    case ND_LT:
    case ND_LTE:
      node->ty = calloc(1, sizeof(Type));
      node->ty->ty = INT;
      break;
    case ND_ADD:
    case ND_SUB:
    case ND_MUL:
    case ND_DIV:
      node->ty = lhs->ty;
      break;
    case ND_DEREF:
      if (lhs->ty->ty == PTR) {
        node->ty = lhs->ty->ptr_to;
      } else {
        node->ty = calloc(1, sizeof(Type));
        node->ty->ty = INT;
      }
      break;
    case ND_ADDR:
      node->ty = calloc(1, sizeof(Type));
      node->ty->ty = PTR;
      node->ty->ptr_to = lhs->ty;
      break;
  }

  return node;
}

Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));

  node->kind = ND_NUM;
  node->val = val;
  node->ty = calloc(1, sizeof(Type));
  node->ty->ty = INT;

  return node;
}

Node *new_node_ident(int offset, Type *ty) {
  Node *node = calloc(1, sizeof(Node));

  node->kind = ND_LVAR;
  node->offset = offset;
  node->ty = ty;

  return node;
}

Node *code[100];

// program = funcdefn*
Node *program() {
  int i = 0;

  while (!at_eof()) {
    code[i++] = funcdefn();
  }

  code[i] = NULL; // terminal?
}

// funcdefn = "int" ident fparams block
// fparams  = "(" (ldef ("," ldef)*)? ")"
Node *funcdefn() {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_FDEFN;

  expect("int");

  Token *tok = consume_ident();
  node->name = tok->str;
  node->len = tok->len;

  // fparams
  consume("(");

  int i = 0;

  if (!consume(")")) {
    do {
      node->args[i++] = ldef();
    } while (consume(","));

    expect(")");
  }

  node->body = block();

  return node;
}

// stmt  = expr ";"
//       | block
//       | "if" "(" expr ")" stmt ("else" stmt)?
//       | "while" "(" expr ")" stmt
//       | "for" "(" expr? ";" expr? ";" expr? ")" stmt
//       | "return" expr ";"
//       | ldef ";"
Node *stmt() {
  Node *node;

  if (consume("if")) {
    node = new_node(ND_IF, NULL, NULL);

    expect("(");
    node->cond = expr();
    expect(")");
    node->then = stmt();

    if (consume("else")) {
      node->otherwise = stmt();
    }
  } else if (consume("while")) {
    node = new_node(ND_WHILE, NULL, NULL);

    expect("(");
    node->cond = expr();
    expect(")");
    node->body = stmt();
  } else if (consume("for")) {
    node = new_node(ND_FOR, NULL, NULL);

    expect("(");
    node->init = expr();
    expect(";");
    node->cond = expr();
    expect(";");
    node->iter = expr();
    expect(")");
    node->body = stmt();
  } else if (consume("return")) {
    // node = calloc(1, sizeof(Node));
    // node->kind = ND_RETURN;
    // node->lhs = expr();
    node = new_node(ND_RETURN, expr(), NULL);
    expect(";");
  } else if (next("int")) {
    node = ldef();
    expect(";");
  } else if (next("{")) {
    node = block();
  } else {
    node = expr();
    expect(";");
  }

  return node;
}

// block = "{" stmt* "}"
Node *block() {
  expect("{");

  int i = 0;

  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_BLOCK;

  while (!consume("}")) {
    node->stmts[i++] = stmt();
  }

  // `stmts` は固定長 (`*Node[100]`) なので、ブロックが丁度100個の文からなる場合、 `gen()` が `stmts` の範囲を超えてしまう。終端に `NULL をセットすることでこれを明示的に回避できる。
  node->stmts[i] = NULL;

  return node;
}

// expr = assign
Node *expr() {
  Node *node = assign();

  return node;
}

// ldef = typename ident
Node *ldef() {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_DEF;

  Type *ty = typename();
  node->ty = ty;

  Token *tok = expect_ident();
  LVar *lvar = find_lvar(tok);

  if (lvar) {
    error_at(tok->str, "変数はすでに定義されています。");
  }

  lvar = calloc(1, sizeof(LVar));
  lvar->next = locals;
  lvar->name = tok->str;
  lvar->len = tok->len;
  lvar->ty = ty;

  if (locals) {
    lvar->offset = locals->offset + 8;
  } else {
    lvar->offset = 8;
  }

  locals = lvar;
  node->offset = lvar->offset;

  return node;
}

// typename = "int" "*"*
Type *typename() {
  Type *basety = calloc(1, sizeof(Type));
  basety->ty = INT;

  expect("int");

  Type *ty = basety;

  while (consume("*")) {
    Type *ptr = calloc(1, sizeof(Type));
    ptr->ty = PTR;
    ptr->ptr_to = ty;
    ty = ptr;
  }

  return ty;
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

// unary = "sizeof" unary
//       | ("+" | "-")? primary
//       | ("*" | "&") unary
Node *unary() {
  if (consume("sizeof")) {
    Node *node = unary();

    return new_node_num(_sizeof(node->ty));
  }

  if (consume("+")) {
    return primary();
  }

  if (consume("-")) {
    return new_node(ND_SUB, new_node_num(0), primary());
  }

  if (consume("*")) {
    return new_node(ND_DEREF, unary(), NULL);
  }

  if (consume("&")) {
    return new_node(ND_ADDR, unary(), NULL);
  }

  return primary();
}

// primary = "(" expr ")"
//         | ident fargs?
//         | num
// fargs   = "(" (expr ("," expr)*)? ")"
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

    if (consume("(")) {
      Node *node = calloc(1, sizeof(Node));

      node->kind = ND_FCALL;
      node->name = tok->str;
      node->len = tok->len;

      // NOTE: 現時点では関数の戻り値の型は捨てているので、とりあえず INT にしておく。
      node->ty = calloc(1, sizeof(Type));
      node->ty->ty = INT;

      int i = 0;

      if (!consume(")")) {
        node->args[i++] = expr();

        while (consume(",")) {
          node->args[i++] = expr();
        }

        expect(")");
      }

      return node;
    } else if (!lvar) {
      error_at(tok->str, "変数が定義されていません。");
    } else {
      return new_node_ident(lvar->offset, lvar->ty);
    }
  }

  // そうでなければ数値
  return new_node_num(expect_number());
}
