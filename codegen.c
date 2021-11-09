#include "9cc.h"

void gen_lval(Node *node) {
  if (node->kind != ND_LVAR && node->kind != ND_DEF) {
    error_at(token->str, "代入の左辺値が変数ではありません。");
  }

  // node のアドレス（rbp から offset を引いた値）を push する
  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
}

// 制御構造用の通し番号
int c = 0;

char *regs[6] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

void gen(Node *node) {
  switch (node->kind) {
    case ND_NUM:
      printf("  push %d\n", node->val);
      return;
    case ND_DEF:
      // noop; プロローグで変数26個分の領域を固定で確保しているため、ここでは何もしない。
      return;
    case ND_LVAR:
      gen_lval(node);

      // スタックトップに node のアドレスがあるので、そこにある値で置き換える。
      printf("  pop rax\n");
      printf("  mov rax, [rax]\n");
      printf("  push rax\n");
      return;
    case ND_ASSIGN:
      // 左辺がデリファレンスされている場合、デリファレンスされている値はアドレスのはずなので、右辺値としてコンパイルすれば代入先が得られる。
      if (node->lhs->kind == ND_DEREF) {
        gen(node->lhs->lhs);
      } else {
        gen_lval(node->lhs);
      }

      gen(node->rhs);

      // スタックトップに代入する値、次に左辺のアドレスがあるので、左辺のアドレスにある値をスタックトップの値で置き換える。代入値をスタックに戻す。
      printf("  pop rdi\n");
      printf("  pop rax\n");
      printf("  mov [rax], rdi\n");
      printf("  push rdi\n");
      return;
    case ND_ADDR:
      gen_lval(node->lhs);
      return;
    case ND_DEREF:
      gen(node->lhs);

      // スタックトップにアドレスがあるので、そこにある値で置き換える。
      printf("  pop rax\n");
      printf("  mov rax, [rax]\n");
      printf("  push rax\n");
      return;
    case ND_RETURN:
      gen(node->lhs);

      printf("  pop rax\n");
      printf("  mov rsp, rbp\n");
      printf("  pop rbp\n");
      printf("  ret\n");
      return;
    case ND_IF:
      gen(node->cond); // スタックトップに結果が入っているはず

      printf("  pop rax\n");
      printf("  cmp rax, 0\n");

      if (!node->otherwise) { // if
        printf("  je  .Lend%d\n", c);
        gen(node->then);
      } else { // if-else
        printf("  je  .Lelse%d\n", c);
        gen(node->then);
        printf("  jmp .Lend%d\n", c);
        printf(".Lelse%d:\n", c);
        gen(node->otherwise);
      }

      printf(".Lend%d:\n", c);

      ++c;
      return;
    case ND_WHILE:
      printf(".Lbegin%d:\n", c);
      gen(node->cond);
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");
      printf("  je  .Lend%d\n", c);
      gen(node->body);
      printf("  jmp .Lbegin%d\n", c);
      printf(".Lend%d:\n", c);
      ++c;
      return;
    case ND_FOR:
      gen(node->init);
      printf(".Lbegin%d:\n", c);
      gen(node->cond);
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");
      printf("  je  .Lend%d\n", c);
      gen(node->body);
      gen(node->iter);
      printf("  jmp .Lbegin%d\n", c);
      printf(".Lend%d:\n", c);
      ++c;
      return;
    case ND_BLOCK:
      for (int i = 0; node->stmts[i]; ++i) {
        gen(node->stmts[i]);
        printf("  pop rax\n");
      }
      return;
    case ND_FCALL: {
      int i = 0;

      for (i = 0; node->args[i]; ++i) {
        gen(node->args[i]);
      }

      for (int j = i - 1; j >= 0; --j) {
        printf("  pop %s\n", regs[j]);
      }

      // TODO: RSP を16の倍数にアラインする

      printf("  call %.*s\n", node->len, node->name);

      // call された関数の戻り値は rax と rdx （あれば）に入っている。現在のバージョンの 9cc では式の評価結果はスタックトップにあるので、 rax をスタックに push する。
      // FIXME: スタックではまずいことがあるかも
      printf("  push rax\n");

      return;
    }
    case ND_FDEFN:
      printf("%.*s:\n", node->len, node->name);

      // プロローグ
      // 変数26個分の領域を確保する
      printf("  push rbp\n");
      printf("  mov rbp, rsp\n");
      printf("  sub rsp, 208\n");

      int i;

      for (i = 0; node->args[i]; ++i) {
        gen_lval(node->args[i]);
      }

      // RDI, RSI, RDX, RCX, R8, R9 に実引数、スタックトップに上（成長元）から順に仮引数のアドレスがあるので、仮引数のアドレスを pop し、仮引数のアドレスにある値を実引数で置き換える。
      for (int j = i - 1; j >= 0; --j) {
        printf("  pop rax\n");
        printf("  mov [rax], %s\n", regs[j]);
      }

      gen(node->body);

      // 式の評価結果としてスタックに1つの値が残っているはずなので、スタックが溢れないように pop しておく
      printf("  pop rax\n");

      // エピローグ
      // 最後の式の結果が RAX に残っているので、それを返り値とする
      printf("  mov rsp, rbp\n");
      printf("  pop rbp\n");
      printf("  ret\n");

      return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n"); // right-hand
  printf("  pop rax\n"); // left-hand

  switch (node->kind) {
    case ND_EQ:
      printf("  cmp rax, rdi\n");
      printf("  sete al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_NE:
      printf("  cmp rax, rdi\n");
      printf("  setne al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_LT:
      printf("  cmp rax, rdi\n");
      printf("  setl al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_LTE:
      printf("  cmp rax, rdi\n");
      printf("  setle al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_ADD:
      if (node->lhs->kind == ND_LVAR && node->lhs->ty->ty == PTR) {
        int s = _sizeof(node->lhs->ty->ptr_to);

        while (s--) {
          printf("  add rax, rdi\n");
        }

        break;
      }

      printf("  add rax, rdi\n");
      break;
    case ND_SUB:
      if (node->lhs->kind == ND_LVAR && node->lhs->ty->ty == PTR) {
        int s = _sizeof(node->lhs->ty->ptr_to);

        while (s--) {
          printf("  sub rax, rdi\n");
        }

        break;
      }

      printf("  sub rax, rdi\n");
      break;
    case ND_MUL:
      printf("  imul rax, rdi\n");
      break;
    case ND_DIV:
      printf("  cqo\n");
      printf("  idiv rdi\n");
      break;
  }

  printf("  push rax\n");
}
