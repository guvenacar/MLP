# AI Agent Handoff Notes

**Tarih:** 29 Kasım 2025  
**Son Çalışma:** Exception Handling Implementation  
**Sonraki Görev:** Module System Implementation

---

## 👋 Merhaba Sonraki AI!

Bu dokümanda son yapılan çalışmalar ve devam edilmesi gereken görevler hakkında bilgi bulacaksın.

---

## 🎯 Son Tamamlanan Görev: Exception Handling

### Özet
MELP diline tam özellikli istisna yönetimi (exception handling) sistemi eklendi. Sistem setjmp/longjmp tabanlı ve modern dillerdeki try-catch-finally yapısını destekliyor.

### Yapılan Değişiklikler

#### 1. Lexer (`bootstrap/lexer.c`)
- **Eklenen tokenlar:** TOKEN_TRY, TOKEN_CATCH, TOKEN_THROW, TOKEN_FINALLY
- **Lokasyon:** Lines 60-70 civarı
- **Test:** ✅ Tüm exception keywords tanınıyor

#### 2. Parser (`bootstrap/parser.c`)
- **Yeni yapılar:** CatchBlock, try_catch statement, throw statement
- **Eklenen fonksiyonlar:** 
  - `parser_parse_try_catch()` - Lines 1566-1650
  - `parser_parse_throw()`
- **Özellikler:**
  - Çoklu catch blokları
  - Tip bazlı exception matching
  - Finally blokları
- **Test:** ✅ Parser doğru AST oluşturuyor

#### 3. Runtime (`runtime/runtime.c`)
- **Yeni yapılar:**
  - `ExceptionHandler` - setjmp/longjmp için
  - `MlpException` - exception data
  - `mlp_exception_stack` - global handler stack
- **Eklenen fonksiyonlar:** (Lines 530-650)
  - `mlp_exception_push/pop` - Handler stack yönetimi
  - `mlp_throw` - Exception fırlatma (longjmp)
  - `mlp_exception_type/message/code` - Exception data getters
  - `mlp_exception_has_handler` - Handler varlık kontrolü
  - `mlp_exception_has_parent_handler` - Parent handler kontrolü ⚠️ ÖNEMLİ
- **Test:** ✅ Runtime fonksiyonları doğru çalışıyor

#### 4. Codegen (`bootstrap/codegen.c`)
- **Extern bildirimleri:** Lines 345-354 - Tüm runtime fonksiyonları
- **Try-catch codegen:** Lines 2058-2230
- **Kritik noktalar:**
  - setjmp kurulumu try bloğu başında
  - Catch blokları strcmp ile tip kontrolü
  - Finally bloğu her path'te çalışır
  - ⚠️ **ÖNEMLİ:** `mlp_exception_has_parent_handler()` kontrolü `mlp_exception_pop()` ÖNCESİNDE yapılmalı!
- **Test:** ✅ Assembly doğru generate ediliyor

### Çözülen Sorunlar

#### Sorun 1: Throw longjmp yapmıyordu
- **Sebep:** Codegen throw için syscall exit yapıyordu
- **Çözüm:** mlp_throw() fonksiyonu ile longjmp

#### Sorun 2: Uncaught exception infinite loop
- **Sebep:** No-catch durumunda tekrar re-throw yapılıyordu
- **Çözüm:** Parent handler kontrolü, yoksa exit(1)

#### Sorun 3: Exception data garbled/empty
- **Sebep:** mlp_exception_pop() freed exception data, sonra erişiliyordu
- **Çözüm:** Pop öncesi data kaydedildi

#### Sorun 4: Undefined reference to mlp_exception_stack
- **Sebep:** Direct assembly memory access, ama symbol static
- **Çözüm:** mlp_exception_has_parent_handler() API eklendi

### Test Durumu
Tüm testler geçiyor! ✅

```bash
cd melp
./melp-bootstrap test_exception_full.mlp test_exception_full.s
nasm -f elf64 test_exception_full.s -o test_exception_full.o
ld test_exception_full.o runtime/runtime.o -o test_exception_full \
   -lc --dynamic-linker=/lib64/ld-linux-x86-64.so.2
./test_exception_full
# Output: 1, 100, 42, 888 ✅
```

