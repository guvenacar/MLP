# 🎯 YZ Hızlı Geliştirme Metodolojisi v3.1

**Hedef:** 10x hızlanma (20 saat → 2 saat)  
**Kanıtlanmış:** YZ_07 (1 saat), Array Parser (1-2 saat), YZ_23 (2 saat) - %100 başarı

---

# BÖLÜM 0: BAŞLAMADAN ÖNCE - ZORUNLU OKUMA! 📖

## 0.1 İlk Adım: NEXT_AI_START_HERE.md

```
⭐ HER YENİ AI MUTLAKA BURADAN BAŞLAMALI:
1. NEXT_AI_START_HERE.md → Güncel görev ne? Önceki AI ne bıraktı?
2. Bu dosya (YZ/AI_METHODOLOGY_SUM.md) → Nasıl çalışacaksın?
3. Görevle ilgili dökümanlar → Detaylı bilgi
```

**Neden bu sıra önemli:**
- NEXT_AI_START_HERE.md = Güncel durum + Spesifik görev
- AI_METHODOLOGY_SUM.md = Genel yöntem + Kurallar
- İkisini karıştırma, önce görev sonra yöntem!

---

# BÖLÜM 1: ZORUNLU KURALLAR

## 1.1 Okuma Kanıtı (Kod Yazmadan ÖNCE)

```
1. temp/kurallar_kitabı.md → ilgili bölümü oku
2. Aşağıdaki formatla özet çıkar
3. İnsan onayı bekle
4. Onay gelince kodla
```

```markdown
---
**OKUMA KANITI: [Modül Adı]**

Okuduğum bölüm: `## X. Bölüm Adı`

Anladığım kurallar:
- Kural 1
- Kural 2

Dikkat edeceğim edge case'ler:
- Case 1
- Case 2
---
```

### ⚠️ STO Kuralı (Zorunlu)

Eğer görevin STO (Smart Type Optimization) yapısını ilgilendiriyorsa:

```
1. temp/kurallar_kitabı.md → STO bölümünü oku
2. Kullanıcıya bildir: "STO bölümünü okudum, kurallara uyacağım"
3. Onay al
4. Kodla
```

**STO'yu ilgilendiren konular:**
- Type system değişiklikleri
- Runtime optimizasyonları
- Numeric/string/boolean işlemleri
- Bellek yönetimi

## 1.2 Branch Kuralı

```bash
git checkout -b feature-name_YZ_XX   # Kendi branch'ın
git push origin feature-name_YZ_XX   # Sadece buraya push

# ❌ ASLA merge isteği yapma
# ❌ ASLA main'e direkt push
```

## 1.3 Modülerlik (KESİNLİKLE)

```
✅ modules/ altında bağımsız dosyalar
✅ JSON/pipes ile iletişim
❌ MERKEZİ DOSYA ÜRETİLMEYECEK
❌ main.c'ye ekleme yapılmayacak
```

## 1.4 Devir Teslim (Görev Sonunda)

```
1. YZ/YZ_XX.md → Yaptıklarını DETAYLI yaz:
   - Problem ne idi?
   - Nasıl çözdün?
   - Hangi dosyaları değiştirdin?
   - Test sonuçları
   - Known limitations (varsa)

2. NEXT_AI_START_HERE.md → Sonraki YZ'ye bilgi ver:
   - Görev tamamlandı mı?
   - Kalan iş var mı?
   - Önerilen sonraki adım
   - ⚠️ MUTLAKA EKLE: "YZ/AI_METHODOLOGY_SUM.md'yi oku!"

3. TODO.md → Güncelle:
   - Tamamlanan görevleri işaretle [x]
   - Yeni bulduğun TODO'ları ekle [ ]
   - Completion % güncelle

