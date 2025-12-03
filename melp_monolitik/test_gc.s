section .data
    ; String literals will be added here

section .text
extern print_number
extern print_string
extern string_length
extern string_concat
extern string_equal
extern string_not_equal
extern int_to_string
extern malloc
extern free
extern mlp_array_alloc
extern mlp_array_free
extern mlp_array_length
extern mlp_array_resize
extern mlp_range
extern mlp_range1
extern mlp_range2
extern mlp_file_read
extern mlp_file_write
extern mlp_file_exists
extern mlp_file_append
extern mlp_substring
extern mlp_indexOf
extern mlp_charAt
extern mlp_string_length
extern mlp_get_argv
extern mlp_get_argc
extern setjmp
extern strcmp
extern mlp_exception_push
extern mlp_exception_pop
extern mlp_throw
extern mlp_exception_type
extern mlp_exception_message
extern mlp_exception_code
extern mlp_exception_has_handler
extern mlp_exception_has_parent_handler
extern gc_init
extern gc_alloc
extern gc_free
extern gc_retain
extern gc_release
extern gc_collect
extern gc_full_collect
extern gc_shutdown
extern gc_get_object_count
extern gc_get_total_bytes
extern gc_get_collections
extern gc_enable
extern gc_disable
global _start


_start:
    ; Get argc and argv from stack
    pop rdi               ; argc (first item on stack)
    mov rsi, rsp          ; argv (pointer to argv[0])
    call mlp_get_argv     ; Convert to MLP string array

    ; Initialize GC
    call gc_init

    push rbp
    mov rbp, rsp


    ; Print statement
    mov rax, str_0
    mov rdi, rax
    call print_string
    ; Declaration: numeric[] arr1 (array, size=10, total=80 bytes)
    sub rsp, 80         ; Allocate space for array arr1
    mov qword [rbp-80], 0   ; Initialize arr1[0]
    mov qword [rbp-72], 0   ; Initialize arr1[1]
    mov qword [rbp-64], 0   ; Initialize arr1[2]
    mov qword [rbp-56], 0   ; Initialize arr1[3]
    mov qword [rbp-48], 0   ; Initialize arr1[4]
    mov qword [rbp-40], 0   ; Initialize arr1[5]
    mov qword [rbp-32], 0   ; Initialize arr1[6]
    mov qword [rbp-24], 0   ; Initialize arr1[7]
    mov qword [rbp-16], 0   ; Initialize arr1[8]
    mov qword [rbp-8], 0   ; Initialize arr1[9]
    ; Declaration: numeric[] arr2 (array, size=20, total=160 bytes)
    sub rsp, 160         ; Allocate space for array arr2
    mov qword [rbp-240], 0   ; Initialize arr2[0]
    mov qword [rbp-232], 0   ; Initialize arr2[1]
    mov qword [rbp-224], 0   ; Initialize arr2[2]
    mov qword [rbp-216], 0   ; Initialize arr2[3]
    mov qword [rbp-208], 0   ; Initialize arr2[4]
    mov qword [rbp-200], 0   ; Initialize arr2[5]
    mov qword [rbp-192], 0   ; Initialize arr2[6]
    mov qword [rbp-184], 0   ; Initialize arr2[7]
    mov qword [rbp-176], 0   ; Initialize arr2[8]
    mov qword [rbp-168], 0   ; Initialize arr2[9]
    mov qword [rbp-160], 0   ; Initialize arr2[10]
    mov qword [rbp-152], 0   ; Initialize arr2[11]
    mov qword [rbp-144], 0   ; Initialize arr2[12]
    mov qword [rbp-136], 0   ; Initialize arr2[13]
    mov qword [rbp-128], 0   ; Initialize arr2[14]
    mov qword [rbp-120], 0   ; Initialize arr2[15]
    mov qword [rbp-112], 0   ; Initialize arr2[16]
    mov qword [rbp-104], 0   ; Initialize arr2[17]
    mov qword [rbp-96], 0   ; Initialize arr2[18]
    mov qword [rbp-88], 0   ; Initialize arr2[19]
    ; Declaration: numeric[] arr3 (array, size=30, total=240 bytes)
    sub rsp, 240         ; Allocate space for array arr3
    mov qword [rbp-480], 0   ; Initialize arr3[0]
    mov qword [rbp-472], 0   ; Initialize arr3[1]
    mov qword [rbp-464], 0   ; Initialize arr3[2]
    mov qword [rbp-456], 0   ; Initialize arr3[3]
    mov qword [rbp-448], 0   ; Initialize arr3[4]
    mov qword [rbp-440], 0   ; Initialize arr3[5]
    mov qword [rbp-432], 0   ; Initialize arr3[6]
    mov qword [rbp-424], 0   ; Initialize arr3[7]
    mov qword [rbp-416], 0   ; Initialize arr3[8]
    mov qword [rbp-408], 0   ; Initialize arr3[9]
    mov qword [rbp-400], 0   ; Initialize arr3[10]
    mov qword [rbp-392], 0   ; Initialize arr3[11]
    mov qword [rbp-384], 0   ; Initialize arr3[12]
    mov qword [rbp-376], 0   ; Initialize arr3[13]
    mov qword [rbp-368], 0   ; Initialize arr3[14]
    mov qword [rbp-360], 0   ; Initialize arr3[15]
    mov qword [rbp-352], 0   ; Initialize arr3[16]
    mov qword [rbp-344], 0   ; Initialize arr3[17]
    mov qword [rbp-336], 0   ; Initialize arr3[18]
    mov qword [rbp-328], 0   ; Initialize arr3[19]
    mov qword [rbp-320], 0   ; Initialize arr3[20]
    mov qword [rbp-312], 0   ; Initialize arr3[21]
    mov qword [rbp-304], 0   ; Initialize arr3[22]
    mov qword [rbp-296], 0   ; Initialize arr3[23]
    mov qword [rbp-288], 0   ; Initialize arr3[24]
    mov qword [rbp-280], 0   ; Initialize arr3[25]
    mov qword [rbp-272], 0   ; Initialize arr3[26]
    mov qword [rbp-264], 0   ; Initialize arr3[27]
    mov qword [rbp-256], 0   ; Initialize arr3[28]
    mov qword [rbp-248], 0   ; Initialize arr3[29]

    ; For-in loop
    sub rsp, 8         ; Allocate space for i
    sub rsp, 8         ; Allocate space for loop index
    sub rsp, 8         ; Allocate space for array base
    sub rsp, 8         ; Allocate space for array length
    mov rax, 0
    push rax
    mov rax, 10
    push rax
    pop rsi
    pop rdi
    call mlp_range2
    mov [rbp-504], rax   ; Store array pointer
    mov rdi, rax
    call mlp_array_length
    mov [rbp-512], rax   ; Store array length
    mov qword [rbp-496], 0   ; Initialize index
