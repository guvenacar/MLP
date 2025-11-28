section .text
global _start

_start:
    push rbp
    mov rbp, rsp

    ; Declaration: numeric lexer_pos
    sub rsp, 8         ; Allocate space for lexer_pos
    ; Declaration: numeric lexer_line
    sub rsp, 8         ; Allocate space for lexer_line
    ; Declaration: numeric lexer_token_count
    sub rsp, 8         ; Allocate space for lexer_token_count
    ; Declaration: numeric parser_current_token
    sub rsp, 8         ; Allocate space for parser_current_token
    ; Declaration: numeric parser_token_index
    sub rsp, 8         ; Allocate space for parser_token_index
    ; Declaration: numeric ast_declaration_count
    sub rsp, 8         ; Allocate space for ast_declaration_count
    ; Declaration: numeric ast_capacity
    sub rsp, 8         ; Allocate space for ast_capacity
    ; Declaration: numeric codegen_stack_offset
    sub rsp, 8         ; Allocate space for codegen_stack_offset
    ; Declaration: numeric codegen_label_count
    sub rsp, 8         ; Allocate space for codegen_label_count
    ; Declaration: numeric TYPE_NUMERIC
    sub rsp, 8         ; Allocate space for TYPE_NUMERIC
    ; Declaration: numeric TYPE_DECIMAL
    sub rsp, 8         ; Allocate space for TYPE_DECIMAL
    ; Declaration: numeric TYPE_BOOLEAN
    sub rsp, 8         ; Allocate space for TYPE_BOOLEAN
    ; Declaration: numeric TOKEN_NUMERIC
    sub rsp, 8         ; Allocate space for TOKEN_NUMERIC
    ; Declaration: numeric TOKEN_DECIMAL
    sub rsp, 8         ; Allocate space for TOKEN_DECIMAL
    ; Declaration: numeric TOKEN_BOOLEAN
    sub rsp, 8         ; Allocate space for TOKEN_BOOLEAN
    ; Declaration: numeric TOKEN_IDENTIFIER
    sub rsp, 8         ; Allocate space for TOKEN_IDENTIFIER
    ; Declaration: numeric TOKEN_EOF
    sub rsp, 8         ; Allocate space for TOKEN_EOF

    ; Exit program
    mov rax, 60        ; sys_exit
    xor rdi, rdi       ; exit code 0
    syscall