4. Hız kazanımını hesapla ve yaz
```

**YZ_23 Örneği (İyi Devir Teslim):**
- YZ/YZ_23.md: 300+ satır detaylı rapor
- NEXT_AI_START_HERE.md: YZ_24 için net seçenekler
- TODO.md: Phase 3 %95 olarak güncellendi
- Whitespace validation TODO olarak eklendi

---

# BÖLÜM 2: HIZLANMA TEKNİKLERİ

## 2.1 İki Yöntem

| Durum | Yöntem |
|-------|--------|
| Temiz kod, <100 satır değişiklik | **5 Adımlı Method** |
| Karmaşık kod, >200 satır, legacy | **REWRITE Method** |

---

## 2.2 Beş Adımlı Method (~2 saat)

### Adım 1: CONTEXT (15 dk)
```
Oku:
├── NEXT_AI_START_HERE.md → ÖNCE BUNU! Güncel görev + Önceki AI'ın notları
├── TODO.md → Genel proje durumu
├── YZ/YZ_XX.md (son AI) → Detaylı implementation notları
├── temp/kurallar_kitabı.md → İlgili bölüm (görevle alakalıysa)
└── ARCHITECTURE.md → Mimari kurallar (modülerlik vb.)
```

**YZ_23 Örneği:**
- NEXT_AI_START_HERE.md → "YZ_22 syntax hatası bıraktı" (net görev)
- kurallar_kitabı.md → Satır 669-750 (collection syntax rules)
- YZ_22.md → "Known Issue" bölümü (problem detayı)

### Adım 2: PATTERN DISCOVERY (20 dk)

**Sezgilerini kullan!** Benzer işi yapmış YZ'yi bul ve model al.

```
Örnek: List modülü yapacaksan → Array yapan YZ'yi incele
       Tuple codegen → List veya Array codegen yapan YZ
       String ops → Arithmetic ops yapan YZ
       Syntax fix → Parser precedence örneklerine bak
```

**İnceleme sırası:**
```
1. YZ/YZ_XX.md → Benzer görevi kim yaptı?
2. O YZ'nin değiştirdiği dosyalar
3. O YZ'nin yazdığı testler
4. O YZ'nin kullandığı pattern
```

**Kod araması:**
```bash
# Benzer kodu bul
grep -rn "benzer_fonksiyon" modules/

# Reference dosyayı oku
cat modules/functions/functions_codegen.c

# Belirli bir pattern ara
grep -r "TOKEN_LBRACKET" modules/
```

**Pattern Kütüphanesi:**
- `modules/functions/functions_codegen.c` - Best reference
- `modules/arithmetic/arithmetic_codegen.c` - Binary ops
- `modules/arithmetic/arithmetic_parser.c` - Parser precedence (YZ_23 fixed this!)
- `modules/comparison/comparison_codegen.c` - Comparisons
- `modules/array/array_parser.c` - Collection parsing

**YZ_23 Örneği:**
- YZ_21 pattern'ini inceledi (tuple indexing)
- YZ_22'nin bıraktığı kodu analiz etti
- arithmetic_parser.c'de precedence sorununu buldu
- Pattern: "Collection access BEFORE function call"

### Adım 3: MINIMAL CODE (45 dk)
```
✅ 3-5 dosya maksimum
✅ <100 satır değişiklik (genellikle)
✅ Pattern'i takip et
✅ Incremental değişiklikler
❌ Overengineering yapma
❌ "Belki lazım olur" kodu yazma
```

**Strateji: Precedence-Based Implementation**

Eğer mevcut kod zaten çalışıyor ama **sıralama/precedence sorunu** varsa:
1. Mevcut kodu SİLME
2. Sadece SIRASINI değiştir
3. Eksik kontrolleri EKLE

**YZ_23 Örneği (Precedence Fix):**
```
Sorun: myList(0) → function call olarak parse ediliyor
Çözüm: Collection access kontrolünü ÖNCE yap
Değişiklik: ~60 satır (silme + ekleme)
Sonuç: Syntax düzeldi, hiçbir feature bozmadı
```

**Dosya Sayısı:**
- Küçük düzeltme: 1-2 dosya
- Orta feature: 3-5 dosya  
- Büyük feature: 5-8 dosya (bölmeyi düşün!)

### Adım 4: TEST (30 dk)
```bash
# 1. Temiz derleme
cd compiler/stage0
make clean && make

# 2. Minimal test yaz
cat > test_feature.mlp << 'EOF'
function main() returns numeric
    -- Test kodun
    return 42
end function
EOF