.L0:  ; for_in_start
    mov rax, [rbp-496]
    cmp rax, [rbp-512]
    jge .L1
    mov rdi, [rbp-504]   ; Array base
    mov rsi, [rbp-496]   ; Index
    mov rax, [rdi + rsi*8]   ; Load element
    mov [rbp-488], rax   ; Store in loop variable

    ; Array assignment: arr1[...] = ...
    mov rax, [rbp-488]
    push rax            ; Save index
    mov rax, [rbp-488]
    push rax
    mov rax, 10
    mov rbx, rax
    pop rax
    imul rax, rbx
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    mov rcx, 8          ; Element size
    imul rax, rcx       ; index * 8
    mov rcx, 80         ; Base offset
    sub rcx, rax        ; base - (index * 8)
    neg rcx             ; -(base - index*8)
    mov [rbp + rcx], rbx ; Store value to array[index]
.L2:  ; for_in_continue
    mov rax, [rbp-496]
    add rax, 1
    mov [rbp-496], rax
    jmp .L0
.L1:  ; for_in_end

    ; For-in loop
    sub rsp, 8         ; Allocate space for i
    sub rsp, 8         ; Allocate space for loop index
    sub rsp, 8         ; Allocate space for array base
    sub rsp, 8         ; Allocate space for array length
    mov rax, 0
    push rax
    mov rax, 20
    push rax
    pop rsi
    pop rdi
    call mlp_range2
    mov [rbp-536], rax   ; Store array pointer
    mov rdi, rax
    call mlp_array_length
    mov [rbp-544], rax   ; Store array length
    mov qword [rbp-528], 0   ; Initialize index