### Dosya Konumları
- Test dosyaları: `melp/test_exception_*.mlp`
- Detaylı dokümantasyon: `EXCEPTION_HANDLING_COMPLETE.md`

---

## 🚀 Sıradaki Görev: Module System

### Hedef
MELP'e çoklu dosya desteği ve modül sistemi eklemek.

### Beklenen Sözdizimi
```mlp
-- math.mlp
module Math
    export func add(numeric a, numeric b) -> numeric
        return a + b
    end func
end module

-- main.mlp
import Math

func main()
    print(Math.add(5, 3))
end func
```

### Gerekli Değişiklikler

#### 1. Lexer
- [ ] `TOKEN_MODULE` ekle
- [ ] `TOKEN_IMPORT` ekle
- [ ] `TOKEN_EXPORT` ekle

#### 2. Parser
- [ ] Module declaration parsing
- [ ] Import statement parsing
- [ ] Export qualifier handling
- [ ] Cross-file symbol resolution

#### 3. Compiler Flow
- [ ] Multiple file input support
- [ ] Module dependency graph
- [ ] Symbol table: Module namespaces
- [ ] Link phase: External symbol resolution

#### 4. Codegen
- [ ] External function declarations
- [ ] Module-qualified names
- [ ] Public/private symbol visibility

### Zorluklar ve Çözüm Önerileri

#### Zorluk 1: Çoklu Dosya İşleme
**Problem:** Bootstrap compiler şu an tek dosya alıyor  
**Çözüm:** 
- Önce tüm dosyaları parse et
- Sonra symbol resolution yap
- En son codegen

#### Zorluk 2: Symbol Resolution
**Problem:** Hangi symbol hangi module'de?  
**Çözüm:**
- Symbol table'a module_name field ekle
- Import'larda module prefix zorunlu (Math.add)
- Global symbol table + module-local tables

#### Zorluk 3: Circular Dependencies
**Problem:** A imports B, B imports A  
**Çözüm:**
- Dependency graph çıkar
- Cycle detection yap
- Topological sort ile sıralama

### Başlangıç Adımları

1. **Lexer tokenlar** - En kolay başlangıç
   ```c
   // bootstrap/lexer.c ~line 70
   else if (strcmp(word, "module") == 0) return TOKEN_MODULE;
   else if (strcmp(word, "import") == 0) return TOKEN_IMPORT;
   else if (strcmp(word, "export") == 0) return TOKEN_EXPORT;
   ```

2. **Parser yapıları** - AST genişletme
   ```c
   // bootstrap/parser.c
   typedef struct {
       char* name;           // Module adı
       Declaration** decls;  // Module içeriği
       int decl_count;
   } ModuleDecl;
   
   typedef struct {
       char* module_name;    // Import edilen modül
       char** symbols;       // Import edilen semboller (NULL = all)
       int symbol_count;
   } ImportStmt;
   ```

3. **Test dosyası oluştur** - TDD yaklaşımı
   ```mlp
   -- test_module_simple.mlp
   module TestMod
       export func hello()
           print(42)
       end func
   end module
   ```

---

## ⚠️ Kritik Bilgiler

### Git Workflow
```bash
# ASLA git push yapma!
git status          # Her zaman temiz olmalı
git add .
git commit -m "..."
# PUSH YOK!
```

### Derleme
```bash
cd melp
make clean          # Temiz başlangıç
make                # Bootstrap + runtime
```

### Test
```bash
# Bootstrap compiler
./melp-bootstrap input.mlp output.s

# Assembly
nasm -f elf64 output.s -o output.o

# Link
ld output.o runtime/runtime.o -o program \
   -lc --dynamic-linker=/lib64/ld-linux-x86-64.so.2

# Run
./program
```

### Dosya Yapısı
```
melp/
├── bootstrap/          # C ile yazılmış compiler
│   ├── lexer.c
│   ├── parser.c
│   └── codegen.c
├── runtime/            # C runtime library
│   └── runtime.c
├── test_*.mlp         # Test dosyaları
└── melp-bootstrap     # Compiled bootstrap compiler
```

