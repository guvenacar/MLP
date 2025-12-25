# 🧠 MELP Üst Akıl (Coordinator YZ) - Devir Belgesi

**Tarih:** 21 Aralık 2025  
**Hazırlayan:** YZ_ÜA_05  
**Amaç:** Gelecek Üst Akıl YZ agent'larına MELP projesi hakkında temel bilgi vermek

---

## 📌 Bu Belge Hakkında

Bu belge **Üst Akıl (Coordinator) YZ agent'ları** için hazırlanmıştır. Üst Akıl, implementation yapmaz, sadece **planlama**, **koordinasyon** ve **görevlendirme** yapar.

**Sen bir Üst Akıl isen:**
1. Bu belgeyi tamamen oku (5-10 dakika)
2. İlgili detaylı belgelere göz at (ihtiyaç oldukça)
3. Mevcut projeyi durumunu anla
4. Görevlendirme yap veya plan hazırla

**Sen görevli YZ isen:**
- Bu belgeyi okuma! Senin için `NEXT_AI_START_HERE.md` var
- Görev dosyalarını oku: `LLVM_YZ/YZ_XX_GOREV.md`

---

## 🎯 MELP Nedir? (5 Dakikada)

### Kısa Tanım

**MELP** = Multi-Language Programming = Eritme Potası 🫕

Çok dilli, modern, self-hosting bir programlama dili.

**Temel Özellikler:**
- **Basit tip sistemi:** `numeric`, `string`, `boolean` (kullanıcı sadece bunları görür)
- **STO (Smart Type Optimization):** Compiler arka planda optimize eder
- **PMPL syntax:** `end_if`, `else_if`, `end_while` (underscore ile!)
- **3 Stage:** Stage 0 (C), Stage 1 (Self-hosting), LLVM Backend
- **Çok dilli:** Türkçe, C style, Python style → hepsi PMPL'ye normalize edilir

### Temel Felsefe

1. **Basitlik:** Kullanıcı karmaşık tip sistemleriyle uğraşmaz
2. **Okunabilirlik:** Kod = düzyazı (`if...then...end if`)
3. **Stateless:** Fonksiyonlar yan etki yaratmaz (default)
4. **Performans:** Compiler optimize eder, kullanıcı düşünmez

**Örnek:**
```pmpl
numeric x = 42           -- Kullanıcı: sadece "numeric"
                         -- Compiler: int64 (register, çok hızlı)

numeric huge = 10 ^ 1000 -- Kullanıcı: yine "numeric"
                         -- Compiler: BigDecimal (heap, güvenli)
```

**Detay:** `MELP_VISION.md`, `ARCHITECTURE.md`

---

## 🏗️ Proje Mimarisi (3 Stage)

### Stage 0: C Compiler (Bootstrap)

**Durum:** ✅ TAMAMLANDI (ama YZ_06 bloker var!)

**Nedir:**
- C ile yazılmış compiler
- PMPL syntax'ını okur, LLVM IR veya Assembly üretir
- Stage 1'i derlemek için kullanılır

**Dizin:** `compiler/stage0/`

**Kısıtlar:**
- ⚠️ **YZ_06 BLOKER:** `()` list literal syntax yok!
- Sadece temel features (functions, if/while, struct)
- Basit tip sistemi
- Tam PMPL syntax desteği yok

**Test:**
```bash
./compiler/stage0/modules/functions/functions_compiler \
  test.mlp output.ll --backend=llvm
```

### Stage 1: Self-hosting Compiler

**Durum:** 🚨 **ENGELLENMIŞ** (66 modül derlenemiyor - YZ_06 bekleniyor)

**Nedir:**
- PMPL ile yazılmış compiler (self-hosting!)
- Stage 0 ile derlenir
- Stage 2 veya kendini derlemek için kullanılır

**Dizin:** `modules/`
- `lexer_mlp/`: 19 modül (token, lexer)
- `parser_mlp/`: 24 modül (AST, parser)
- `codegen_mlp/`: 19 modül (LLVM IR generator)
- `compiler.mlp`, `bootstrap_*.mlp`: 4 modül

**Problem:**
- Stage 1 modüller `returns list` + `()` syntax kullanıyor
- Stage 0 parser `()` list syntax'ını desteklemiyor
- **66 modül derlenemiyor (0 functions)!**

