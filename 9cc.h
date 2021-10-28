#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// トークンの種類
typedef enum {
  TK_RESERVED, // 記号
  TK_IDENT,    // 識別子
  TK_NUM,      // 整数
  TK_EOF,      // 入力の終わり
} TokenKind;

typedef struct Token Token;

// トークン型
struct Token {
  TokenKind kind; // トークンの型
  Token *next;    // 次の入力トークン
  int val;        // kind が TK_NUM の場合、その数値
  char *str;      // トークン文字列
  int len;        // トークンの長さ
};

// 現在着目しているトークン
extern Token *token;

// エラーを報告するための関数
// printf と同じ引数を取る。
void error(char *fmt, ...);

// 入力プログラム
// extern char *user_input;

// エラー箇所を報告する。
void error_at(char *loc, char *fmt, ...);

// 次のトークンが期待している記号の場合はトークンを1つ読み進めて真を返す。それ以外の場合は偽を返す。
bool consume(char *op);

// 次のトークンが期待している記号の場合はトークンを1つ読み進める。それ以外の場合はエラーを報告する。
void expect(char *op);

// 次のトークンが数値の場合はトークンを1つ読み進めてその数値を返す。それ以外の場合はエラーを報告する。
int expect_number();

bool at_eof();

// 新しいトークンを作成して cur に繋げる。
Token *new_token(TokenKind kind, Token *cur, char *str, int len);

// 入力文字列 p をトークナイズしてそれを返す。
Token *tokenize(char *p);

// 抽象構文木のノードの種類
typedef enum {
  ND_ASSIGN, // =
  ND_EQ,     // ==
  ND_NE,     // !-
  ND_LT,     // <
  ND_LTE,    // <=
  ND_ADD,    // +
  ND_SUB,    // -
  ND_MUL,    // *
  ND_DIV,    // /
  ND_NUM,    // 整数
  ND_LVAR,   // ローカル変数
} NodeKind;

typedef struct Node Node;

// 抽象構文木のノードの型
struct Node {
  NodeKind kind; // ノードの型
  Node *lhs;     // 左辺
  Node *rhs;     // 右辺
  int val;       // kind が ND_NUM の場合のみ使う
  int offset;    // kind が ND_LVAR の場合のみ使う
};

Node *new_node(NodeKind kind, Node *lhs, Node *rhs);

Node *new_node_num(int val);

// Node *program();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

void gen(Node *node);

// int main(int argc, char **argv);
