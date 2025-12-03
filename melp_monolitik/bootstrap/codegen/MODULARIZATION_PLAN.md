# MELP Codegen Modülarization Plan
# SON GÜNCELLEME: 1 Aralık 2025

## MEVCUT DURUM

Ana monolitik dosya: `bootstrap/codegen.c` (~5015 satır)
- Çalışıyor, derleniyor
- Yorum satırlarıyla modül sınırları işaretlendi
- Self-hosting için çok büyük

Modüler dosyalar: `bootstrap/codegen/` klasörü
- 24 adet .c dosyası mevcut
- GÜNCEL DEĞİL - ana codegen.c ile uyuşmuyor
- `codegen_all.c` oluşturuldu ama çalışmıyor

## SORUN

1. Mevcut modüler dosyalar ESKİ kodla yazılmış
2. Ana codegen.c'deki fonksiyonlar değişmiş/eklenmiş
3. Forward declaration'lar eksik
4. Bazı yardımcı fonksiyonlar (is_double_expression, is_string_expression, is_builtin_function) modüllere dağıtılmamış

## ÇÖZÜM PLANI

### Adım 1: Ana codegen.c'den Kod Çıkarma (Her AI bir bölüm yapabilir)

Her modül için:
1. Ana `codegen.c`'de ilgili `// MODÜL:` yorumlarını bul
2. O bölümdeki kodu kopyala
3. `bootstrap/codegen/<modül>.c` dosyasını güncelle
4. `#include "codegen.h"` ekle
5. Gerekli forward declaration'ları ekle

### Adım 2: codegen_all.c Güncellemesi

Tüm modüller güncellenince:
1. `codegen_all.c`'yi aktif et
2. `main.c`'de `#include "codegen.c"` yerine `#include "codegen/codegen_all.c"` yap
3. Test et

### Adım 3: codegen.c Silme

Tüm testler geçince eski monolitik dosyayı sil.

## MODÜL HARİTASI (codegen.c satır numaraları)

| Satır Aralığı | Modül Dosyası | İçerik |
|---------------|---------------|--------|
| 45-186 | codegen.h | Struct tanımları (ZATEN VAR) |
| 190-239 | codegen_core.c | codegen_create, emit, add_string, free |
| 243-266 | codegen_module.c | module_seen, mark_module_seen |
| 270-299 | codegen_operator.c | add/find_operator_overload |
| 582-639 | codegen_type.c | enum_value, type_alias + generic funcs |
| 391-578 | codegen_variable.c | Değişken yönetimi (~190 satır) |
| 668-696 | codegen_function.c | add/find_function |
| 700-760 | codegen_struct.c | add/find_struct |
| 764-796 | codegen_interface.c | add/find_interface |
| 800-974 | codegen_prologue.c | prologue, epilogue, is_builtin_function |
| 978-1279 | codegen_decl.c | declaration (~300 satır) |
| 1283-1422 | codegen_assign.c | assignment (~140 satır) |
| 1426-1531 | codegen_print.c | print + is_string_expression (~105 satır) |
| 1535-3002 | codegen_expr.c | expression_value (~1470 satır) - EN BÜYÜK |
| 3006-3153 | codegen_compare.c | comparison (~150 satır) |
| 3157-3217 | codegen_if.c | if-elseif-else (~60 satır) |
| 3221-3485 | codegen_for.c | for, for-in (~265 satır) |
| 3489-3606 | codegen_while.c | while, do-while (~120 satır) |
| 3610-3772 | codegen_switch.c | switch-case (~160 satır) |
| 3776-3930 | codegen_funcdef.c | func_def (~155 satır) |
| 3934-4018 | codegen_return.c | return (~85 satır) |
| 4022-4771 | codegen_stmt.c | statement dispatcher (~750 satır) |
| 4775-5011 | codegen_main.c | generate, free (~240 satır) |

## YENİ MODÜLLER (codegen_stmt.c'den çıkarılacak)

| Satır (stmt içinde) | Yeni Modül | İçerik |
|---------------------|------------|--------|
| STMT_TRY_CATCH | codegen_trycatch.c | ~190 satır |
| STMT_THROW | codegen_throw.c | ~35 satır |
| STMT_MATCH | codegen_match.c | ~95 satır |
| STMT_STATE_DECL | codegen_state.c | ~30 satır |
| STMT_DEBUG_* | codegen_debug.c | ~45 satır |
| STMT_YIELD | codegen_yield.c | ~30 satır |
| STMT_OPERATOR_DEF | codegen_opdef.c | ~105 satır |
| STMT_MODULE_DEF | codegen_moddef.c | ~35 satır |

## YENİ MODÜLLER (codegen_expr.c'den çıkarılacak)

| Satır (expr içinde) | Yeni Modül | İçerik |
|---------------------|------------|--------|
| EXPR_LAMBDA | codegen_lambda.c | ~165 satır |
| EXPR_AWAIT | codegen_async.c | ~10 satır |

## ÖNCELİK SIRASI

1. **codegen_core.c** - Tüm modüllerin bağımlı olduğu temel fonksiyonlar
2. **codegen_variable.c** - Değişken yönetimi (çok kullanılıyor)
3. **codegen_prologue.c** - is_builtin_function dahil
4. **codegen_expr.c** - En büyük ve en karmaşık modül
5. **codegen_stmt.c** - İkinci en büyük modül
6. Diğerleri...

## TALİMATLAR (Sonraki AI için)

```
1. Bu dosyayı oku: MODULARIZATION_PLAN.md
2. Bir modül seç (henüz güncellenmemiş olanlardan)
3. Ana codegen.c'den ilgili kodu bul (satır numaraları yukarıda)
4. bootstrap/codegen/<modül>.c dosyasını güncelle
5. Test: gcc -c -Wall -fsyntax-only bootstrap/codegen/<modül>.c
6. Bu dosyayı güncelle (hangi modülü yaptığını işaretle)
7. Commit yap
```

## TAMAMLANAN MODÜLLER

- [x] codegen.h - Header dosyası (güncel)
- [ ] codegen_core.c - Temel fonksiyonlar
- [ ] codegen_module.c - Modül takibi
- [ ] codegen_operator.c - Operator overload
- [ ] codegen_type.c - Type alias + generic
- [ ] codegen_variable.c - Değişken yönetimi
- [ ] codegen_function.c - Fonksiyon tablosu
- [ ] codegen_struct.c - Struct yönetimi
- [ ] codegen_interface.c - Interface yönetimi
- [ ] codegen_enum.c - Enum yönetimi
- [ ] codegen_prologue.c - Prologue/epilogue
- [ ] codegen_decl.c - Declaration
- [ ] codegen_assign.c - Assignment
- [ ] codegen_print.c - Print
- [ ] codegen_expr.c - Expression (EN BÜYÜK)
- [ ] codegen_compare.c - Comparison
- [ ] codegen_if.c - If/else
- [ ] codegen_for.c - For loops
- [ ] codegen_while.c - While loops
- [ ] codegen_switch.c - Switch/case
- [ ] codegen_funcdef.c - Function definition
- [ ] codegen_return.c - Return statement
- [ ] codegen_stmt.c - Statement dispatcher
- [ ] codegen_main.c - Main generate

## HEDEF

Self-hosting için:
1. Her modül ~200-300 satır (yönetilebilir boyut)
2. Modüler yapı MLP'ye çevrilebilir
3. AI'lar tek bir modüle odaklanabilir
