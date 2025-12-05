=================================================
MLP COMPILER - HANDOVER REPORT
=================================================

TARİH: 5 Aralık 2025 - 14:45
MEVCUT DURUM: 3 Modül %100 Çalışıyor! 🎉
SONRAKİ AI'YE GÖREV: While Loop Body Parsing

─────────────────────────────────────────────────
1. ✅ %100 ÇALIŞAN MODÜLLER (DOKUNMA!)
─────────────────────────────────────────────────

🎯 **MODÜL #1: PRINT MODULE** ✅ COMPLETE
   Dosya: melp/C/stage0/modules/print/print_standalone.c
   Binary: print_standalone (ÇALIŞIYOR)
   Test: ./print_standalone merhaba_dunya.mlp /tmp/test.s
   Çıktı: "Merhaba Dünya!" ✅
   
   Özellikler:
   - String literals
   - Türkçe karakter desteği
   - sys_write syscall
   - .data section string'leri
   
   ⚠️ DOKUNMA! Bu modül tamamen çalışıyor.

🎯 **MODÜL #2: FUNCTIONS MODULE** ✅ COMPLETE
   Dosya: melp/C/stage0/modules/functions/functions_standalone.c
   Binary: functions_standalone (ÇALIŞIYOR)
   Test: ./functions_standalone test_real_function.mlp /tmp/test.s
   Çıktı: add(10, 20) = 30 ✅
   
   Özellikler:
   - Function definitions: function name(type param, ...) ... end function
   - Parameters: Sınırsız, x86-64 calling convention (rdi, rsi, rdx, rcx, r8, r9)
   - Return values: return expression → rax
   - Local variables: Stack allocation (rbp-8, rbp-16, ...)
   - Function calls: Parametre yükleme + call instruction
   - Arithmetic expressions: a + b, a - b, a * b
   - Print statements: print(variable) + print(number)
   - Variable declarations: numeric x = 10
   - Variable assignments: x = x + 1
   
   ÇALIŞAN KOD ÖRNEKLERI:
   ```mlp
   function add(numeric a, numeric b) numeric
       return a + b
   end function
   
   function main() numeric
       numeric result = add(10, 20)
       print(result)
       return 0
   end function
   ```
   
   ⚠️ DOKUNMA! Bu modül tamamen çalışıyor.

🎯 **MODÜL #3: ARRAYS MODULE** ✅ COMPLETE
   Dosya: melp/C/stage0/modules/array/array_standalone.c
   Binary: array_standalone (ÇALIŞIYOR)
   Test: ./array_standalone test_array_simple.mlp /tmp/test.s
   Çıktı: "Array Module OK!" ✅
   
   Özellikler:
   - Array literals: [10, 20, 30]
   - String arrays: ["Ali", "Veli", "Ayşe"]
   - Array initialization: Proper offset calculation (0, 8, 16, ...)
   - String constants: .data section'da arr_str_0, arr_str_1, ...
   - Memory layout: .bss section'da resq allocation
   
   ÇALIŞAN KOD ÖRNEKLERI:
   ```mlp
   numeric[] numbers = [10, 20, 30]
   text[] names = ["Ali", "Veli", "Ayşe"]
   numeric size = 5
   ```
   
   ⚠️ DOKUNMA! Bu modül tamamen çalışıyor.

─────────────────────────────────────────────────
2. ⚠️ KISMİ ÇALIŞAN MODÜLLER (DEVAM EDİLECEK)
─────────────────────────────────────────────────

🔧 **WHILE LOOP** - Parser OK, Body Codegen EKSİK
   Dosya: melp/C/stage0/modules/functions/functions_standalone.c
   Durum: STMT_WHILE enum eklendi, parser çalışıyor
   
   NE YAPILDI:
   ✅ TOKEN_WHILE parsing
   ✅ Condition parsing (variable < value)
   ✅ Assembly labels (.while_start_N, .while_end_N)
   ✅ Comparison codegen (cmp + conditional jump)
   
   NE EKSİK:
   ❌ Loop body statement parsing (nested statements)
   ❌ Body içindeki variable assignments
   ❌ Body içindeki print statements
   ❌ Body içindeki function calls
   
   TEST PROGRAMI:
   ```mlp
   function count_to_five() numeric
       numeric counter = 0
       
       while counter < 5
           counter = counter + 1  -- ❌ Bu satır parse edilmiyor!
       end while
       
       return counter
   end function
   ```
   
   ÜRETİLEN ASSEMBLY (ŞU AN):
   ```asm
   .while_start_0:
     mov rax, [rbp-8]     ; load counter
     cmp rax, 5
     jge .while_end_0     ; exit if >= 
     jmp .while_start_0   ; loop back (❌ BODY YOK!)
   .while_end_0:
   ```
   
   OLMASI GEREKEN:
   ```asm
   .while_start_0:
     mov rax, [rbp-8]     ; load counter
     cmp rax, 5
     jge .while_end_0     ; exit if >= 
     
     ; BODY BAŞLANGIÇ
     mov rax, [rbp-8]     ; load counter
     add rax, 1           ; + 1
     mov [rbp-8], rax     ; store counter
     ; BODY BİTİŞ
     
     jmp .while_start_0   ; loop back
   .while_end_0:
   ```

