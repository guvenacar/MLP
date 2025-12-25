# 🎯 YENİ YZ İÇİN BAŞLANGIÇ NOKTASI

**Son Güncelleme:** 25 Aralık 2025  
**Proje:** mlp-original (guvenacar/mlp)  
**Aktif TODO:** STAGE0_PMLP - Stage0-C Compiler PMLP Syntax Desteği

---

## 🎯 SEN KİMSİN?

Sen bir **Görevli YZ** (Yapay Zeka) - TODO'daki belirli bir görevi tamamlamakla sorumlusun.

**Senin YZ numaranı Üst Akıl belirler.** (Örn: STAGE0_YZ_02)

---

## 📋 İLK OKUMALAR (ZORUNLU!)

**Sırayla oku:**

1. 📖 [`TODO_KURALLARI.md`](TODO_KURALLARI.md)
   - Tüm YZ'lerin uyması gereken GENEL kurallar
   - Kendini tanıtma protokolü
   - Git workflow
   - Yasaklar ve yapılacaklar

2. 📖 [`1-TODO_STAGE0_PMLP_SYNTAX.md`](1-TODO_STAGE0_PMLP_SYNTAX.md)
   - Bu TODO'nun detaylı görev listesi
   - Phase'ler ve Task'lar
   - Senin görevin burada tanımlı

3. 📖 [`TODO_STAGE0_PMLP/docs/pmlp_sozdizimi.md`](TODO_STAGE0_PMLP/docs/pmlp_sozdizimi.md)
   - PMLP syntax referansı (1138 satır)
   - **TÜMÜNÜ OKUMA!** Sadece görevinle ilgili bölümü oku
   - Index'i kullan

---

## 🎤 GÖREVE BAŞLAMADAN ÖNCE: KENDİNİ TANIT (ZORUNLU!)

**Belgeleri okuduktan sonra, göreve başlamadan ÖNCE şu formatı kullanarak kendini tanıt:**

```markdown
🤖 [YZ_NUMARAN] GÖREVE HAZIRIM

**Kimlik:** STAGE0_YZ_0X
**Phase:** Phase X - [Phase Adı]
**Görev:** [Kısa görev açıklaması]
**Dosya:** [Hangi dosyalarda çalışacaksın]
**Süre:** [Tahmini süre]

---

## ✅ OKUDUKLARIM:

- ✅ TODO_KURALLARI.md - Genel YZ kuralları
- ✅ NEXT_AI_START_HERE.md - Bu dosya
- ✅ 1-TODO_STAGE0_PMLP_SYNTAX.md - TODO detayları
- ✅ pmlp_sozdizimi.md (Bölüm X) - İhtiyacım olan syntax

---

## 📐 KULLANACAĞIM SYNTAX KURALLARI:

**pmlp_sozdizimi.md'den öğrendiklerim:**

1. **[Kural 1]:** [Açıklama]
   ```mlp
   [Örnek kod]
   ```

2. **[Kural 2]:** [Açıklama]
   ```mlp
   [Örnek kod]
   ```

(Sadece görevinle ilgili syntax kurallarını listele!)

---

## 💪 TAAHHÜTLER:

- ✅ Pseudo/stub/hack yazmayacağım - Her şey gerçek, çalışır kod
- ✅ Her task sonunda test yapacağım
- ✅ %100 çalışır kod üreteceğim
- ✅ Git workflow'a uyacağım
- ✅ Test başarısız olursa düzeltip tekrar deneyeceğim (3 deneme)
- ✅ Merge yapmayacağım

**⚠️ GÖREV BİTİŞİNDE MUTLAKA YAPACAĞIM:**
- ✅ Ana TODO'da benim bölümümü güncelleyeceğim (`1-TODO_STAGE0_PMLP_SYNTAX.md` - SADECE kendi Phase/Task checkbox'larım)
- ✅ Ana TODO'da kendi bölümümün sonuna test sonuçlarını ekleyeceğim (hangi testler, sonuçlar, nasıl doğruladım)
- ✅ NEXT_AI'ı güncelleyeceğim (Phase durumu: ⏳ → ✅, sonraki YZ bilgisi)
- ✅ Rapor yazacağım (`TODO_STAGE0_PMLP/STAGE0_YZ/completed/STAGE0_YZ_0X_TAMAMLANDI.md` - testler + sonuçlar + istatistikler)
- ✅ Git push yapacağım (merge yapmadan)

---

## 🚀 ONAY BEKLİYORUM

Üst Akıl, onay verirsen Task X.1'den başlıyorum.
```

---

## 📍 ŞU AN HANGİ PHASE'TEYİZ?

```
✅ Phase 1: Exit/Continue Tokenları - TAMAMLANDI (STAGE0_YZ_01) ✅
   └─> 10/10 test geçti
   └─> Token enum, keyword table, parser, codegen tamamlandı
   └─> Assembly çıktısı doğrulandı (jmp instructions)

⏳ Phase 2: Semicolon Separator - ŞİMDİ (2-3h) ⚠️ KRİTİK!
   └─> STAGE0_YZ_02 atanacak

⏸️ Phase 3: Test & Migration - BEKLEMEDE (1-2h)
```

**Toplam:** 4-7 saat, 3 Phase, 3 YZ

---

## ✅ PHASE 1 TAMAMLANDI - STAGE0_YZ_01 RAPORU

**Tamamlanan İşler:**
- ✅ Task 1.1: Token Enum'a 8 yeni token eklendi
- ✅ Task 1.2: Keyword Table'a 8 yeni keyword eklendi
- ✅ Task 1.3: Parser'da exit/continue variant'ları handle ediliyor
- ✅ Task 1.4: Codegen - jmp instructions doğru üretiliyor
- ✅ Task 1.5: Test - 10/10 test başarılı ✅

