#include "9cc.h"

// 変数の型のサイズを計算する
int _sizeof(Type *ty) {
  switch (ty->ty) {
    case INT:
      return 4;
    case PTR:
      return 8;
  }
}
