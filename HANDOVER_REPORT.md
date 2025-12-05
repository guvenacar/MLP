=================================================
MLP COMPILER - FUNCTIONS MODULE DEVREDİLİYOR
=================================================

TARİH: 5 Aralık 2025
MEVCUT DURUM: %40 Tamamlandı, %60 Eksik
SONRAKİ AI'YE GÖREV: Functions Module'ü bitir

─────────────────────────────────────────────────
1. TAMAMLANANLAR ✅
─────────────────────────────────────────────────

✅ Function Definition Parsing
   - Syntax: function name(type param, ...) ... end function
   - Parametre sayısı: Sınırsız
   - Tipler: numeric, text, boolean

✅ Return Statement Parsing
   - Syntax: return expression
   - Expression evaluation: a + b, a - b, a * b
   - Assembly: mov rax, result

✅ Stack Frame Setup
   - Prologue: push rbp, mov rbp, rsp
   - Epilogue: mov rsp, rbp, pop rbp, ret
   - Stack allocation: sub rsp, N

✅ x86-64 Calling Convention
   - Parameters: rdi, rsi, rdx, rcx, r8, r9
   - Return: rax

ÇALIŞAN KOD:
function add(numeric a, numeric b) numeric
    return a + b
end function

ÜRETİLEN ASSEMBLY:
add:
  push rbp
  mov rbp, rsp
  mov rax, rdi   ; a
  add rax, rsi   ; + b
  pop rbp
  ret

─────────────────────────────────────────────────
2. EKSİKLER ❌ (SONRAKİ AI BUNLARI TAMAMLAYACAK)
─────────────────────────────────────────────────

❌ Function Call Parsing ve Codegen
   
   HEDEF KOD:
   numeric result = add(10, 20)
   
   HEDEF ASSEMBLY:
   mov rdi, 10          ; 1st param
   mov rsi, 20          ; 2nd param
   call add             ; function call
   mov [rbp-8], rax     ; result = return value

❌ Local Variable Allocation
   
   HEDEF KOD:
   numeric x = 10
   numeric y = 20
   
   HEDEF ASSEMBLY:
   mov qword [rbp-8], 10    ; x
   mov qword [rbp-16], 20   ; y

❌ Variable Assignment in Function Body
   
   HEDEF KOD:
   x = x + 1
   
   HEDEF ASSEMBLY:
   mov rax, [rbp-8]     ; load x
   add rax, 1
   mov [rbp-8], rax     ; store x

❌ Print Statement in Function Body
   
   HEDEF KOD:
   print("Hello")
   print(x)
   
   HEDEF ASSEMBLY:
   lea rdi, [str_1]
   call print_string
   
   mov rdi, [rbp-8]
   call print_number

❌ Complex Expressions
   
   HEDEF KOD:
   return (a + b) * c
   
   HEDEF ASSEMBLY:
   mov rax, rdi    ; a
   add rax, rsi    ; + b
   imul rax, rdx   ; * c

❌ Recursion Support
   
   HEDEF KOD:
   function factorial(numeric n) numeric
       if n <= 1 then
           return 1
       end if
       return n * factorial(n - 1)
   end function

─────────────────────────────────────────────────
3. DOSYA YAPISI
─────────────────────────────────────────────────

ÇALIŞMA DİZİNİ:
~/projeler/MLP/MLP/melp/C/stage0/modules/functions/

ANA DOSYA:
functions_standalone.c (241 satır)

YAPILMASI GEREKEN:
functions_standalone.c içinde parse_function_body() 
fonksiyonuna yukarıdaki eksik özellikleri ekle.

TEST DOSYALARI:
test_functions.mlp          (basit test)
test_real_function.mlp      (return test ✅)
test_function_call.mlp      (function call test ❌)

─────────────────────────────────────────────────
4. SONRAKİ AI İÇİN TALİMATLAR
─────────────────────────────────────────────────

GÖREV: Functions Module'ü %100 tamamla

ADIM 1: Function Call Parsing Ekle (1 saat)
────────────────────────────────────────────

Pattern: numeric result = function_name(arg1, arg2)

Implementation:
- TOKEN_IDENTIFIER (variable)
- TOKEN_ASSIGN (=)
- TOKEN_IDENTIFIER (function name)
- TOKEN_LPAREN (
- Parse arguments (TOKEN_NUMBER veya TOKEN_IDENTIFIER)
- TOKEN_RPAREN )

Codegen:
for (int i = 0; i < arg_count; i++) {
    fprintf(output, "  mov %s, %d\n", regs[i], args[i]);
}
fprintf(output, "  call %s\n", func_name);
fprintf(output, "  mov [rbp-8], rax\n");

Test:
function add(numeric a, numeric b) numeric
    return a + b
end function

function main() numeric
    numeric result = add(10, 20)
    return result
