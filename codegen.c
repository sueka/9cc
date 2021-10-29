#include "9cc.h"

void gen_lval(Node *node) {
  if (node->kind != ND_LVAR) {
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
    case ND_LVAR:
    gen_lval(node);

    // スタックトップに node のアドレスがあるので、そこにある値で置き換える。
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
    case ND_ASSIGN:
    gen_lval(node->lhs);
    gen(node->rhs);

    // スタックトップに代入する値、次に左辺のアドレスがあるので、左辺のアドレスにある値をスタックトップの値で置き換える。代入値をスタックに戻す。
    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  mov [rax], rdi\n");
    printf("  push rdi\n");
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
      return;
    }
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
    printf("  add rax, rdi\n");
    break;
    case ND_SUB:
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