# 3. Derle ve çalıştır
./modules/functions/functions_compiler test_feature.mlp test_feature.s
gcc test_feature.s -L../../runtime/stdlib -lmlp_stdlib -L../../runtime/sto -lsto_runtime -lm -o test_feature
./test_feature
echo $?  # Exit code'u kontrol et
```

**Test Stratejisi:**
1. En basit case → Çalışıyor mu?
2. Edge case → Sınır değerler?
3. Mixed case → Diğer featurelerle uyum?

**YZ_23 Test Seviyeleri:**
- test_array_syntax.mlp → arr[1] ✅
- test_list_syntax.mlp → myList(1) ✅
- test_tuple_syntax.mlp → myPair<0> ✅
- test_all_syntax.mlp → Hepsi birlikte ✅

### Adım 5: DOCUMENT (30 dk)
→ Bölüm 1.4'teki devir teslim kurallarını uygula

---

## 2.3 REWRITE Method (Karmaşık Kod İçin)

**Ne zaman kullan:**
- Mevcut kod >200 satır ve karmaşık
- Stateful → Stateless dönüşüm
- Anlamak düzeltmekten uzun sürüyorsa

**Nasıl:**
```
1. Mevcut kodu anlamaya ÇALIŞMA
2. Reference'tan pattern al
3. Sıfırdan yaz
4. Test et
```

**Örnek:** Array Parser - 440 satır stateful → 427 satır stateless, 2 saat

---

## 2.4 Hızlanma Teknikleri

### Grep Komutları
```bash
grep -rn "function_name" modules/      # Pattern ara
grep -r "#include" modules/ | grep "x.h"  # Dependency bul
find modules/ -name "*.c" -exec grep -l "pattern" {} \;
```

### Compiler-Guided Development
```
1. Eksik kod yaz
2. Derlemeye çalış
3. Error mesajını oku
4. Düzelt
5. Tekrarla

Compiler = rehberin!
```

### Parallel Operations
```bash
# Aynı anda birden fazla dosya düzenle
# Bağımsız okumalar paralel yap
```

---

## 2.5 Anti-Pattern'ler (YAPMA)

| ❌ Anti-Pattern | ✅ Doğrusu | 📝 YZ Örneği |
|-----------------|-----------|--------------|
| 2 saat kod okuma, 0 satır yazma | 20 dk pattern bul, 1 saat yaz | YZ_23: 30dk okuma, 1.5h kod |
| "Belki lazım olur" 500 satır | "Şimdi lazım" 50 satır | YZ_23: ~93 satır değişiklik |
| 10 dosya, 1000 satır, 1 commit | 3-5 dosya, <150 satır, incremental | YZ_23: 3 dosya, test-driven |
| Dokümantasyonsuz bitir | Yaz, test et, dokümante et | YZ_23: 300+ satır dokümantasyon |
| "Kolay yol" merkezi dosya | "Doğru yol" modüler | YZ_23: Sadece modules/ düzenledi |
| Parser'ı baştan yaz | Precedence'ı düzelt | YZ_23: Sırayı değiştir, feature ekle |
| Tüm edge case'leri şimdi | MVP çalışsın, sonra extend | YZ_23: Syntax fix → Whitespace TODO |

---

## 2.6 Debugging & Problem Çözme

### Compile Error Strategy
```bash
# 1. Derlemeyi dene
make modules 2>&1 | tee build.log

# 2. İlk error'a odaklan
head -20 build.log

# 3. Compiler sana ne diyor?
# - Undefined reference → eksik fonksiyon/link
# - Type mismatch → tip uyuşmazlığı
# - Undeclared → header eksik
```

### Runtime Error Strategy
```bash
# 1. Assembly çıktısını kontrol et
cat test.s | grep -A 5 "problem_area"

# 2. Exit code'u yorumla
./test; echo $?
# 0 = başarı, 42 = panic, diğerleri = return value