.L3:  ; for_in_start
    mov rax, [rbp-528]
    cmp rax, [rbp-544]
    jge .L4
    mov rdi, [rbp-536]   ; Array base
    mov rsi, [rbp-528]   ; Index
    mov rax, [rdi + rsi*8]   ; Load element
    mov [rbp-520], rax   ; Store in loop variable

    ; Array assignment: arr2[...] = ...
    mov rax, [rbp-520]
    push rax            ; Save index
    mov rax, [rbp-520]
    push rax
    mov rax, 5
    mov rbx, rax
    pop rax
    imul rax, rbx
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    mov rcx, 8          ; Element size
    imul rax, rcx       ; index * 8
    mov rcx, 240         ; Base offset
    sub rcx, rax        ; base - (index * 8)
    neg rcx             ; -(base - index*8)
    mov [rbp + rcx], rbx ; Store value to array[index]
.L5:  ; for_in_continue
    mov rax, [rbp-528]
    add rax, 1
    mov [rbp-528], rax
    jmp .L3
.L4:  ; for_in_end

    ; For-in loop
    sub rsp, 8         ; Allocate space for i
    sub rsp, 8         ; Allocate space for loop index
    sub rsp, 8         ; Allocate space for array base
    sub rsp, 8         ; Allocate space for array length
    mov rax, 0
    push rax
    mov rax, 30
    push rax
    pop rsi
    pop rdi
    call mlp_range2
    mov [rbp-568], rax   ; Store array pointer
    mov rdi, rax
    call mlp_array_length
    mov [rbp-576], rax   ; Store array length
    mov qword [rbp-560], 0   ; Initialize index
.L6:  ; for_in_start
    mov rax, [rbp-560]
    cmp rax, [rbp-576]
    jge .L7
    mov rdi, [rbp-568]   ; Array base
    mov rsi, [rbp-560]   ; Index
    mov rax, [rdi + rsi*8]   ; Load element
    mov [rbp-552], rax   ; Store in loop variable

    ; Array assignment: arr3[...] = ...
    mov rax, [rbp-552]
    push rax            ; Save index
    mov rax, [rbp-552]
    push rax
    mov rax, 2
    mov rbx, rax
    pop rax
    imul rax, rbx
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    mov rcx, 8          ; Element size
    imul rax, rcx       ; index * 8
    mov rcx, 480         ; Base offset
    sub rcx, rax        ; base - (index * 8)
    neg rcx             ; -(base - index*8)
    mov [rbp + rcx], rbx ; Store value to array[index]
.L8:  ; for_in_continue
    mov rax, [rbp-560]
    add rax, 1
    mov [rbp-560], rax
    jmp .L6