**Çözüm:** YZ_06 (Stage 0 List Syntax Support)

### Stage 2: Production Compiler

**Durum:** ⏳ Henüz başlamadı (Stage 1 bloker çözülmeli)

**Nedir:**
- Stage 1 compiler kendini derleyip Stage 2'yi üretir
- Optimize edilmiş, production-ready
- Son kullanıcılar bu compiler'ı kullanır

---

## 🔧 LLVM Backend

**Durum:** ✅ Faz 1 TAMAMLANDI (YZ_00-04)

**Özellikler:**
- Temel fonksiyon desteği (YZ_01)
- Control flow: if/while (YZ_02)
- Struct tanımlama ve kullanım (YZ_03)
- Stage 1 full compilation support (YZ_04)

**Opsiyonel Fazlar (YZ_05-13):**
- Array support, foreach, switch/case, string operations, etc.
- İhtiyaç oldukça yapılır

**Dizin:** `compiler/stage0/modules/codegen/llvm/`

**Test:**
```bash
./functions_compiler test.mlp output.ll --backend=llvm
lli output.ll  # LLVM interpreter ile çalıştır
```

**Detay:** `docs/LLVM_IR_GUIDE.md`, `LLVM_YZ/FAZ_1_TAMAMLANDI.md`

---

## 🚨 KRİTİK BLOKER: YZ_06

### Problem

**66 Stage 1 modül derlenemiyor!**

```bash
./functions_compiler modules/lexer_mlp/lexer.mlp temp/test.ll
# Sonuç: 0 functions compiled ❌
```

**Neden:**
- Stage 1 modüller şu syntax'ı kullanıyor:
  ```pmpl
  function create_token(...) returns list
    return (token_type; token_value; line_num; col_num;)  -- ✅ Doğru syntax
  end_function
  ```

- Stage 0 parser `()` list literal syntax'ını desteklemiyor!
- Test: `()` syntax kullanıldığında → **Segmentation Fault**

**Etki:**
- 580+ satır etkileniyor
- 25 dosya
- Tüm Stage 1 bootstrap engellenmiş!
- Self-hosting tamamlanamıyor!

### Çözüm: YZ_06 Görevi

**Görev:** Stage 0 List Syntax Support

**Öncelik:** 🔴🔴🔴 EN YÜKSEK - BLOKER

**Süre:** 1 hafta (21-28 Aralık 2025)

**Yapılacaklar:**
1. **Stage 0 Lexer:** `(` ve `)` tokenization + list literal detection
2. **Stage 0 Parser:** `(elem1; elem2;)` syntax parsing + `;` parametre ayırıcı
3. **Stage 0 Codegen:** List allocation + initialization

**Başarı Kriteri:**
```bash
./functions_compiler modules/lexer_mlp/token.mlp temp/test.ll
# Şu an: 0 functions ❌
# Hedef: 6 functions ✅
```

**Dosyalar:**
- `LLVM_YZ/YZ_06_GOREV.md` - Görevli YZ brifingi
- `temp/YZ_06_GOREV_OZET.md` - Detaylı analiz (150+ satır)
- `TODO_MODERN_LANGUAGE.md` Faz 0 - Genel bakış

**Referanslar:**
- `pmlp_kesin_sozdizimi.md` line 170, 640-642 (list syntax)
- `kurallar_kitabı.md` line 857-898 (collection types)

**⚠️ ÖNEMLİ:** YZ_06 tamamlanmadan Stage 1 bootstrap devam edemez!

---

## 📚 PMPL Syntax (Kritik Bilgi)

### Temel Kurallar

**1. Block Terminators (UNDERSCORE ile!):**
```pmpl
if condition then
    statement
end_if          -- ✅ UNDERSCORE!

while condition do
    statement
end_while       -- ✅ UNDERSCORE!

function name()
    statement
end_function    -- ✅ UNDERSCORE!
```

**❌ YANLIŞ:** `end if`, `end while` (boşluklu - kullanıcı kodu için)
**✅ DOĞRU:** `end_if`, `end_while` (underscore - compiler kodu için)

**2. Else If:**
```pmpl
if x > 10 then
    statement
else_if x > 5 then   -- ✅ UNDERSCORE!
    statement
else
    statement
end_if
```

**3. Collection Types:**