─────────────────────────────────────────────────
3. 🎯 SONRAKİ AI İÇİN GÖREV
─────────────────────────────────────────────────

GÖREV: While loop body statement parsing implement et

NEREDE:
- Dosya: melp/C/stage0/modules/functions/functions_standalone.c
- Fonksiyon: parse_function_body() içinde STMT_WHILE parsing kısmı
- Satır: ~365-415 arası

NE YAPILMALI:
1. While condition'dan sonra body statement'larını parse et
2. Her statement'ı ayrı ayrı STMT_WHILE içine embedded statement olarak ekle
3. Codegen'de STMT_WHILE içindeki embedded statement'ları iterate et
4. Her embedded statement için uygun assembly üret

ZORLUK SEVİYESİ: ⭐⭐⭐ (Orta-Zor)
- Nested statement parsing gerekiyor
- Recursive parsing yapılabilir veya inline parse edilebilir
- Statement boundary detection önemli (end while'ı yakalama)

ALTERNATIF KOLAY YOL:
While loop'u şimdilik atla, önce:
- Arrays + Functions entegrasyonu test et
- Array'leri function parametresi olarak geç
   
   TEST PROGRAMI:
   ```mlp
   function sum_array(numeric[] arr, numeric size) numeric
       numeric total = 0
       numeric i = 0
       -- while yerine for kullan veya manuel unroll
       return total
   end function
   ```

─────────────────────────────────────────────────
4. 🚨 KRİTİK KURALLAR (MUTLAKA OKU!)
─────────────────────────────────────────────────

⛔ **LEXER'A DOKUNMA!**
   Dosya: melp/C/stage0/lexer.c, melp/C/stage0/lexer.h
   Neden: Tüm modüller lexer'ı kullanıyor, bir değişiklik hepsini bozar
   Sorun varsa: Parser'da çöz, lexer'da değil!

⛔ **ÇALIŞAN MODÜLLERE DOKUNMA!**
   - print_standalone.c ✅
   - functions_standalone.c ✅ (sadece STMT_WHILE codegen eklenecek)
   - array_standalone.c ✅
   
⛔ **MLP SYNTAX KURALLARI:**
   - Yorum: `--` kullan (// YOK!)
   - Çoklu satır yorum: `--- ... ---`
   - Function syntax: function ... end function
   - While syntax: while ... end while
   - Array syntax: numeric[] arr = [1, 2, 3]

📖 **DOKÜMANTASYON:**
   - Syntax kuralları: user/kurallar_kitabı.md
   - Koleksiyon tipleri: Array [], List (), Tuple <>
   - TTO: Transparent Type Optimization
   
🧪 **TEST DOSYALARI:**
   - test_real_function.mlp → Functions test ✅
   - test_array_simple.mlp → Arrays test ✅
   - test_func_while.mlp → While test ⚠️ (body eksik)

─────────────────────────────────────────────────
5. 📊 PROJE DURUMU ÖZET
─────────────────────────────────────────────────

TAMAMLANAN MODÜLLER: 3/63 (%5)
- ✅ Print Module (strings, Turkish chars)
- ✅ Functions Module (params, locals, calls, return)
- ✅ Arrays Module (literals, initialization)

PARSER HAZIR: 1 modül
- ⚠️ While Loop (body codegen eksik)

TODo MODÜLLER:
- For Loop
- If-Else (control_flow modülünde var ama functions ile entegre değil)
- Structs
- Switch-Match
- Exception Handling
- ... (60 modül daha)

HEDEF: Stage 0 tamamlansın → Self-hosting başlasın

─────────────────────────────────────────────────
6. 🔗 YARDIMCI LİNKLER
─────────────────────────────────────────────────

GitHub Branch: mlp-true-syntax
Son Commit: de2b616 (05 Aralık 2025)
Snapshot Branch: MELP_Claude_Code_05_Aralik_2025

Test Komutları:
```bash
# Functions test
cd /home/pardus/projeler/MLP/MLP
./melp/C/stage0/modules/functions/functions_standalone test_real_function.mlp /tmp/test.s
nasm -f elf64 /tmp/test.s -o /tmp/test.o
ld /tmp/test.o -o /tmp/test
/tmp/test  # Output: 30

# Arrays test  
./melp/C/stage0/modules/array/array_standalone test_array_simple.mlp /tmp/test.s
nasm -f elf64 /tmp/test.s -o /tmp/test.o
ld /tmp/test.o -o /tmp/test
/tmp/test  # Output: Array Module OK!

# While test (body eksik)
./melp/C/stage0/modules/functions/functions_standalone test_func_while.mlp /tmp/test.s
# Parse OK, ama body boş
```

─────────────────────────────────────────────────

🎯 **SONRAKI AI'YE TAVSİYE:**

1. Önce bu dosyayı tamamen oku
2. CURRENT_STATUS.md'yi oku (hangi modüller çalışıyor)
3. user/kurallar_kitabı.md'yi oku (MLP syntax)
4. Test programlarını çalıştır (öğren)
5. Sonra while loop body parsing'e başla

YA DA kolay yoldan:
- While loop'u şimdilik atla
- Arrays + Functions integration'a odaklan
- Basit programlar yaz ve test et

İyi şanslar! 🚀

=================================================
   
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
