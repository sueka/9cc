#include "9cc.h"

// エラーを報告するための関数
// printf と同じ引数を取る。
void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// 入力プログラム
char *user_input;

// エラー箇所を報告する。
void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  int pos = loc - user_input;

  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, " "); // pos 個の空白を出力
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");

  exit(2);
}

int main(int argc, char **argv) {
  if (argc != 2) {
    error("引数の個数が正しくありません。");

    return 1;
  }

  // トークナイズしてパーズする
  // 結果は code に保存される
  user_input = argv[1];
  token = tokenize(user_input);
  program();

  // アセンブリの前半部分を出力
  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("\n");

  // 先頭の式から順に、抽象構文木を下りながらコード生成
  for (int i = 0; code[i]; ++i) {
    gen(code[i]);
  }

  return 0;
}