**Test Dosyaları:**
- `tests/pmlp/test_exit_simple.mlp` - exit_while ✅
- `tests/pmlp/test_exit_for.mlp` - exit_for ✅
- `tests/pmlp/test_generic_exit.mlp` - exit (generic) ✅
- `tests/pmlp/test_continue_simple.mlp` - continue_while ✅
- `tests/pmlp/test_continue_for.mlp` - continue_for ✅
- `tests/pmlp/test_exit_function.mlp` - exit_function ✅
- `tests/pmlp/test_comprehensive.mlp` - tüm variant'lar ✅
- `tests/pmlp/test_regression.mlp` - eski syntax hala çalışıyor ✅
- `tests/pmlp/test_exit.mlp` - exit_while ✅
- `tests/pmlp/test_continue.mlp` - continue_while ✅

**Assembly Doğrulama:**
- `exit_while` → `jmp .L1` (loop end) ✅
- `exit_for` → `jmp .L2` (loop end) ✅
- `continue_while` → `jmp .L0` (loop start) ✅
- `continue_for` → `jmp .L1` (loop start) ✅
- `exit_function` → `leave; ret` ✅

**Compiler Durumu:**
- Recompile başarılı ✅
- Eski syntax hala çalışıyor (geriye dönük uyumluluk) ✅
- Hiç pseudo/stub kod yok ✅

---

## 🟡 ŞİMDİKİ GÖREV: PHASE 2 (STAGE0_YZ_02)

### Phase 2: Semicolon Separator (2-3 saat) ⚠️ EN ZORLU!

**⚠️ KRİTİK:** Bu phase tüm parametre/list/array parsing'i değiştirir!

**Detaylı Task Listesi:** `1-TODO_STAGE0_PMLP_SYNTAX.md` dosyasına bak (checkbox'lar orada)

**Dosya:** `compiler/stage1-mlp/mlp_compiler.c`

---

#### 📌 Task 2.1: Fonksiyon Parametreleri (45dk)

**Değişiklik:** `TOKEN_COMMA` → `TOKEN_SEMICOLON`

**Dosyalar:**
- `parse_function_declaration()` - Parametre listesi
- `parse_function_call()` - Çağrı parametreleri

**Syntax:**
```mlp
-- Eski (hala çalışmalı):
function add(a, b, c)

-- Yeni (ekleniyor):
function add(a; b; c)
```

**Not:** STRING içindeki virgüller korunmalı: `"merhaba, dünya"`

---

#### 📌 Task 2.2: Array Literals (30dk)

**Syntax:**
```mlp
-- Eski (hala çalışmalı):
[1, 2, 3]

-- Yeni (ekleniyor):
[1; 2; 3;]  -- trailing semicolon!
```

**Dosya:** `parse_array_literal()`

---

#### 📌 Task 2.3: List Literals (30dk)

**Syntax:**
```mlp
-- Eski (hala çalışmalı):
(1, 2, 3)

-- Yeni (ekleniyor):
(1; 2; 3;)  -- trailing semicolon!
```

**Dosya:** `parse_list_literal()`

---

#### 📌 Task 2.4: Test ve Doğrulama (30dk)

**Test dosyası:** `tests/pmlp/test_semicolon.mlp`

```mlp
function calc(x; y; z) as numeric
    arr = [1; 2; 3;]
    list = (4; 5; 6;)
    result = x + y + z
    return result
end_function

function main() as numeric
    val = calc(10; 20; 30)
    return val
end_function
```

**Test:**
```bash
./build/bin/mlpc tests/pmlp/test_semicolon.mlp /tmp/test.asm
gcc /tmp/test.asm -o /tmp/test_exec -lm -no-pie
./tmp/test_exec
echo $?  # 60 dönmeli (10+20+30)
```

**Başarı Kriterleri:**
- ✅ Fonksiyon parametreleri semicolon ile çalışıyor
- ✅ Array literal `[1; 2; 3;]` derleniyor
- ✅ List literal `(1; 2; 3;)` derleniyor
- ✅ String içindeki virgüller bozulmamış
- ✅ Eski syntax (virgül) hala çalışıyor
- ✅ Program doğru sonuç veriyor (60)

---

## 📝 GÖREV BİTİNCE (YZ_02 YAPACAK)

**1. Ana TODO'yu güncelle:**
`1-TODO_STAGE0_PMLP_SYNTAX.md` - Phase 2 checkbox'larını ✅ yap + test sonuçlarını ekle

**2. Bu dosyayı (NEXT_AI) güncelle:**
```markdown
✅ Phase 2: Semicolon Separator - TAMAMLANDI (STAGE0_YZ_02)

⏳ Phase 3: Test & Migration - ŞİMDİ (1-2h)
   └─> STAGE0_YZ_03 atanacak
```

**3. Rapor yaz:**
`TODO_STAGE0_PMLP/STAGE0_YZ/completed/STAGE0_YZ_02_TAMAMLANDI.md`

**4. Git push:**
```bash
git add .
git commit -m "STAGE0_YZ_02: Phase 2 - Semicolon Separator tamamlandı"
git push origin todo_STAGE0_YZ_02
```

---

**İlerleme:** Phase 1/3 Tamamlandı (%33) - STAGE0_YZ_01 ✅  
**Sonraki:** STAGE0_YZ_02 (Phase 2 - Semicolon Separator)  
**Hedef:** Stage0-C → PMLP tam uyumlu!