# 3. Segfault varsa → bounds check, null pointer
gdb ./test
(gdb) run
(gdb) backtrace
```

**YZ_23 Debug Örneği:**
```
Problem: myList(1) → "undefined reference to myList"
Analiz: grep "myList" test.s → "call myList" görünüyor
Sebep: Parser myList(1)'i function call olarak yorumluyor
Fix: Collection access'i function call'dan ÖNCE kontrol et
```

---

# BÖLÜM 3: CHECKLIST & TAKİP

## 3.1 Başlangıç Checklist
- [ ] **NEXT_AI_START_HERE.md** okudum (ÖNCE BU!)
- [ ] **YZ/AI_METHODOLOGY_SUM.md** okudum (bu dosya)
- [ ] Kurallar kitabını okudum (görevle ilgili bölüm)
- [ ] Özet çıkardım ve insan onayı aldım (gerekiyorsa)
- [ ] Branch oluşturdum: `feature_YZ_XX`
- [ ] Pattern buldum (benzer YZ'yi inceledim)

## 3.2 Geliştirme Checklist (Süreç İçinde)
- [ ] Minimal kod yazdım (<150 satır değişiklik)
- [ ] Incremental test ettim (her değişiklik sonrası)
- [ ] Compiler error'ları düzelttim
- [ ] En az 1 test passing

## 3.3 Bitiş Checklist
- [ ] Tüm testler çalışıyor
- [ ] Merkezi dosyaya dokunmadım (main.c, pipeline.c vb.)
- [ ] **YZ/YZ_XX.md** yazdım (detaylı: problem, çözüm, dosyalar, testler, TODO)
- [ ] **NEXT_AI_START_HERE.md** güncelledim (sonraki AI için net görev)
- [ ] **TODO.md** güncelledim (completion %, yeni TODO'lar)
- [ ] Hız kazanımını hesapladım
- [ ] Branch'a push ettim (`git push origin feature_YZ_XX`)

## 3.4 Hız Takibi (Her Görev Sonunda)

```
Tahmini süre: ___ saat
Gerçek süre:  ___ saat
Hızlanma:     ___x

Örnek (YZ_23):
Tahmini süre: 3-4 saat (NEXT_AI_START_HERE.md)
Gerçek süre:  2 saat
Hızlanma:     1.75x
```

---

## 3.5 Başarı Metrikleri

✅ **Hızlı:** <3 saat (tahmininin %50-75'i)  
✅ **Temiz:** 0 compiler warning (ya da <5 non-critical)  
✅ **Çalışan:** ≥1 test passing, critical path test edilmiş  
✅ **Dokümante:** YZ_XX.md + NEXT_AI_START_HERE.md + TODO.md güncel  
✅ **Modular:** ARCHITECTURE.md uyumlu, merkezi dosya yok  

**İyi Örnek (YZ_23):**
- ✅ 2 saat (tahmini 3-4)
- ✅ 0 critical warning
- ✅ 4 test passing (array, list, tuple, mixed)
- ✅ 300+ satır dokümantasyon
- ✅ Sadece modules/ düzenlendi

---

# BÖLÜM 4: ÖZEL DURUMLAR

## 4.1 Syntax Fix (YZ_23 Pattern)

**Ne zaman:** Mevcut feature çalışıyor ama syntax yanlış

**Yaklaşım:**
1. ❌ Tüm kodu yeniden yazma
2. ✅ Sadece precedence/sıralama düzelt
3. ✅ Eksik case'leri ekle (ör: TOKEN_LANGLE)

**Steps:**
```
1. Mevcut kod nerede parse ediyor? → grep ile bul
2. Neden yanlış parse ediyor? → precedence sorunu mu?
3. Doğru sıralama ne olmalı? → kurallar_kitabı.md'ye bak
4. Minimal değişiklik yap → sadece IF sırasını değiştir
5. Test et → önceki testler bozulmamalı
```

**YZ_23 Specifics:**
- Problem: `myList(0)` → function call
- Root cause: Function call önce, collection access sonra
- Fix: Collection access önce, function call sonra
- Ekleme: TOKEN_LANGLE support for tuples
- Result: ~93 satır, 2 saat, 0 broken test

## 4.2 Known Limitations Pattern

**Ne zaman:** Feature çalışıyor ama %100 tamamlanmamış

**Yapılacaklar:**
1. ✅ Known Limitations bölümü yaz (YZ_XX.md'de)
2. ✅ TODO.md'ye sonraki AI için madde ekle
3. ✅ NEXT_AI_START_HERE.md'de açıkça belirt
4. ✅ Priority belirle (⭐⭐⭐ CRITICAL vs ⭐ LOW)

**YZ_23 Örneği:**
```markdown
## Known Limitation: Whitespace Validation

**Not enforced:** myList (0) with space should error
**Priority:** ⭐⭐ MEDIUM (syntax works, just not strict)
**Estimated:** 2-3 hours
**Files:** lexer.h, lexer.c, array_parser.c
```

---

**Altın Kural:**
> Oku (NEXT_AI_START_HERE.md) → Özet çıkar → Onay al → Pattern bul → Minimal kodla → Test et → Dokümante et (detaylı!) → Hızı ölç

**Son Güncelleme:** 10 Aralık 2025, 22:30 (YZ_23 deneyimleriyle güncellenmiş)