| Tip | Syntax | Ayırıcı | Örnek |
|-----|--------|---------|-------|
| Array | `[]` | `;` | `[1; 2; 3]` |
| List | `()` | `;` | `(1; "two"; 3.0;)` |
| Tuple | `<>` | `;` | `<10; 20>` |

**⚠️ ÖNEMLİ:**
- Parametre ayırıcı: `;` (noktalı virgül, virgül DEĞİL!)
- Trailing semicolon: Gerekli! `(1; 2; 3;)`
- Virgül `,` : Ondalık ayırıcı (Turkish format: `3,14`)

**Detay:** `pmlp_kesin_sozdizimi.md`, `kurallar_kitabı.md`

---

## 📋 TODO Sistemi

### TODO Dosyaları

1. **`TODO_MODERN_LANGUAGE.md`** - Modern dil özellikleri (YZ_06-224)
   - Faz 0: YZ_06 (1 hafta) - List syntax BLOKER
   - Faz 1: YZ_200-203 (4 hafta) - List/Array operations
   - Faz 2: YZ_204-208 (5 hafta) - Module system
   - Faz 3: YZ_209-215 (7 hafta) - Error handling
   - Faz 4: YZ_216-224 (9 hafta) - Advanced features

2. **`TODO_LLVM_SUMMARY.md`** - LLVM backend özet
   - Faz 1: ✅ TAMAMLANDI (YZ_00-04)
   - Faz 2-4: Opsiyonel features

3. **`LLVM_YZ/TODO.md`** - LLVM detaylı görev listesi
   - YZ_00-04: ✅ Completed
   - YZ_05-13: Pending (optional)

### Öncelik Matrisi

| Faz | YZ | Görev | Öncelik | Süre | Not |
|-----|-------|-------|---------|------|-----|
| 0 | YZ_06 | **Stage 0 List Syntax** | 🔴🔴🔴 **BLOKER** | 1 hafta | Stage 1 modüller derlenemiyor! |
| 1 | YZ_200 | List Operations | 🔴🔴 | 1 hafta | append, prepend, length |
| 1 | YZ_201 | Array Type Full | 🔴 | 1 hafta | indexing, slicing |
| 2 | YZ_204 | Module System | 🔴🔴 | 1 hafta | import/export |

**YZ_06 tamamlanmadan diğer görevlere başlanamaz!**

---

## 🔄 YZ Workflow (Üst Akıl İçin)

### Görev Tipine Göre Akış

#### 1. Planlama/Koordinasyon Görevi

**Sen şunları yaparsın:**
1. Mevcut durumu analiz et
2. TODO dosyalarını incele
3. Öncelik belirle
4. Görev brifingi hazırla
5. Görevli YZ için döküman oluştur
6. Git branch oluştur
7. Devir raporu yaz

**Örnek:** YZ_ÜA_05, YZ_ÜA_06

#### 2. Kritik Bloker Koordinasyonu

**Durum:** Acil görev var, görevli YZ'ye hazırlık yap

**Yapılacaklar:**
1. Bloker durumunu anla
2. Etki analizi yap (kaç dosya, kaç satır?)
3. Detaylı görev brifingi oluştur
4. Referans belgeleri topla
5. Test case'leri tasarla
6. Başarı kriterlerini belirle
7. Git branch + commit
8. NEXT_AI_START_HERE.md güncelle

**Örnek:** YZ_06 görevlendirmesi

#### 3. Devir Görevi

**Durum:** Sonraki Üst Akıl için belge hazırla

**Yapılacaklar:**
1. Mevcut projeyi özetle
2. Mimari bilgileri aktar
3. Kritik belgelere link ver
4. TODO durumunu göster
5. Öncelikli görevleri listele

**Örnek:** Bu belge!

---

## 📁 Kritik Dizinler ve Dosyalar

### Üst Akıl İçin Önemli Dosyalar

**1. Durumu Anlamak İçin:**
- `NEXT_AI_START_HERE.md` - Mevcut durum, son session
- `TODO_MODERN_LANGUAGE.md` - Modern dil TODO
- `TODO_LLVM_SUMMARY.md` - LLVM özet
- `ust_akil_YZ/YZ_ÜA_0X.md` - Önceki üst akıl raporları

**2. Mimari Bilgiler:**
- `ARCHITECTURE.md` - Mimari kurallar (1896 satır!)
- `MELP_VISION.md` - Proje vizyonu
- `kurallar_kitabı.md` - AI agent referansı (2030 satır!)
- `pmlp_kesin_sozdizimi.md` - PMPL syntax kuralları

