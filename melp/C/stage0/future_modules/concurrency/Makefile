# MLP Concurrency Module - Makefile
# Module #40 - Stage 1
# MODÜLER ZİNCİR: 39 modül entegre
#
# Dosya Yapısı:
#   concurrency.h          - Header dosyası (tipler ve fonksiyon bildirimleri)
#   concurrency_parser.c   - Lexer ve Parser implementasyonu
#   concurrency_codegen.c  - Assembly kod üretici
#   concurrency_standalone.c - Ana giriş noktası ve context yönetimi

CC = gcc
CFLAGS = -Wall -g -O2

TARGET = concurrency_standalone

# Modüler kaynak dosyaları
SOURCES = concurrency_standalone.c \
          concurrency_parser.c \
          concurrency_codegen.c

HEADERS = concurrency.h

TEST_INPUT = test.mlp
TEST_OUTPUT = test.s
TEST_EXEC = test

.PHONY: all clean test info

all: $(TARGET)
	@echo "✅ Build: $(TARGET)"
	@ls -lh $(TARGET) | awk '{print "   Binary: "$$5}'

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $(SOURCES)

info:
	@echo "📦 Concurrency Module #40"
	@echo "   Header:  concurrency.h"
	@echo "   Parser:  concurrency_parser.c"
	@echo "   Codegen: concurrency_codegen.c"
	@echo "   Main:    concurrency_standalone.c"
	@wc -l $(SOURCES) $(HEADERS) | tail -1

test: $(TARGET)
	@echo ""
	@echo "=========================================="
	@echo "🔄 Concurrency Module #40 - Test"
	@echo "=========================================="
	@echo "-- Test: Full concurrency features --"
	@echo ""
	@echo '-- Thread operations' > $(TEST_INPUT)
	@echo 'spawn thread worker1 {' >> $(TEST_INPUT)
	@echo '    print("Worker 1 running")' >> $(TEST_INPUT)
	@echo '}' >> $(TEST_INPUT)
	@echo 'spawn thread worker2 {' >> $(TEST_INPUT)
	@echo '    print("Worker 2 running")' >> $(TEST_INPUT)
	@echo '}' >> $(TEST_INPUT)
	@echo 'join(worker1)' >> $(TEST_INPUT)
	@echo 'detach(worker2)' >> $(TEST_INPUT)
	@echo '' >> $(TEST_INPUT)
	@echo '-- Go-style goroutine' >> $(TEST_INPUT)
	@echo 'go func() {' >> $(TEST_INPUT)
	@echo '    print("Goroutine running")' >> $(TEST_INPUT)
	@echo '}' >> $(TEST_INPUT)
	@echo '' >> $(TEST_INPUT)
	@echo '-- Mutex operations' >> $(TEST_INPUT)
	@echo 'let m: Mutex = Mutex::new()' >> $(TEST_INPUT)
	@echo 'm.lock()' >> $(TEST_INPUT)
	@echo 'm.unlock()' >> $(TEST_INPUT)
	@echo 'm.try_lock()' >> $(TEST_INPUT)
	@echo '' >> $(TEST_INPUT)
	@echo '-- Channel operations' >> $(TEST_INPUT)
	@echo 'let ch: Channel<numeric> = Channel::new()' >> $(TEST_INPUT)
	@echo 'ch.send(42)' >> $(TEST_INPUT)
	@echo 'ch.recv()' >> $(TEST_INPUT)
	@echo '' >> $(TEST_INPUT)
	@echo '-- Atomic operations' >> $(TEST_INPUT)
	@echo 'let counter: Atomic<numeric> = Atomic::new(0)' >> $(TEST_INPUT)
	@echo 'counter.load()' >> $(TEST_INPUT)
	@echo 'counter.store(10)' >> $(TEST_INPUT)
	@echo 'counter.fetch_add(1)' >> $(TEST_INPUT)
	@echo 'counter.compare_exchange(10, 20)' >> $(TEST_INPUT)
	@echo '' >> $(TEST_INPUT)
	@echo '-- Sync primitives' >> $(TEST_INPUT)
	@echo 'let sem: Semaphore = Semaphore::new(5)' >> $(TEST_INPUT)
	@echo 'let bar: Barrier = Barrier::new(3)' >> $(TEST_INPUT)
	@echo 'let cv: CondVar = CondVar::new()' >> $(TEST_INPUT)
	@echo '' >> $(TEST_INPUT)
	@echo '-- Control structures' >> $(TEST_INPUT)
	@echo 'parallel_for i in range(0, 10) {' >> $(TEST_INPUT)
	@echo '    process(i)' >> $(TEST_INPUT)
	@echo '}' >> $(TEST_INPUT)
	@echo '' >> $(TEST_INPUT)
	@echo 'select {' >> $(TEST_INPUT)
	@echo '    case msg = <- ch:' >> $(TEST_INPUT)
	@echo '        handle(msg)' >> $(TEST_INPUT)
	@echo '    default:' >> $(TEST_INPUT)
	@echo '        skip()' >> $(TEST_INPUT)
	@echo '}' >> $(TEST_INPUT)
	@./$(TARGET) $(TEST_INPUT) $(TEST_OUTPUT) && \
	nasm -f elf64 $(TEST_OUTPUT) -o $(TEST_EXEC).o && \
	ld $(TEST_EXEC).o -o $(TEST_EXEC) && \
	./$(TEST_EXEC) && \
	echo "" && \
	echo "✅ PASSED!"

clean:
	rm -f $(TARGET) $(TEST_INPUT) $(TEST_OUTPUT) $(TEST_EXEC).o $(TEST_EXEC)
	@echo "🧹 Cleaned"
