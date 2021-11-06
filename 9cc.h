#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// トークンの種類
typedef enum {
  TK_RESERVED, // 記号
  TK_RETURN,   // return キーワード
  TK_IF,       // if キーワード
  TK_ELSE,     // else キーワード
  TK_WHILE,    // while キーワード
  TK_FOR,      // for キーワード
  TK_INT,      // int; TODO: 将来的には識別子になるはず……
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

// 入力文字列 p をトークナイズしてそれを返す。
Token *tokenize(char *p);

// 新しいトークンを作成して cur に繋げる。
Token *new_token(TokenKind kind, Token *cur, char *str, int len);

int is_alnum(char c);

// 抽象構文木のノードの種類
typedef enum {
  ND_RETURN, // return
  ND_IF,     // if, if-else
  ND_WHILE,  // while
  ND_FOR,    // for
  ND_BLOCK,  // ブロック
  ND_ASSIGN, // =
  ND_EQ,     // ==
  ND_NE,     // !-
  ND_LT,     // <
  ND_LTE,    // <=
  ND_ADD,    // +
  ND_SUB,    // -
  ND_ADDR,   // &
  ND_DEREF,  // *
  ND_MUL,    // *
  ND_DIV,    // /
  ND_NUM,    // 整数
  ND_DEF,    // 変数定義
  ND_LVAR,   // ローカル変数
  ND_FDEFN,  // 関数定義
  ND_FCALL,  // 関数呼び出し
} NodeKind;

typedef struct Node Node;
typedef struct Type Type;

// 抽象構文木のノードの型
struct Node {
  NodeKind kind; // ノードの型
  Node *lhs;     // 左辺
  Node *rhs;     // 右辺

  // kind が ND_NUM の場合のみ使う
  int val;

  // FIXME: ノードの時点では offset にせずに名前を保持した方が良いような気がする。
  // kind が ND_DEF/ND_LVAR の場合のみ使う
  int offset;

  Type *ty; // kind が ND_DEF/ND_LVAR の場合のみ使う

  // kind が ND_FDEFN/ND_FCALL の場合のみ使う
  char *name;
  int len;
  Node *args[6]; // params/args

  // kind が ND_BLOCK の場合のみ使う
  Node *stmts[20];

  // 制御構造用の値
  // if (cond) then else otherwise
  // while (cond) body
  // for (init; cond; iter) body
  Node *cond;
  Node *then;
  Node *otherwise;
  Node *body;
  Node *init;
  Node *iter;
};

typedef enum {
  INT,
  PTR,
} TypeKind;

struct Type {
  TypeKind ty;
  struct Type *ptr_to; // ty が PTR の場合のみ使う
};

Node *new_node(NodeKind kind, Node *lhs, Node *rhs);

Node *new_node_num(int val);

extern Node *code[100];

Node *program();
Node *funcdefn();
Node *stmt();
Node *block();
Node *expr();
Node *ldef();
Type *typename();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

typedef struct LVar LVar;

// ローカル変数の型
struct LVar {
  LVar *next; // 次の変数か NULL
  char *name; // 変数の名前
  int len;    // 名前の長さ
  int offset; // RBP からのオフセット
  Type *ty;   // 変数のデータ型
};

// ローカル変数
extern LVar *locals;

void gen(Node *node);

// int main(int argc, char **argv);