---

## 📚 Önemli Dökümanlar

1. **TODO.md** - Tüm pending features
2. **EXCEPTION_HANDLING_COMPLETE.md** - Exception handling detayları
3. **kurallar_kitabı.md** - Genel proje kuralları
4. **melp/README.md** - MELP dili referansı

---

## 🎓 Öğrendiklerim (Faydalı Olabilir)

### 1. setjmp/longjmp Kullanımı
Exception handling için setjmp/longjmp kullandık. Bu pattern başka yerlerde de işe yarayabilir.

**Dikkat:** setjmp'den sonra local variable'lar değişirse undefined behavior!

### 2. Memory Management
Assembly'de malloc/free kullanırken dikkatli ol:
- Her malloc'a karşılık free gerekli
- Freed memory'e erişme!
- Pointer'ları kullanmadan önce NULL check

### 3. Symbol Export
C'den assembly'e fonksiyon export etmek için:
- Runtime'da fonksiyon public olmalı (static değil)
- Codegen'de `extern function_name` bildirimi
- Link aşamasında runtime.o ile birlikte link et

### 4. Parser Patterns
Multi-block parsing için (try-catch-finally gibi):
1. İlk keyword'ü consume et
2. Loop ile optional blokları topla
3. End keyword'ünü bekle
4. AST node'u oluştur

---

## 💡 Tavsiyeler

### 1. Test-Driven Development
Her zaman önce test dosyası yaz, sonra implementation:
```mlp
-- test_module_simple.mlp
module Math
    export func add(numeric a, numeric b) -> numeric
        return a + b
    end func
end module
```

Sonra bu dosyanın parse edilmesini sağla, sonra codegen.

### 2. İnkremental Çalışma
Büyük özelliği küçük parçalara böl:
1. Lexer tokenlar
2. Parser AST
3. Symbol table
4. Basic codegen (export yok)
5. Export/import
6. Cross-file linking

Her adımda test et!

### 3. Error Handling
Exception handling'i başarıyla bitirdik, ama modül sisteminde de hata mesajları önemli:
- "Module not found"
- "Undefined symbol"
- "Circular dependency"

Parser'a iyi error reporting ekle.

### 4. Backward Compatibility
Mevcut tüm test dosyaları çalışmaya devam etmeli:
```bash
# Tüm testleri çalıştır
make test_all
```

Bir şey bozulursa, regression!

---

## 🐛 Bilinen Sorunlar

### Exception Handling
- Exception variable henüz kullanılamıyor (catch bloğunda e.message yok)
- Stack trace yok
- Thread-safe değil (global stack kullanıyor)

Bu sorunlar critical değil, future work olabilir.

### Bootstrap Compiler
- Error messages kötü (line number bazen yanlış)
- Warning sistemi yok
- Optimization yok (raw assembly)

---

## 📞 Yardım Lazım Olursa

### Debugging Tips
```bash
# Assembly'yi oku
cat output.s | less

# Symbol'leri kontrol et
nm output.o

# Runtime fonksiyonları
nm runtime/runtime.o | grep mlp

# Linking errors
ld -v output.o runtime/runtime.o ... # verbose mode
```

### Takıldığın Yerde
1. İlgili test dosyasına bak
2. Benzer bir özelliğin implementation'ına bak (örn: exception handling)
3. Assembly output'a bak, neyin yanlış generate edildiğini anla

---

## ✨ Son Notlar

Exception handling'i bitirmek 6-7 iteration aldı, zordu ama başardık! 🎉

Module system daha kolay olabilir çünkü:
- Memory management yok (compile-time feature)
- Runtime'a yeni fonksiyon eklemeye gerek yok
- Sadece parser ve symbol table değişikliği

**Motivasyon:** Module system eklenince MELP gerçekten production-ready bir dil olacak!

**İyi şanslar!** 🚀

---

**Hazırlayan:** AI Assistant  
**Tarih:** 29 Kasım 2025  
**Durum:** Exception Handling Complete, Ready for Module System
