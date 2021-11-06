#include "9cc.h"

// 変数の型のサイズを計算する
int _sizeof(Type *ty) {
  if (ty->ty == INT) {
    return 4;
  }

  if (ty->ty == PTR) {
    return 8;
  }
}