**3. Stage 0 (C Compiler):**
- `compiler/stage0/modules/lexer/` - Lexer (tokenization)
- `compiler/stage0/modules/functions/functions_parser.c` - Parser
- `compiler/stage0/modules/codegen/` - Code generation

**4. Stage 1 (Self-hosting):**
- `modules/lexer_mlp/` - 19 modül (tokenization)
- `modules/parser_mlp/` - 24 modül (parsing, AST)
- `modules/codegen_mlp/` - 19 modül (LLVM codegen)

**5. LLVM Backend:**
- `LLVM_YZ/` - YZ görev raporları
- `docs/LLVM_IR_GUIDE.md` - LLVM IR kılavuzu
- `LLVM_YZ/FAZ_1_TAMAMLANDI.md` - Faz 1 özet

**6. Testler:**
- `tests/` - Test dosyaları
- `examples/` - Örnek programlar
- `temp/` - Geçici dosyalar, analizler

---

## 🎯 Görevlendirme Süreci

### Adım 1: Durumu Anla

```bash
# Mevcut branch
git branch --show-current

# Son commit
git log --oneline -5

# TODO durumu
cat TODO_MODERN_LANGUAGE.md | grep "🔴🔴🔴"
```

### Adım 2: Görev Belirle

**Soru listesi:**
- En yüksek öncelikli görev nedir?
- Hangi görev bloker durumda?
- Kaç dosya/satır etkileniyor?
- Süre tahmini nedir?
- Başarı kriterleri neler?

### Adım 3: Görev Brifingi Oluştur

**Şablon:** `LLVM_YZ/YZ_XX_GOREV.md`

```markdown
# YZ_XX: Görev Adı

**Öncelik:** 🔴🔴🔴/🔴🔴/🔴
**Süre:** X hafta
**Başlangıç:** Tarih

## Problem
[Detaylı problem açıklaması]

## Yapılacaklar
1. ...
2. ...

## Referanslar
- dosya1.md
- dosya2.c

## Test
```bash
# Test komutu
```

## Başarı Kriterleri
- [ ] Kriter 1
- [ ] Kriter 2
```

### Adım 4: Git Branch Oluştur

```bash
# Branch adı formatı: task-description_YZ_XX
git checkout -b stage0-list-syntax_YZ_06
```

### Adım 5: Commit ve Devir

```bash
git add LLVM_YZ/YZ_XX_GOREV.md ust_akil_YZ/YZ_ÜA_XX.md \
        TODO_MODERN_LANGUAGE.md NEXT_AI_START_HERE.md

git commit -m "YZ_ÜA_XX: Görev hazırlandı

- YZ_XX görev brifingi oluşturuldu
- TODO güncellendi
- Branch: task-name_YZ_XX

Status: Ready for assignment"

git push origin task-name_YZ_XX
```

### Adım 6: NEXT_AI_START_HERE.md Güncelle

**Güncellenmesi gerekenler:**
- Last Session
- Date
- Status
- Current Focus
- Next Action

---

## 💡 Önemli İpuçları

### 1. Bloker Durumları Önce

**Kural:** Bloker görevler (🔴🔴🔴) önceliklidir!

Örnek: YZ_06 Stage 0 list syntax bloker → 66 modül derlenemiyor → Stage 1 bootstrap engellenmiş!

### 2. Etki Analizi Yap

**Sorular:**
- Kaç dosya etkileniyor?
- Kaç satır değişecek?
- Hangi modüller bağımlı?
- Süre tahmini nedir?

### 3. Root Cause Fix

**İyi:** Stage 0'ı düzeltmek (fix once, use forever)
**Kötü:** 580+ satır manuel değişiklik (band-aid fix)

### 4. Detaylı Brief Hazırla

**İçerik:**
- Problem tanımı
- Etki analizi
- Syntax kuralları
- Gerekli değişiklikler
- Test case'leri
- Başarı kriterleri
- Referanslar
- Uyarılar

### 5. Timeline Realistik Olsun

**Süre tahminleri:**
- Lexer değişikliği: 1-2 gün
- Parser değişikliği: 2-3 gün
- Codegen değişikliği: 2-3 gün
- Test + debug: 1-2 gün
- Toplam: 1 hafta (buffer ile)

