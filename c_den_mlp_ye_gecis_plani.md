# C'den MLP'ye Geçiş Planı

Bu belge, `self_host/mlp_compiler.c` dosyasındaki bootstrap derleyicisini
modüler MLP kaynaklarına taşıma adımlarını özetler. Hedef, self-hosting
zincirini tamamen MLP koduyla sürdürülebilir hale getirmektir.

---

## 1. Mevcut C Dosyasındaki Ana Modüller

1. **Function Registry**
   - `FunctionSignature`, `ParameterInfo` yapıları, HashMap tabanlı kayıt
   - `pre_scan_functions`, default parametre desteği

2. **Lexer (`c_lexer`)**
   - Token enum'u, `Token` struct'ı, `initLexer`, `getNextToken`,
     `peekNextToken`, `check_keyword`
   - UTF-8, string interpolation, triple quote, line continuation

3. **AST Tanımları (`c_ast`)**
   - `ASTNodeType` enum'u ve tüm düğüm veri yapıları (literal, kontrol,
     struct/list/map/optional, lambda/async, argv)

4. **Parser**
   - Pratt tabanlı ifade çözümü, `komut`, `blok`, `islec_tanimlama`,
     `async_function_tanimlama`, `import`
   - Default parametre kaydı, range/for-each, lambda, await

5. **Scope / Sembol Yönetimi**
   - `kapsam_gir/cik`, `kapsam_bul`, tip takibi ve auto-free listesi

6. **Codegen Çekirdeği**
   - `AsmSection`, `asm_append`, `visit_*` ziyaretçileri
   - Struct/list/map/optional işlemleri, lambda/closure, async await

7. **Import Sistemi**
   - `visit_Import`, döngü tespiti, lexer/parser durumunun yedeklenmesi

8. **Async/Await State Machine**
   - `count_awaits_in_node`, `emit_async_state_machine`, promise çağrıları

9. **Pipeline & CLI**
   - `compile_to_asm`, `write_to_file`, NASM+GCC çağrıları, CLI argümanları

10. **C Backend (opsiyonel)**
    - AST → C kodu üretimi (`c_backend` prototipi)

---

## 2. MLP Tarafında Önerilen Modüler Yapı

1. **Temel Veri Yapıları**
   - `token.mlp`: TokenType enum, Token kaydı, TokenStream yardımcıları
   - `ast/*.mlp`: AST düğümleri (ifade, ifade, veri yapıları, async)
   - `function_registry.mlp`: Signature saklama, `pre_scan_functions`

2. **Lexer (`lexer.mlp`)**
   - `init_lexer`, `next_token`, `peek_token`, `classify_keyword`
   - İngilizce IR üzerinde çalışır; UTF-8 ve interpolation kurallarını taşır

3. **Parser Modülleri**
   - `parser/pratt.mlp`: ifade çözümü
   - `parser/statements.mlp`: blok, if/while/for, async, import
   - `parser/desugar.mlp`: range/for-each, default parametre işleme

4. **Scope & Sembol Yardımcıları** (`scope.mlp`, `symbol_table.mlp`)
   - Değişken ofseti, tip bilgisi, auto-free listesi
   - Closure yakalama analizi için yardımcılar

5. **Code Generator Paketi**
   - `asm/emitter.mlp`: `AsmSection`, `lambda_section`, final birleşim
   - `generator/expressions.mlp`
   - `generator/control.mlp`
   - `generator/data_structures.mlp`
   - `generator/functional.mlp` (lambda & closure)
   - `generator/async.mlp` (state machine)
   - `generator/import.mlp`

6. **Import Yönetimi (`importer.mlp`)**
   - Dosya okuma, döngü tespiti, lexer/parser durum yedekleme

7. **Async Yardımcıları**
   - `async/state_machine.mlp`: await sayımı ve resume noktaları
   - `async/runtime_hooks.mlp`: promise API sembolleri

8. **CLI & Pipeline (`mlpc.mlp`)**
   - Argüman parse, preprocessor çağrısı, lexer→parser→codegen zinciri
   - `--emit-c`, `--run`, `--lang` desteği, NASM/GCC entegrasyonu

9. **C Backend (opsiyonel)**
   - `backends/c_backend.mlp`: AST → C emisyonu (gelecekteki iş)

---

## 3. Bağımlılık Haritası

- Lexer → Token + keyword tablosu
- Parser → Lexer + AST + Function Registry + Scope helper
- Function Registry → AST + HashMap runtime
- Scope Helpers → Parser + Code Generator
- Code Generator → AST + Scope + Function Registry + Async helpers + Importer
- Importer → Lexer + Parser + Code Generator
- CLI → Preprocessor + tüm pipeline + dosya/komut yürütme

---

## 4. Önerilen Port Sıralaması

1. **Temel veri yapıları:** Token, AST, Function Registry
2. **Lexer** (MLP'de birebir port)
3. **Parser** (ifade + statement modülleri)
4. **Scope/Sembol araçları**
5. **Codegen çekirdeği:** AsmSection, literal ziyaretçileri
6. **Kontrol & veri yapısı ziyaretçileri**
7. **Gelişmiş özellikler:** lambda/closure, optional, async/await
8. **Import sistemi & CLI**
9. **C backend ve diğer opsiyoneller**

Bu sıralama ile her adım bağımsız test edilebilir olacak ve C kaynaklı
karmaşıklık azaltılarak tamamen MLP tabanlı self-host pipeline'a geçiş
kolaylaşacaktır.
