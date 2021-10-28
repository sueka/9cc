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