---

## 🔍 Sık Sorulan Sorular

### S1: Üst Akıl implementation yapabilir mi?

**Cevap:** HAYIR! Üst Akıl sadece planlama/koordinasyon yapar. Implementation görevli YZ yapar.

### S2: TODO dosyalarını kim günceller?

**Cevap:**
- Yeni görev eklemek: Üst Akıl
- Tamamlanma işareti: Görevli YZ
- Öncelik değişikliği: Üst Akıl

### S3: Git branch kimin sorumluluğunda?

**Cevap:**
- Branch oluşturma: Üst Akıl (koordinasyon için)
- Implementation commit: Görevli YZ
- Merge: İNSAN (manuel review)

### S4: YZ_06 neden YZ_199 değildi?

**Cevap:**
- LLVM_YZ dizininde son görev YZ_04 idi
- Sonraki görev YZ_05 olmalıydı
- Ancak array support YZ_05 olarak zaten planlanmıştı
- YZ_06 (list syntax) bloker olduğu için YZ_05'ten önce geldi
- İsimlendirme: dizin bazlı (LLVM_YZ/YZ_06_GOREV.md)

### S5: Stage 1 modüllerin syntax'ı yanlış mı?

**Cevap:**
- HAYIR! Stage 1 modüller DOĞRU syntax kullanıyor: `(elem;)`
- Problem Stage 0 parser'da: `()` list syntax'ını desteklemiyor
- Çözüm: Stage 0'a list syntax eklemek (YZ_06)

### S6: Modern language TODO'da 224 görev var, hepsi yapılacak mı?

**Cevap:**
- YZ_06: ZORUNLU (bloker)
- YZ_200-224: İhtiyaç oldukça
- LLVM YZ_05-13: Opsiyonel features
- Öncelik matrisi var: 🔴🔴🔴 > 🔴🔴 > 🔴

---

## 📊 Proje Durumu Özet (21 Aralık 2025)

### ✅ Tamamlanan

**Stage 0:**
- ✅ Temel syntax (functions, variables, if/while)
- ✅ LLVM backend entegrasyonu
- ✅ Struct desteği
- ⚠️ **EKSIK:** List literal syntax `()`

**LLVM Backend:**
- ✅ YZ_00-04: Faz 1 TAMAMLANDI (production ready!)

### 🚨 Kritik Bloker

**YZ_06: Stage 0 List Syntax Support**
- Öncelik: 🔴🔴🔴 EN YÜKSEK
- Süre: 1 hafta (21-28 Aralık 2025)
- Etki: 66 modül, 580+ satır
- Durum: Görevlendirme hazır

### ⏳ Bekleyen

**Stage 1 Bootstrap:**
- 66 modül var ama derlenemiyor (YZ_06 bekleniyor)
- YZ_06 sonrası: Bootstrap devam edecek

**Modern Language Features:**
- 25 görev (YZ_200-224)
- 19 hafta timeline
- 5 faz

---

## 🚀 Sonraki Üst Akıl İçin Notlar

### Hemen Bakılacaklar

1. **YZ_06 durumu nedir?**
   - Tamamlandı mı?
   - 66 modül derlenebiliyor mu?
   - `./functions_compiler modules/lexer_mlp/token.mlp` → 6 functions?

2. **Stage 1 bootstrap durumu?**
   - 66 modül compile oluyor mu?
   - Self-hosting tamamlandı mı?

3. **TODO güncel mi?**
   - YZ_06 [x] işaretlendi mi?
   - Öncelikler değişti mi?

### Muhtemel Senaryolar

**Senaryo 1: YZ_06 tamamlandı ✅**
- Stage 1 bootstrap devam etsin
- YZ_200 (List Operations) planlanabilir
- Modern language features başlayabilir

**Senaryo 2: YZ_06 devam ediyor ⏳**
- Bloker çözülmesini bekle
- Alternatif görev varsa (LLVM optional features) planla
- Ama öncelik YZ_06'da kalmalı

**Senaryo 3: YZ_06 problemli 🚨**
- Sorun analizi yap
- Alternatif çözüm öner
- Gerekirse görev yeniden tanımla

---

## 📚 Detaylı Belgeler (Referans)

### Mimari ve Felsefe
- `ARCHITECTURE.md` - Mimari kurallar (1896 satır)
- `MELP_VISION.md` - Proje vizyonu (189 satır)
- `MELP_REFERENCE.md` - Dil referansı

