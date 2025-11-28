# MELP Phase 0 - Self-Hosting Stratejisi

## Problem
Phase 0'da sadece deklarasyon var:
```
numeric x
decimal y
boolean flag
```

Ama compiler yazmak için if/for/function lazım!

## Çözüm: Kademeli Self-Hosting

### Adım 1: Veri yapıları MELP'de
```decl
numeric token_type
numeric token_line
numeric parser_pos
numeric ast_count
```

Bu bile bir adım! Compiler'ın **state'i** MELP'de tanımlı.

### Adım 2: +Assignment ekle (Phase 1)
```decl
numeric x
x = 42
```

Artık değişkenleri init edebiliriz!

### Adım 3: +If ekle (Phase 2)
```decl
numeric x = 5
if x > 3 then
    x = 10
end if
```

Artık basit lexer yazabiliriz!

### Adım 4: +For ekle (Phase 3)
```decl
for i = 0 to 10
    process_token(i)
end for
```

Artık tam compiler yazabiliriz!

## Sonuç
Her phase'de compiler **kendi dilinde** yeniden yazılır.
Phase N compiler, Phase N+1 özellikleri ekler.

Bu klasik **bootstrapping** stratejisi!
