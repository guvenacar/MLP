# Functions Module - Standalone Test Compiler Makefile
# Modül #7 (P0 Critical)
#
# Bağımlılıklar:
#   - variable (modül #1)
#   - comments (modül #2)
#   - arithmetic (modül #3)
#   - comparison (modül #4)
#   - logical (modül #5)
#   - control_flow (modül #6)

CC = gcc
CFLAGS = -Wall -Wextra -g -I../..
NASM = nasm
NASMFLAGS = -f elf64
LD = ld

# Ana hedef
TARGET = functions_compiler

# Ana kaynak dosyalar
MAIN_SRC = functions_standalone.c

# Functions modülü kaynakları
FUNCTIONS_SOURCES = \
	functions.c \
	functions_parser.c \
	functions_codegen.c

# Variable modülü (Modül #1)
VARIABLE_DIR = ../variable
VARIABLE_SOURCES = \
	$(VARIABLE_DIR)/variable.c \
	$(VARIABLE_DIR)/variable_parser.c \
	$(VARIABLE_DIR)/variable_codegen.c

# Comments modülü (Modül #2)
COMMENTS_DIR = ../comments
COMMENTS_SOURCES = \
	$(COMMENTS_DIR)/comments.c \
	$(COMMENTS_DIR)/comments_parser.c

# Arithmetic modülü (Modül #3)
ARITHMETIC_DIR = ../arithmetic
ARITHMETIC_SOURCES = \
	$(ARITHMETIC_DIR)/arithmetic.c \
	$(ARITHMETIC_DIR)/arithmetic_parser.c \
	$(ARITHMETIC_DIR)/arithmetic_codegen.c

# Comparison modülü (Modül #4)
COMPARISON_DIR = ../comparison
COMPARISON_SOURCES = \
	$(COMPARISON_DIR)/comparison.c \
	$(COMPARISON_DIR)/comparison_parser.c \
	$(COMPARISON_DIR)/comparison_codegen.c

# Logical modülü (Modül #5)
LOGICAL_DIR = ../logical
LOGICAL_SOURCES = \
	$(LOGICAL_DIR)/logical.c \
	$(LOGICAL_DIR)/logical_parser.c \
	$(LOGICAL_DIR)/logical_codegen.c

# Control Flow modülü (Modül #6)
CONTROL_FLOW_DIR = ../control_flow
CONTROL_FLOW_SOURCES = \
	$(CONTROL_FLOW_DIR)/control_flow.c \
	$(CONTROL_FLOW_DIR)/control_flow_parser.c \
	$(CONTROL_FLOW_DIR)/control_flow_codegen.c

# Lexer (merkezi)
LEXER_SOURCES = ../../lexer.c

# Tüm kaynak dosyalar
SOURCES = \
	$(MAIN_SRC) \
	$(FUNCTIONS_SOURCES) \
	$(VARIABLE_SOURCES) \
	$(COMMENTS_SOURCES) \
	$(ARITHMETIC_SOURCES) \
	$(COMPARISON_SOURCES) \
	$(LOGICAL_SOURCES) \
	$(CONTROL_FLOW_SOURCES) \
	$(LEXER_SOURCES)

# Object dosyaları
OBJECTS = $(SOURCES:.c=.o)

# Test dosyaları
TEST_SOURCE = test_func.mlp
TEST_ASM = test_func.s
TEST_OBJ = test_func.o
TEST_EXEC = test_func

# Varsayılan hedef
all: info $(TARGET)
	@echo ""
	@echo "✅ Functions Standalone Compiler hazır!"
	@echo "   Binary: $(TARGET)"
	@echo "   Boyut: $$(du -h $(TARGET) | cut -f1)"
	@echo ""
	@echo "Test için: make test"

# Bilgilendirme
info:
	@echo "=========================================="
	@echo "  FUNCTIONS MODULE BUILD"
	@echo "  Modül #7 (P0 Critical)"
	@echo "=========================================="
	@echo "Bağımlılıklar:"
	@echo "  • variable (Modül #1)"
	@echo "  • comments (Modül #2)"
	@echo "  • arithmetic (Modül #3)"
	@echo "  • comparison (Modül #4)"
	@echo "  • logical (Modül #5)"
	@echo "  • control_flow (Modül #6)"
	@echo ""
	@echo "Toplam kaynak dosya sayısı: $$(echo $(SOURCES) | wc -w)"
	@echo ""

# Ana derleyici
$(TARGET): $(OBJECTS)
	@echo "🔗 Linking: $(TARGET)"
	$(CC) $(CFLAGS) -o $@ $^

# C dosyalarını derle
%.o: %.c
	@echo "  📦 Compiling: $<"
	$(CC) $(CFLAGS) -c $< -o $@

# Test hedefi
test: $(TARGET)
	@echo ""
	@echo "=========================================="
	@echo "  FUNCTIONS MODULE TEST"
	@echo "=========================================="
	@echo ""
	@echo "📝 Test dosyası oluşturuluyor..."
	@echo 'sayisal x = 10' > $(TEST_SOURCE)
	@echo 'sayisal y = 20' >> $(TEST_SOURCE)
	@echo '' >> $(TEST_SOURCE)
	@echo 'def topla(sayisal a, sayisal b) {' >> $(TEST_SOURCE)
	@echo '    sayisal sonuc = a + b' >> $(TEST_SOURCE)
	@echo '    return sonuc' >> $(TEST_SOURCE)
	@echo '}' >> $(TEST_SOURCE)
	@echo '' >> $(TEST_SOURCE)
	@echo 'def carpma(sayisal a, sayisal b) {' >> $(TEST_SOURCE)
	@echo '    return a * b' >> $(TEST_SOURCE)
	@echo '}' >> $(TEST_SOURCE)
	@echo '' >> $(TEST_SOURCE)
	@echo 'sayisal z = topla(x, y)' >> $(TEST_SOURCE)
	@echo 'sayisal w = carpma(x, 5)' >> $(TEST_SOURCE)
	@echo ""
	@echo "Test dosyası içeriği ($$(wc -c < $(TEST_SOURCE)) bytes):"
	@cat $(TEST_SOURCE)
	@echo ""
	@echo "---"
	@echo ""
	@echo "🔨 Derleme başlıyor..."
	./$(TARGET) $(TEST_SOURCE) $(TEST_ASM)
	@echo ""
	@echo "📜 Üretilen Assembly (ilk 50 satır):"
	@head -50 $(TEST_ASM)
	@echo "..."
	@echo ""
	@echo "🔧 NASM ile assembly..."
	$(NASM) $(NASMFLAGS) $(TEST_ASM) -o $(TEST_OBJ)
	@echo "  ✅ NASM: OK"
	@echo ""
	@echo "🔗 Linking..."
	$(LD) $(TEST_OBJ) -o $(TEST_EXEC)
	@echo "  ✅ Linking: OK"
	@echo ""
	@echo "🚀 Çalıştırılıyor..."
	./$(TEST_EXEC) && echo "  ✅ Execution: OK (exit code $$?)" || echo "  ❌ Execution failed (exit code $$?)"
	@echo ""
	@echo "=========================================="
	@echo "  ✅ TEST TAMAMLANDI"
	@echo "=========================================="

# Temizlik
clean:
	@echo "🧹 Temizlik yapılıyor..."
	rm -f $(OBJECTS) $(TARGET) $(TEST_SOURCE) $(TEST_ASM) $(TEST_OBJ) $(TEST_EXEC)
	@echo "✅ Temizlendi!"

.PHONY: all info test clean