### Syntax ve Kurallar
- `kurallar_kitabı.md` - AI agent referansı (2030 satır)
- `pmlp_kesin_sozdizimi.md` - PMPL syntax kuralları
- `PMPL_SYNTAX.md` - Syntax özeti

### TODO ve Planlama
- `TODO_MODERN_LANGUAGE.md` - Modern dil TODO (25 görev)
- `TODO_LLVM_SUMMARY.md` - LLVM özet
- `LLVM_YZ/TODO.md` - LLVM detaylı TODO

### Stage 0 (C Compiler)
- `STAGE0_LIMITATIONS_DETAILED.md` - Stage 0 kısıtları
- `STAGE0_SIGNOFF.md` - Stage 0 onay belgesi
- `compiler/stage0/` - Kaynak kodlar

### Stage 1 (Self-hosting)
- `STAGE1_STATUS_REPORT.md` - Stage 1 durum raporu
- `modules/` - 66 modül
- `temp/MODÜL_ANALIZ_KARŞILAŞTIRMA.md` - Modül analizi

### LLVM Backend
- `docs/LLVM_IR_GUIDE.md` - LLVM IR kılavuzu
- `LLVM_YZ/FAZ_1_TAMAMLANDI.md` - Faz 1 özet
- `LLVM_YZ/YZ_00-04_*.md` - Görev raporları

### Üst Akıl Raporları
- `ust_akil_YZ/YZ_ÜA_01-06.md` - Önceki üst akıl raporları
- `ust_akil_YZ/YZ_UA_DEVIR_TEMPLATE.md` - Bu belge!

---

## ✅ Checklist (Yeni Üst Akıl İçin)

**İlk 10 dakika:**
- [ ] Bu belgeyi oku
- [ ] `NEXT_AI_START_HERE.md` oku
- [ ] Son üst akıl raporunu oku (`ust_akil_YZ/YZ_ÜA_0X.md`)
- [ ] TODO dosyalarını kontrol et
- [ ] Git durumu kontrol et: `git status`, `git log --oneline -5`

**Durum analizi:**
- [ ] YZ_06 durumu nedir?
- [ ] Stage 1 bootstrap durumu?
- [ ] Bloker görev var mı?
- [ ] Öncelikli görev hangisi?

**Görevlendirme (varsa):**
- [ ] Görev brifingi hazırla
- [ ] Git branch oluştur
- [ ] TODO güncelle
- [ ] NEXT_AI_START_HERE.md güncelle
- [ ] Devir raporu yaz
- [ ] Git commit + push

**Bitmeden önce:**
- [ ] Tüm değişiklikler commit edildi mi?
- [ ] NEXT_AI_START_HERE.md güncel mi?
- [ ] Sonraki YZ için notlar bırakıldı mı?

---

## 🎓 Son Notlar

### Üst Akıl'ın Görevleri

**✅ YAPACAĞIN:**
- Planlama
- Koordinasyon
- Görevlendirme
- Analiz
- Döküman hazırlama
- TODO güncellemesi
- Git branch oluşturma

**❌ YAPMAYACAĞIN:**
- Implementation
- Code yazma
- Parser/lexer değişikliği
- Test yazma (görev tanımında örnek olabilir)

### Başarı Kriterleri

**İyi Üst Akıl:**
- Durumu hızlı anlar (5-10 dakika)
- Öncelikleri doğru belirler
- Detaylı brief hazırlar
- Gerçekçi timeline verir
- Görevli YZ için hazır eder

**Kötü Üst Akıl:**
- Implementation yapmaya çalışır
- Bloker görevleri atlar
- Yetersiz brief bırakır
- TODO'yu güncellemez
- Git workflow'u takip etmez

---

**🎯 HEDEF:** Görevli YZ agent'lar için hazır zemin oluşturmak!

**🚀 BAŞARININ ANAHTARI:** Detaylı analiz + net brief + uygun önceliklendirme!

---

**Son Güncelleme:** 21 Aralık 2025, 15:00 (YZ_ÜA_05)  
**Sonraki Güncelleme:** YZ_06 tamamlandıktan sonra (YZ_ÜA_07?)  
**Durum:** YZ_06 bloker, görevlendirme hazır, görevli YZ bekleniyor
