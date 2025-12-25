#!/bin/bash
# setup_todo.sh - Yeni TODO Oluşturma Scripti
# Kullanım: ./setup_todo.sh TODO_KISA TODO_TAM TODO_BELGE

set -e  # Hata olursa dur

# Renkler
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Banner
echo -e "${BLUE}╔════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║   TODO OLUŞTURMA SİSTEMİ              ║${NC}"
echo -e "${BLUE}╚════════════════════════════════════════╝${NC}"
echo ""

# Parametreleri kontrol et
if [ "$#" -ne 3 ]; then
    echo -e "${RED}❌ HATA: Yanlış parametre sayısı${NC}"
    echo ""
    echo "Kullanım:"
    echo "  $0 TODO_KISA TODO_TAM TODO_BELGE"
    echo ""
    echo "Örnek:"
    echo "  $0 RUNTIME RUNTIME_INT 2-TODO_RUNTIME_INTEGRATION.md"
    echo ""
    echo "Parametreler:"
    echo "  TODO_KISA  : Kısa ad (STAGE0, RUNTIME, BACKEND)"
    echo "  TODO_TAM   : Tam ad (STAGE0_PMLP, RUNTIME_INT, BACKEND_LLVM)"
    echo "  TODO_BELGE : TODO belge adı (1-TODO_....md)"
    exit 1
fi

TODO_KISA=$1
TODO_TAM=$2
TODO_BELGE=$3

# Project config dosyasını oku
if [ ! -f "project_config.yaml" ]; then
    echo -e "${RED}❌ HATA: project_config.yaml bulunamadı!${NC}"
    echo "Lütfen proje kök dizininde çalıştırın."
    exit 1
fi

# Proje adını oku
PROJECT_NAME=$(grep "name:" project_config.yaml | head -1 | awk '{print $2}' | tr -d '"')

echo -e "${GREEN}📋 Yapılandırma:${NC}"
echo "  Proje      : $PROJECT_NAME"
echo "  TODO Kısa  : $TODO_KISA"
echo "  TODO Tam   : $TODO_TAM"
echo "  TODO Belge : $TODO_BELGE"
echo ""

# Template kontrolü
if [ ! -d "templates/TODO_TEMPLATE" ]; then
    echo -e "${RED}❌ HATA: templates/TODO_TEMPLATE/ bulunamadı!${NC}"
    exit 1
fi

# Hedef klasör kontrolü
TARGET_DIR="TODO_${TODO_TAM}"
if [ -d "$TARGET_DIR" ]; then
    echo -e "${YELLOW}⚠️  UYARI: $TARGET_DIR zaten mevcut!${NC}"
    read -p "Üzerine yazılsın mı? (y/N) " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        echo -e "${BLUE}ℹ️  İptal edildi.${NC}"
        exit 0
    fi
    rm -rf "$TARGET_DIR"
fi

echo -e "${BLUE}📁 TODO klasörü oluşturuluyor...${NC}"

# Template'i kopyala
cp -r templates/TODO_TEMPLATE "$TARGET_DIR"
echo "  ✓ Template kopyalandı"

# Klasör isimlerini değiştir
cd "$TARGET_DIR"
mv TODO_KISA_MM "${TODO_KISA}_MM"
mv TODO_KISA_UA "${TODO_KISA}_UA"
mv TODO_KISA_YZ "${TODO_KISA}_YZ"
echo "  ✓ Klasör isimleri güncellendi"

cd ..

# Proje prensiplerini oku (project_config.yaml'dan)
PRINCIPLES=""
while IFS= read -r line; do
    if [[ $line == *"name:"* ]] && [[ $PRINCIPLES != "" ]]; then
        PRINCIPLE_NAME=$(echo $line | awk '{print $2}' | tr -d '"')
        PRINCIPLES="${PRINCIPLES}${PRINCIPLE_NAME}, "
    fi
done < <(sed -n '/principles:/,/^[^ ]/p' project_config.yaml)
PRINCIPLES=${PRINCIPLES%, }  # Son virgülü kaldır

# Dosya içeriklerini değiştir
echo -e "${BLUE}🔄 Placeholder'lar değiştiriliyor...${NC}"

# Tarih bilgisi
CURRENT_DATE=$(date +"%d %B %Y" | sed 's/January/Ocak/; s/February/Şubat/; s/March/Mart/; s/April/Nisan/; s/May/Mayıs/; s/June/Haziran/; s/July/Temmuz/; s/August/Ağustos/; s/September/Eylül/; s/October/Ekim/; s/November/Kasım/; s/December/Aralık/')

