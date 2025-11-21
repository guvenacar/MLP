# MLP Project Quick Start Guide

## 🚀 VSCode'da Yeni MLP Projesi Başlatma

### Yöntem 1: Boş Proje
```bash
mkdir my-mlp-project
cd my-mlp-project
code .
```

Ardından yeni bir dosya oluşturun: `main.mlp`

### Yöntem 2: Template Kullanma
```bash
mkdir my-mlp-project
cd my-mlp-project

# Template'lerden birini kopyalayın
cp /path/to/MLP/vscode-mlp/templates/hello-world.mlp main.mlp

code .
```

## 📝 Snippets Kullanımı

VSCode'da `.mlp` dosyası açıkken, aşağıdaki snippet prefix'lerini yazıp `Tab` tuşuna basın:

### Genel Snippets
- `mlp-hello` → Hello World
- `mlp-main` → Main function
- `mlp-func` → Function tanımı
- `mlp-if` → If statement
- `mlp-ifelse` → If-else statement
- `mlp-while` → While loop
- `mlp-for` → For loop
- `mlp-switch` → Switch statement

### Veri Yapıları
- `mlp-list` → List<T> oluşturma
- `mlp-struct` → Struct tanımı
- `mlp-enum` → Enum tanımı

### Dosya İşlemleri
- `mlp-fileread` → Dosya okuma
- `mlp-filewrite` → Dosya yazma

### Türkçe Snippets
- `mlp-tr-hello` → Türkçe Hello World
- `mlp-tr-func` → Türkçe fonksiyon

## 📦 Hazır Template'ler

Extension içinde şu template'ler var:

1. **hello-world.mlp** - Basit başlangıç
2. **calculator.mlp** - Hesap makinesi
3. **fibonacci.mlp** - Fibonacci hesaplama
4. **list-demo.mlp** - List<T> kullanımı
5. **file-demo.mlp** - Dosya okuma/yazma

### Template Kopyalama
```bash
# MLP repository'sini clone'ladıysanız
cp ~/projeler/tyd-lang/MLP/vscode-mlp/templates/hello-world.mlp ./main.mlp

# Veya manuel olarak template içeriğini kopyalayın
```

## 🔨 Projeyi Derleme ve Çalıştırma

### 1. MLP Compiler'ı Yükleyin
```bash
# MLP repository'sini clone edin
git clone https://github.com/guvenacar/MLP.git
cd MLP

# Compiler'ı derleyin
cd c_compiler
make

# Compiler'ı PATH'e ekleyin (opsiyonel)
export PATH=$PATH:$(pwd)
```

### 2. Projenizi Derleyin
```bash
# Yöntem 1: Doğrudan çalıştırma
~/projeler/tyd-lang/MLP/c_compiler/mlpc main.mlp output.asm
nasm -f elf64 output.asm -o output.o
gcc -no-pie output.o ~/projeler/tyd-lang/MLP/runtime/runtime.o -lm -o myprogram
./myprogram

# Yöntem 2: Tek satırda
~/projeler/tyd-lang/MLP/c_compiler/mlpc main.mlp output.asm && \
  nasm -f elf64 output.asm -o output.o && \
  gcc -no-pie output.o ~/projeler/tyd-lang/MLP/runtime/runtime.o -lm -o myprogram && \
  ./myprogram
```

## 🎯 Örnek Proje Yapısı

```
my-mlp-project/
├── main.mlp          # Ana program dosyası
├── utils.mlp         # Yardımcı fonksiyonlar (gelecek versiyonlarda module desteği ile)
├── test.mlp          # Test dosyası
├── README.md         # Proje açıklaması
└── build/            # Derlenmiş dosyalar
    ├── output.asm
    ├── output.o
    └── myprogram
```

## 🛠️ VSCode Extension'ı Güncelleme

Extension'ı güncelledikten sonra yeniden yüklemek için:

```bash
cd ~/projeler/tyd-lang/MLP/vscode-mlp

# VSIX paketini oluştur
npm install -g vsce
vsce package

# Extension'ı yükle
code --install-extension mlp-language-1.1.0.vsix

# Veya manuel yükleme
cp -r ~/projeler/tyd-lang/MLP/vscode-mlp ~/.vscode/extensions/mlp-language-1.1.0

# VSCode'u yeniden başlat
```

## 📚 Hızlı Başlangıç Örnekleri

### Örnek 1: Hello World
```mlp
print "Hello, World!"
```

### Örnek 2: Değişkenler
```mlp
int x = 42
string name = "MLP"
print x
print name
```

### Örnek 3: Fonksiyon
```mlp
int add(int a, int b) {
    return a + b;
}

print add(10, 20)
```

### Örnek 4: List<T>
```mlp
list<int> numbers = list<int>();
numbers.add(10)
numbers.add(20)
print numbers.get(0)
```

## 🎨 Türkçe Örnek

```mlp
SAYISAL toplam(SAYISAL a, SAYISAL b) {
    DÖNÜŞ a + b;
}

YAZDIR toplam(10, 20)
```

## 🐛 Sorun Giderme

**Syntax highlighting çalışmıyor:**
- VSCode'u yeniden başlatın
- Extension'ın yüklendiğinden emin olun: `Ctrl+Shift+X` → "MLP" ara

**Snippets görünmüyor:**
- `.mlp` uzantılı dosya açın
- Snippet prefix'i yazın ve `Ctrl+Space` basın

**Compiler bulunamıyor:**
- MLP compiler'ının tam yolunu kullanın
- Veya PATH'e ekleyin

## 📖 Daha Fazla Bilgi

- [MLP Documentation](https://github.com/guvenacar/MLP)
- [MLP Specs](https://github.com/guvenacar/MLP/blob/main/SPECS.md)
- [Contributing Guide](https://github.com/guvenacar/MLP/blob/main/CONTRIBUTING.md)

---

**Keyifli kodlamalar! 🚀**