.L7:  ; for_in_end

    ; Print statement
    mov rax, str_1
    mov rdi, rax
    call print_string

    ; Print statement

    ; Array indexing
    mov rax, 5
    mov rbx, 8          ; Element size
    imul rax, rbx       ; index * 8
    mov rbx, 80         ; Base offset
    sub rbx, rax        ; base - (index * 8)
    neg rbx             ; -(base - index*8)
    mov rax, [rbp + rbx] ; Load array[index]
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, str_2
    mov rdi, rax
    call print_string

    ; Print statement

    ; Array indexing
    mov rax, 10
    mov rbx, 8          ; Element size
    imul rax, rbx       ; index * 8
    mov rbx, 240         ; Base offset
    sub rbx, rax        ; base - (index * 8)
    neg rbx             ; -(base - index*8)
    mov rax, [rbp + rbx] ; Load array[index]
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, str_3
    mov rdi, rax
    call print_string

    ; Print statement

    ; Array indexing
    mov rax, 20
    mov rbx, 8          ; Element size
    imul rax, rbx       ; index * 8
    mov rbx, 480         ; Base offset
    sub rbx, rax        ; base - (index * 8)
    neg rbx             ; -(base - index*8)
    mov rax, [rbp + rbx] ; Load array[index]
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, str_4
    mov rdi, rax
    call print_string
    call gc_collect

    ; Print statement
    mov rax, str_5
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, str_6
    mov rdi, rax
    call print_string
    ; Declaration: numeric obj_count
    sub rsp, 8         ; Allocate space for obj_count
    call gc_get_object_count
    mov [rbp-584], rax   ; Initialize obj_count

    ; Print statement
    mov rax, str_7
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, [rbp-584]
    mov rdi, rax
    call print_number
    ; Declaration: numeric total_bytes
    sub rsp, 8         ; Allocate space for total_bytes
    call gc_get_total_bytes
    mov [rbp-592], rax   ; Initialize total_bytes

    ; Print statement
    mov rax, str_8
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, [rbp-592]
    mov rdi, rax
    call print_number
    ; Declaration: numeric collections
    sub rsp, 8         ; Allocate space for collections
    call gc_get_collections
    mov [rbp-600], rax   ; Initialize collections

    ; Print statement
    mov rax, str_9
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, [rbp-600]
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, str_10
    mov rdi, rax
    call print_string

    ; For-in loop
    sub rsp, 8         ; Allocate space for i
    sub rsp, 8         ; Allocate space for loop index
    sub rsp, 8         ; Allocate space for array base
    sub rsp, 8         ; Allocate space for array length
    mov rax, 0
    push rax
    mov rax, 100
    push rax
    pop rsi
    pop rdi
    call mlp_range2
    mov [rbp-624], rax   ; Store array pointer
    mov rdi, rax
    call mlp_array_length
    mov [rbp-632], rax   ; Store array length
    mov qword [rbp-616], 0   ; Initialize index
.L9:  ; for_in_start
    mov rax, [rbp-616]
    cmp rax, [rbp-632]
    jge .L10
    mov rdi, [rbp-624]   ; Array base
    mov rsi, [rbp-616]   ; Index
    mov rax, [rdi + rsi*8]   ; Load element
    mov [rbp-608], rax   ; Store in loop variable
    ; Declaration: numeric[] temp_arr (array, size=50, total=400 bytes)
    sub rsp, 400         ; Allocate space for array temp_arr
    mov qword [rbp-1032], 0   ; Initialize temp_arr[0]
    mov qword [rbp-1024], 0   ; Initialize temp_arr[1]
    mov qword [rbp-1016], 0   ; Initialize temp_arr[2]
    mov qword [rbp-1008], 0   ; Initialize temp_arr[3]
    mov qword [rbp-1000], 0   ; Initialize temp_arr[4]
    mov qword [rbp-992], 0   ; Initialize temp_arr[5]
    mov qword [rbp-984], 0   ; Initialize temp_arr[6]
    mov qword [rbp-976], 0   ; Initialize temp_arr[7]
    mov qword [rbp-968], 0   ; Initialize temp_arr[8]
    mov qword [rbp-960], 0   ; Initialize temp_arr[9]
    mov qword [rbp-952], 0   ; Initialize temp_arr[10]
    mov qword [rbp-944], 0   ; Initialize temp_arr[11]
    mov qword [rbp-936], 0   ; Initialize temp_arr[12]
    mov qword [rbp-928], 0   ; Initialize temp_arr[13]
    mov qword [rbp-920], 0   ; Initialize temp_arr[14]
    mov qword [rbp-912], 0   ; Initialize temp_arr[15]
    mov qword [rbp-904], 0   ; Initialize temp_arr[16]
    mov qword [rbp-896], 0   ; Initialize temp_arr[17]
    mov qword [rbp-888], 0   ; Initialize temp_arr[18]
    mov qword [rbp-880], 0   ; Initialize temp_arr[19]
    mov qword [rbp-872], 0   ; Initialize temp_arr[20]
    mov qword [rbp-864], 0   ; Initialize temp_arr[21]
    mov qword [rbp-856], 0   ; Initialize temp_arr[22]
    mov qword [rbp-848], 0   ; Initialize temp_arr[23]
    mov qword [rbp-840], 0   ; Initialize temp_arr[24]
    mov qword [rbp-832], 0   ; Initialize temp_arr[25]
    mov qword [rbp-824], 0   ; Initialize temp_arr[26]
    mov qword [rbp-816], 0   ; Initialize temp_arr[27]
    mov qword [rbp-808], 0   ; Initialize temp_arr[28]
    mov qword [rbp-800], 0   ; Initialize temp_arr[29]
    mov qword [rbp-792], 0   ; Initialize temp_arr[30]
    mov qword [rbp-784], 0   ; Initialize temp_arr[31]
    mov qword [rbp-776], 0   ; Initialize temp_arr[32]
    mov qword [rbp-768], 0   ; Initialize temp_arr[33]
    mov qword [rbp-760], 0   ; Initialize temp_arr[34]
    mov qword [rbp-752], 0   ; Initialize temp_arr[35]
    mov qword [rbp-744], 0   ; Initialize temp_arr[36]
    mov qword [rbp-736], 0   ; Initialize temp_arr[37]
    mov qword [rbp-728], 0   ; Initialize temp_arr[38]
    mov qword [rbp-720], 0   ; Initialize temp_arr[39]
    mov qword [rbp-712], 0   ; Initialize temp_arr[40]
    mov qword [rbp-704], 0   ; Initialize temp_arr[41]
    mov qword [rbp-696], 0   ; Initialize temp_arr[42]
    mov qword [rbp-688], 0   ; Initialize temp_arr[43]
    mov qword [rbp-680], 0   ; Initialize temp_arr[44]
    mov qword [rbp-672], 0   ; Initialize temp_arr[45]
    mov qword [rbp-664], 0   ; Initialize temp_arr[46]
    mov qword [rbp-656], 0   ; Initialize temp_arr[47]
    mov qword [rbp-648], 0   ; Initialize temp_arr[48]
    mov qword [rbp-640], 0   ; Initialize temp_arr[49]

    ; Array assignment: temp_arr[...] = ...
    mov rax, 0
    push rax            ; Save index
    mov rax, [rbp-608]
    mov rbx, rax        ; Save value in rbx
    pop rax             ; Restore index
    mov rcx, 8          ; Element size
    imul rax, rcx       ; index * 8
    mov rcx, 1032         ; Base offset
    sub rcx, rax        ; base - (index * 8)
    neg rcx             ; -(base - index*8)
    mov [rbp + rcx], rbx ; Store value to array[index]