find "$TARGET_DIR" -type f \( -name "*.md" -o -name "*.template" \) | while read -r file; do
    # Temel placeholder'lar
    sed -i "s/{PROJECT_NAME}/${PROJECT_NAME}/g" "$file"
    sed -i "s/{TODO_KISA}/${TODO_KISA}/g" "$file"
    sed -i "s/{TODO_TAM}/${TODO_TAM}/g" "$file"
    sed -i "s/{TODO_BELGE}/${TODO_BELGE}/g" "$file"
    sed -i "s/{CURRENT_DATE}/${CURRENT_DATE}/g" "$file"
    
    # Varsayılan değerler (kullanıcı sonradan düzenleyecek)
    sed -i "s/{MAIN_CODE_PATH}/compiler\/stage1-mlp\/mlp_compiler.c/g" "$file"
    sed -i "s/{MAIN_FILES}/compiler\/ tests\/ docs\//g" "$file"
    sed -i "s/{TEST_COMMAND}/\.\/build\/bin\/mlpc tests\/test.mlp \/tmp\/test.asm/g" "$file"
    sed -i "s/{PHASE1_NAME}/İlk Phase Adı/g" "$file"
    sed -i "s/{PHASE1_TIME}/1-2 saat/g" "$file"
    sed -i "s/{TASK1_NAME}/İlk Task/g" "$file"
    sed -i "s/{TASK1_TIME}/30 dakika/g" "$file"
    sed -i "s/{TASK1_DESC_1}/Task açıklaması 1/g" "$file"
    sed -i "s/{TASK1_DESC_2}/Task açıklaması 2/g" "$file"
    sed -i "s/{TASK1_DESC_3}/Task açıklaması 3/g" "$file"
    sed -i "s/{TASK2_NAME}/İkinci Task/g" "$file"
    sed -i "s/{TASK2_TIME}/45 dakika/g" "$file"
    sed -i "s/{TASK2_DESC_1}/Task açıklaması 1/g" "$file"
    sed -i "s/{TASK2_DESC_2}/Task açıklaması 2/g" "$file"
    sed -i "s/{TASK2_DESC_3}/Task açıklaması 3/g" "$file"
    sed -i "s/{TASK3_NAME}/Üçüncü Task/g" "$file"
    sed -i "s/{TASK3_TIME}/30 dakika/g" "$file"
    sed -i "s/{TASK3_DESC_1}/Task açıklaması 1/g" "$file"
    sed -i "s/{TASK3_DESC_2}/Task açıklaması 2/g" "$file"
    sed -i "s/{TASK3_DESC_3}/Task açıklaması 3/g" "$file"
    sed -i "s/{CRITERIA_1}/Başarı kriteri 1/g" "$file"
    sed -i "s/{CRITERIA_2}/Başarı kriteri 2/g" "$file"
    sed -i "s/{CRITERIA_3}/Başarı kriteri 3/g" "$file"
    
    # .template uzantısını kaldır
    if [[ $file == *.template ]]; then
        mv "$file" "${file%.template}"
    fi
done

echo "  ✓ Placeholder'lar değiştirildi"

# Özet bilgileri göster
echo ""
echo -e "${GREEN}✅ TODO başarıyla oluşturuldu!${NC}"
echo ""
echo -e "${BLUE}📂 Oluşturulan yapı:${NC}"
echo "  $TARGET_DIR/"
echo "  ├── ${TODO_KISA}_MM/"
echo "  │   └── Mastermind_buradan_basla.md"
echo "  ├── ${TODO_KISA}_UA/"
echo "  │   └── Ust_Akil_buradan_basla.md"
echo "  ├── ${TODO_KISA}_YZ/"
echo "  │   ├── NEXT_AI_START_HERE.md"
echo "  │   ├── YZ_RAPOR_TEMPLATE.md"
echo "  │   └── completed/"
echo "  └── docs/"
echo "      └── (proje spesifik belgeler ekle)"
echo ""
echo -e "${YELLOW}⚠️  SONRAKİ ADIMLAR:${NC}"
echo "  1. ${TARGET_DIR}/docs/ klasörüne proje spesifik belgeler ekle"
echo "  2. ${TODO_BELGE} dosyasını oluştur (TODO görev listesi)"
echo "  3. ${TARGET_DIR}/${TODO_KISA}_YZ/NEXT_AI_START_HERE.md'yi DÜZENLE:"
echo "     - {PHASE1_NAME}, {TASK1_NAME} gibi placeholder'ları doldur"
echo "     - {MAIN_CODE_PATH} ve {TEST_COMMAND} projeye göre ayarla"
echo "  4. ${TARGET_DIR}/${TODO_KISA}_MM/Mastermind_buradan_basla.md'yi incele"
echo "  5. Proje prensiplerini güncelle (PROJECT_PRINCIPLES bölümü)"
echo "  6. Mastermind'a TODO'yu tanıt!"
echo ""
echo -e "${BLUE}💡 İPUCU:${NC}"
echo "  NEXT_AI_START_HERE.md'de '🔍 İLK ÖNCE: MEVCUT DURUMU KONTROL ET!'"
echo "  bölümü çok önemli - YZ'nin gereksiz iş yapmasını önler!"
echo ""
echo -e "${GREEN}🚀 Hazır! İyi çalışmalar!${NC}"
