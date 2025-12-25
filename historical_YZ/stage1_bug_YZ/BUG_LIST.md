# 🐛 MELP Bug Listesi - LLVM Öncesi

**Son Güncelleme:** 20 Aralık 2025  
**Üst Akıl:** YZ_ÜA_04

---

## 🔴 Açık Bug'lar

### BUG-002: String Variable Codegen Bug

**Öncelik:** 🟡 ORTA  
**Durum:** ⏳ Ertelendi (LLVM sonrası)  
**Keşfeden:** YZ_01

**Açıklama:**
```mlp
function main() as int {
    const msg = "hello"
    println(msg)  # ❌ Segfault
    return(0)
}
```

**Sorun:**
- Type inference çalışıyor (VAR_STRING parse ediliyor)
- Ama codegen yanlış assembly üretiyor
- String variable adresi yüklenmeli ama `movq $0, %r8` üretiliyor

**Assembly (Yanlış):**
```asm
movq $0, %r8       # ❌ msg adresi yüklenmeli
movq %r8, %rdi
call puts
```

**Bakılacak Dosyalar:**
- `compiler/stage0/modules/variable/variable_codegen.c`
- `compiler/stage0/modules/expression/expression_codegen.c`

**Sebep:** LLVM'de test edilecek, belki otomatik çözülür

---

## 🟢 Çözülen Bug'lar

### BUG-001: Fonksiyon Body Boş Üretiliyor ✅

**Öncelik:** 🔴 YÜKSEK  
**Durum:** ✅ ÇÖZÜLDÜ (YZ_01)  
**Çözüm Tarihi:** 20 Aralık 2025

**Sorun:**
```mlp
function main() as int {
    const x = 88
    return(x)
}
```
→ Body boş üretiliyordu (const ve return için kod yoktu)

**Kök Sebep:** Type inference eksikti - `const int x = 88` çalışıyordu ama `const x = 88` çalışmıyordu

**Çözüm:** YZ_01 type inference implementasyonu ekledi
- `compiler/stage0/modules/variable/variable_parser.c` (+128 satır)
- Expression'dan tip çıkarma algoritması
- Numeric/String/Boolean/Collection desteği

**Test Sonuçları:**
```bash
const x = 88; return(x)      # Exit: 88 ✅
const x = 10 + 20; return(x) # Exit: 30 ✅
```

**Rapor:** `stage1_bug_YZ/YZ_01.md`  
**Commit:** `bug-001-type-inference_YZ_01`

---

## 📊 Özet

| Durum | Sayı |
|-------|------|
| 🔴 Açık | 1 |
| 🟢 Çözüldü | 0 |
| **Toplam** | 1 |

**Hedef:** Tüm bug'lar çözüldükten sonra LLVM'e geçiş!

---

**Üst Akıl:** YZ_ÜA_04
