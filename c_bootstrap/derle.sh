#!/bin/bash

# Hata durumunda script'i durdur
set -e

MLP_KAYNAK_DOSYASI=$1

if [ -z "$MLP_KAYNAK_DOSYASI" ]; then
    echo "Kullanım: ./derle.sh <dosya_adi.mlp>"
    exit 1
fi

# Dosya adından uzantıyı kaldırarak temel adı al
TEMEL_AD=$(basename -s .mlp "$MLP_KAYNAK_DOSYASI")

ASM_DOSYASI="$TEMEL_AD.asm"
NESNE_DOSYASI="$TEMEL_AD.o"
CALISTIRILABILIR_DOSYA="$TEMEL_AD"

echo "--- MLP Derleyici Zinciri Başlatılıyor ---"

echo "1. MLP -> Assembly"
./compiler_test "$MLP_KAYNAK_DOSYASI" "$ASM_DOSYASI"

echo "2. Assembly -> Nesne Kodu (nasm)"
nasm -f elf64 "$ASM_DOSYASI" -o "$NESNE_DOSYASI"

echo "3. Nesne Kodu -> Çalıştırılabilir (gcc linker + runtime)"
gcc "$NESNE_DOSYASI" ../runtime.o -o "$CALISTIRILABILIR_DOSYA" -no-pie

echo "--- Derleme Tamamlandı: '$CALISTIRILABILIR_DOSYA' oluşturuldu. ---"