// ============================================================================
// MELP Modular Codegen - All Modules
// ============================================================================
// Bu dosya tüm codegen modüllerini sırasıyla include eder.
// main.c bu dosyayı include etmelidir.
//
// MODÜL SIRASI ÖNEMLİ! Bağımlılıklara göre sıralanmıştır:
// 1. codegen_core.c      - Temel fonksiyonlar (create, emit, string)
// 2. codegen_module.c    - Modül takibi
// 3. codegen_operator.c  - Operatör overload kaydı
// 4. codegen_generic.c   - Generic fonksiyon yönetimi
// 5. codegen_variable.c  - Değişken yönetimi
// 6. codegen_type.c      - Enum, type alias
// 7. codegen_emit.c      - Emit helpers (codegen_core'a dahil)
// 8. codegen_function.c  - Fonksiyon bilgisi
// 9. codegen_struct.c    - Struct bilgisi
// 10. codegen_interface.c - Interface bilgisi
// 11. codegen_prologue.c - Prologue/epilogue
// 12. codegen_decl.c     - Declaration
// 13. codegen_assign.c   - Assignment
// 14. codegen_print.c    - Print statement
// 15. codegen_expr.c     - Expression (EN BÜYÜK ~1500 satır)
// 16. codegen_compare.c  - Comparison
// 17. codegen_if.c       - If/elseif/else
// 18. codegen_for.c      - For, for-in
// 19. codegen_while.c    - While, do-while
// 20. codegen_switch.c   - Switch/case
// 21. codegen_funcdef.c  - Function definition
// 22. codegen_return.c   - Return statement
// 23. codegen_trycatch.c - Try/catch/finally (YENİ)
// 24. codegen_throw.c    - Throw statement (YENİ)
// 25. codegen_match.c    - Pattern matching (YENİ)
// 26. codegen_state.c    - State management (YENİ)
// 27. codegen_lambda.c   - Lambda/closure (YENİ - expr'den çıkarılacak)
// 28. codegen_async.c    - Async/await (YENİ - expr'den çıkarılacak)
// 29. codegen_stmt.c     - Statement dispatcher
// 30. codegen_main.c     - Main generate function
//
// SON GÜNCELLEME: 1 Aralık 2025
// DURUM: Tüm modüller codegen.c'den çıkarıldı ve güncellendi
// ============================================================================

// NOT: parser.c main.c'den önce include edilmelidir
// codegen.h sadece parser.h'ı (type definitions) include eder

// Header include - sadece tip tanımları
// NOT: Her modül dosyasında #include "codegen.h" var
// Bu dosya modülleri birleştirirken include guard ile çakışma önlenir

// Temel modüller (1-10)
#include "codegen_core.c"
#include "codegen_emit.c"       // emit, add_string
#include "codegen_module.c"
#include "codegen_operator.c"
#include "codegen_type.c"       // Type alias + enum
#include "codegen_generic.c"    // Generic function management (Phase 13)
#include "codegen_variable.c"
#include "codegen_function.c"
#include "codegen_struct.c"
#include "codegen_interface.c"
// codegen_enum.c kaldırıldı - codegen_type.c içinde
#include "codegen_prologue.c"

// Declaration ve Assignment (11-14)
#include "codegen_decl.c"
#include "codegen_assign.c"
#include "codegen_print.c"

// Expression (15-16) - En büyük modül
#include "codegen_expr.c"
#include "codegen_compare.c"

// Control Flow (17-22)
#include "codegen_if.c"
#include "codegen_for.c"
#include "codegen_while.c"
#include "codegen_switch.c"
#include "codegen_funcdef.c"
#include "codegen_return.c"

// Gelişmiş özellikler (23-28) - HENÜZ OLUŞTURULMADI
// TODO: Bu modüller codegen_stmt.c'den çıkarılacak
// #include "codegen_trycatch.c"
// #include "codegen_throw.c"
// #include "codegen_match.c"
// #include "codegen_state.c"
// #include "codegen_lambda.c"
// #include "codegen_async.c"

// Ana dispatcher ve generate (29-30)
#include "codegen_stmt.c"
#include "codegen_main.c"

// ============================================================================
// SONRAKI AI İÇİN TALİMATLAR:
// ============================================================================
// 1. codegen_stmt.c içinden şunları AYRI dosyalara çıkar:
//    - STMT_TRY_CATCH → codegen_trycatch.c (~190 satır)
//    - STMT_THROW → codegen_throw.c (~35 satır)
//    - STMT_MATCH → codegen_match.c (~95 satır)
//    - STMT_STATE_DECL → codegen_state.c (~30 satır)
//
// 2. codegen_expr.c içinden şunları AYRI dosyalara çıkar:
//    - EXPR_LAMBDA → codegen_lambda.c (~165 satır)
//    - EXPR_AWAIT → codegen_async.c (~10 satır)
//
// 3. Her modül çıkarıldığında:
//    - Bu dosyadaki ilgili #include satırını aktif et
//    - codegen_stmt.c veya codegen_expr.c'den kodu SİL
//
// 4. Test: make clean && make ile derlenmeli
//
// 5. Self-hosting için selfhost/codegen/*.mlp dosyaları oluştur
// ============================================================================