end function

Beklenen Assembly:
main:
  push rbp
  mov rbp, rsp
  mov rdi, 10
  mov rsi, 20
  call add
  mov [rbp-8], rax
  mov rax, [rbp-8]
  pop rbp
  ret

ADIM 2: Local Variables (30 dk)
────────────────────────────────

Pattern: numeric x = 10

Stack offset tracking:
- Herbir local variable için rbp'den offset tut
- x = rbp-8, y = rbp-16, z = rbp-24, ...

Codegen:
fprintf(output, "  mov qword [rbp-%d], %d\n", offset, value);

ADIM 3: Print in Function Body (30 dk)
───────────────────────────────────────

Mevcut print modülünü entegre et:
melp/C/stage0/modules/print/print_standalone.c

ADIM 4: Variable Assignment (30 dk)
────────────────────────────────────

Pattern: x = x + 1

Codegen:
mov rax, [rbp-offset]
add rax, 1
mov [rbp-offset], rax

ADIM 5: Recursion Test (30 dk)
───────────────────────────────

Test:
function factorial(numeric n) numeric
    if n <= 1 then
        return 1
    end if
    return n * factorial(n - 1)
end function

Control flow + function call kombinasyonu

─────────────────────────────────────────────────
5. TAHMİNİ SÜRE
─────────────────────────────────────────────────

Toplam: 3-4 saat
- Function call: 1 saat
- Local variables: 30 dk
- Print: 30 dk
- Assignment: 30 dk
- Recursion: 1 saat
- Test ve debug: 30 dk

─────────────────────────────────────────────────
6. BAŞARI KRİTERLERİ
─────────────────────────────────────────────────

Functions Module %100 tamamlanmış sayılır eğer:

✅ Function definition ✅
✅ Function call ❌
✅ Return statement ✅
✅ Local variables ❌
✅ Variable assignment ❌
✅ Print in function ❌
✅ Recursion ❌

TEST:
function factorial(numeric n) numeric
    print("Computing factorial")
    if n <= 1 then
        return 1
    end if
    numeric result = n * factorial(n - 1)
    return result
end function

function main() numeric
    numeric answer = factorial(5)
    print(answer)
    return 0
end function

EXPECTED OUTPUT:
Computing factorial
Computing factorial
Computing factorial
Computing factorial
Computing factorial
120

─────────────────────────────────────────────────
7. NOTLAR
─────────────────────────────────────────────────

- Context overflow nedeniyle önceki AI yoruldu
- %40 iş yapıldı, %60 kaldı
- Yeni AI temiz context ile başlıyor
- Modüler yaklaşım: Her özelliği ayrı ekle ve test et
- Acele etme, her adımı doğrula

─────────────────────────────────────────────────
8. MEVCUT MODÜLLER (8/63)
─────────────────────────────────────────────────

1. ✅ Print Module - String output
2. ✅ Variable Module - numeric/text/boolean
3. ✅ Comments Module - --- ve -- removal
4. ✅ Arithmetic Module - +, -, *, /, %
5. ✅ Control Flow Module - if/else, while, for
6. ✅ Comparison Module - ==, !=, <, >, <=, >=
7. ✅ Logical Module - and, or, not
8. ⏳ Functions Module - %40 tamamlandı

İLERLEME: ~13% (8/63 modül)

─────────────────────────────────────────────────
9. DOSYA KONUMLARI
─────────────────────────────────────────────────

ÇALIŞMA DİZİNİ:
/home/pardus/projeler/MLP/MLP

MODÜL DOSYASI:
melp/C/stage0/modules/functions/functions_standalone.c

LEXİKAL ANALYZER:
melp/C/stage0/lexer.c
melp/C/stage0/lexer.h

COMMENTS MODULE:
melp/C/stage0/modules/comments/comments.c

TEST DOSYALARI:
test_functions.mlp
test_real_function.mlp
test_function_call.mlp (YENİ - oluşturulacak)

ASSEMBLY ÇIKTISI:
/tmp/output.s

BUILD KOMUTU:
gcc -o functions_standalone functions_standalone.c ../../lexer.c ../comments/comments.c -I../../

─────────────────────────────────────────────────
10. ÖZET
─────────────────────────────────────────────────

📊 TAMAMLANAN: %40
   - Function definition parsing
   - Return statement
   - Basic expression evaluation
   - Stack frame management
   - x86-64 calling convention

📋 KALAN: %60
   - Function call mechanism
   - Local variable allocation
   - Variable assignment
   - Print in function body
   - Complex expressions
   - Recursion support

🎯 HEDEF: Functions Module %100 tamamlamak

⏱️ TAHMİNİ SÜRE: 3-4 saat

🔧 YÖNTEM: Modüler implementation + test

=================================================
RAPOR SONU - BAŞARILAR!
=================================================