.L11:  ; for_in_continue
    mov rax, [rbp-616]
    add rax, 1
    mov [rbp-616], rax
    jmp .L9
.L10:  ; for_in_end

    ; Print statement
    mov rax, str_11
    mov rdi, rax
    call print_string

    ; Assignment: obj_count = ...
    call gc_get_object_count
    mov [rbp-584], rax   ; Store to obj_count

    ; Print statement
    mov rax, str_12
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, [rbp-584]
    mov rdi, rax
    call print_number

    ; Assignment: collections = ...
    call gc_get_collections
    mov [rbp-600], rax   ; Store to collections

    ; Print statement
    mov rax, str_13
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, [rbp-600]
    mov rdi, rax
    call print_number

    ; Print statement
    mov rax, str_14
    mov rdi, rax
    call print_string

    ; Print statement
    mov rax, 999
    mov rdi, rax
    call print_number

    ; Shutdown GC
    call gc_shutdown

    ; Exit program
    mov rax, 60        ; sys_exit
    xor rdi, rdi       ; exit code 0
    syscall

section .data
str_14: db "=== GC Test Complete ===", 0
str_13: db "Collections:", 0
str_12: db "Object count:", 0
str_11: db "=== After Stress Test ===", 0
str_10: db "=== Stress Test: Create many objects ===", 0
str_9: db "Collections:", 0
str_8: db "Total bytes:", 0
str_7: db "Object count:", 0
str_6: db "=== GC Statistics ===", 0
str_5: db "GC collect completed", 0
str_4: db "=== Calling gc_collect() ===", 0
str_3: db "arr3[20] =", 0
str_2: db "arr2[10] =", 0
str_1: db "arr1[5] =", 0
str_0: db "=== GC Test: Array Allocation ===", 0
