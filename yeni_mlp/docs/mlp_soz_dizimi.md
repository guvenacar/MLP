# MLP Söz Dizimi Referansı (Syntax Reference)

**Versiyon:** 3.0  
**Tarih:** 25 Kasım 2025  
**Durum:** Üretim Hazır ✅

---

## 📋 İçindekiler

1. [Veri Tipleri](#1-veri-tipleri)
2. [Değişken Tanımlama](#2-değişken-tanımlama)
3. [Koşullar (if-then-else)](#3-koşullar-if-then-else)
4. [Döngüler (for, while)](#4-döngüler-for-while)
5. [Fonksiyonlar](#5-fonksiyonlar)
6. [Yapılar (struct)](#6-yapılar-struct)
7. [Numaralandırmalar (enum)](#7-numaralandırmalar-enum)
8. [Diziler (Array)](#8-diziler-array)
9. [Dinamik Listeler (list)](#9-dinamik-listeler-list)
10. [Hash Map (map)](#10-hash-map-map)
11. [Switch-Case](#11-switch-case)
12. [Lambda Fonksiyonlar](#12-lambda-fonksiyonlar)
13. [İçe Aktarma (import)](#13-içe-aktarma-import)
14. [Try-Catch-Finally](#14-try-catch-finally)
15. [Async-Await](#15-async-await)
16. [String İşlemleri](#16-string-i̇şlemleri)
17. [Dosya İşlemleri](#17-dosya-i̇şlemleri)
18. [Yorumlar](#18-yorumlar)

---

## 1. Veri Tipleri

MLP'de 3 temel veri tipi vardır:

```mlp
-- Sayısal (64-bit integer)
Numeric x = 42
Numeric pi = 3.14

-- Metin (String)
String name = "Alice"
String empty = ""

-- Mantıksal (Boolean)
Boolean is_valid = true
Boolean is_done = false
```

**Özel Değerler:**
```mlp
-- Null değer
String maybe_text = null

-- Tip kontrolü
String result = typeof(x)  -- "Numeric"
```

---

## 2. Değişken Tanımlama

```mlp
-- Tip belirtilerek
Numeric age = 25
String city = "Istanbul"

-- Sabit değişken
const Numeric MAX_SIZE = 100
const String APP_NAME = "MLP Compiler"

-- Çoklu tanımlama
Numeric x = 10
Numeric y = 20
Numeric z = x + y
```

---

## 3. Koşullar (if-then-else)

### Basit If

```mlp
if x > 0 then
    print "Pozitif"
end_if
```

### If-Else

```mlp
if temperature > 30 then
    print "Sıcak"
else
    print "Soğuk"
end_if
```

### If-Else If-Else

```mlp
if score >= 90 then
    print "A"
else if score >= 80 then
    print "B"
else if score >= 70 then
    print "C"
else
    print "F"
end_if
```

### İç İçe If

```mlp
if is_logged_in == true then
    if is_admin == true then
        print "Admin paneli"
    else
        print "Kullanıcı paneli"
    end_if
end_if
```

---

## 4. Döngüler (for, while)

### For Döngüsü

```mlp
-- Basit for döngüsü
for i = 0 to 10
    print i
end_for

-- Adım (step) ile
for i = 0 to 100 step 5
    print i  -- 0, 5, 10, 15, ...
end_for

-- Geriye doğru
for i = 10 to 0 step -1
    print i  -- 10, 9, 8, ...
end_for
```

### While Döngüsü

```mlp
-- Koşullu döngü
Numeric count = 0
while count < 5
    print count
    count = count + 1
end_while

-- Sonsuz döngü (break ile çık)
while true
    Numeric input = read_input()
    if input == 0 then
        break
    end_if
    print input
end_while
```

### Break ve Continue

```mlp
-- Break: Döngüden çık
for i = 0 to 100
    if i == 50 then
        break
    end_if
    print i
end_for

-- Continue: Sonraki iterasyona geç
for i = 0 to 10
    if i == 5 then
        continue
    end_if
    print i  -- 5 atlanır
end_for
```

### For-Each (Range ile)

```mlp
-- 1'den 10'a kadar
for i in 1..10
    print i
end_for

-- Liste üzerinde iterasyon
list[Numeric] numbers = [1, 2, 3, 4, 5]
for item in numbers
    print item
end_for
```

---

## 5. Fonksiyonlar

### Basit Fonksiyon

```mlp
function greet()
    print "Merhaba!"
end_function

-- Çağırma
greet()
```

### Parametreli Fonksiyon

```mlp
function add(Numeric a, Numeric b)
    return a + b
end_function

Numeric result = add(5, 3)  -- 8
```

### Çoklu Parametreli

```mlp
function calculate_area(Numeric width, Numeric height)
    return width * height
end_function

Numeric area = calculate_area(10, 20)  -- 200
```

### Return İfadesi

```mlp
function is_even(Numeric num)
    if num % 2 == 0 then
        return true
    else
        return false
    end_if
end_function

Boolean result = is_even(42)  -- true
```

### Void Fonksiyon (dönüş değeri yok)

```mlp
function print_header()
    print "==============="
    print "  MLP Program  "
    print "==============="
end_function
```

---

## 6. Yapılar (struct)

### Struct Tanımlama

```mlp
struct Person
    String name
    Numeric age
    String city
end_struct
```

### Struct Kullanımı

```mlp
-- Oluşturma
Person alice
alice.name = "Alice"
alice.age = 25
alice.city = "Istanbul"

-- Erişim
print alice.name  -- "Alice"
print alice.age   -- 25

-- Fonksiyon parametresi
function print_person(Person p)
    print p.name
    print p.age
end_function

print_person(alice)
```

### İç İçe Struct

```mlp
struct Address
    String street
    String city
end_struct

struct Employee
    String name
    Address address
end_struct

Employee emp
emp.name = "Bob"
emp.address.city = "Ankara"
```

---

## 7. Numaralandırmalar (enum)

### Enum Tanımlama

```mlp
enum Color
    RED
    GREEN
    BLUE
end_enum
```

### Enum Kullanımı

```mlp
Color my_color = Color.RED

if my_color == Color.RED then
    print "Renk kırmızı"
end_if
```

### Değerli Enum

```mlp
enum HttpStatus
    OK = 200
    NOT_FOUND = 404
    SERVER_ERROR = 500
end_enum

HttpStatus status = HttpStatus.OK
print status  -- 200
```

---

## 8. Diziler (Array)

### Dizi Tanımlama

```mlp
-- Sabit boyutlu dizi
Numeric numbers[10]

-- İlk değer atama
Numeric scores[5] = [100, 85, 90, 78, 92]
```

### Dizi Erişimi

```mlp
-- Eleman erişimi
numbers[0] = 42
numbers[1] = 99

Numeric first = numbers[0]  -- 42

-- Döngü ile erişim
for i = 0 to 5
    print scores[i]
end_for
```

### Çok Boyutlu Dizi

```mlp
-- 2D dizi (matris)
Numeric matrix[3][3]

matrix[0][0] = 1
matrix[0][1] = 2
matrix[1][0] = 3

-- İç içe döngü ile doldurma
for i = 0 to 3
    for j = 0 to 3
        matrix[i][j] = i * 3 + j
    end_for
end_for
```

---

## 9. Dinamik Listeler (list)

### Liste Oluşturma

```mlp
-- Boş liste
list[Numeric] numbers = list()

-- Başlangıç değerleri ile
list[String] names = ["Alice", "Bob", "Charlie"]
```

### Liste Metodları

```mlp
list[Numeric] nums = list()

-- Eleman ekleme
nums.add(10)
nums.add(20)
nums.add(30)

-- Erişim
Numeric first = nums.get(0)  -- 10

-- Boyut
Numeric size = nums.size()  -- 3

-- Güncelleme
nums.set(1, 99)  -- [10, 99, 30]

-- Silme
nums.remove(0)  -- [99, 30]

-- Temizleme
nums.clear()  -- []

-- Boş kontrolü
Boolean is_empty = (nums.size() == 0)
```

### Liste Üzerinde Döngü

```mlp
list[String] fruits = ["Elma", "Armut", "Kiraz"]

for i = 0 to fruits.size()
    print fruits.get(i)
end_for

-- veya for-each ile
for fruit in fruits
    print fruit
end_for
```

---

## 10. Hash Map (map)

### Map Oluşturma

```mlp
-- String → Numeric
map[String, Numeric] ages = map()

-- String → String
map[String, String] config = map()
```

### Map Metodları

```mlp
map[String, Numeric] scores = map()

-- Ekleme/Güncelleme
scores.set("Alice", 95)
scores.set("Bob", 87)
scores.set("Charlie", 92)

-- Erişim
Numeric alice_score = scores.get("Alice")  -- 95

-- Kontrol
Boolean has_bob = scores.has("Bob")  -- true

-- Silme
scores.remove("Charlie")

-- Boyut
Numeric count = scores.size()  -- 2

-- Temizleme
scores.clear()
```

### Map Kullanım Örnekleri

```mlp
-- Konfigürasyon
map[String, String] config = map()
config.set("host", "localhost")
config.set("port", "8080")

String host = config.get("host")

-- Kelime sayacı
map[String, Numeric] word_count = map()
word_count.set("hello", 3)
word_count.set("world", 2)

if word_count.has("hello") then
    Numeric count = word_count.get("hello")
    print "hello kelimesi " + str(count) + " kez geçiyor"
end_if
```

---

## 11. Switch-Case

### Basit Switch

```mlp
Numeric day = 3

switch day
    case 1:
        print "Pazartesi"
    case 2:
        print "Salı"
    case 3:
        print "Çarşamba"
    default:
        print "Bilinmeyen gün"
end_switch
```

### String ile Switch

```mlp
String command = "start"

switch command
    case "start":
        print "Başlatılıyor..."
    case "stop":
        print "Durduruluyor..."
    case "restart":
        print "Yeniden başlatılıyor..."
    default:
        print "Bilinmeyen komut"
end_switch
```

### Fall-through (break olmadan)

```mlp
Numeric grade = 85

switch grade / 10
    case 10:
    case 9:
        print "A"
    case 8:
        print "B"
    case 7:
        print "C"
    default:
        print "F"
end_switch
```

---

## 12. Lambda Fonksiyonlar

### Basit Lambda

```mlp
-- Lambda tanımlama
lambda square = (Numeric x) => x * x

-- Kullanım
Numeric result = square(5)  -- 25
```

### Çoklu Parametreli Lambda

```mlp
lambda add = (Numeric a, Numeric b) => a + b

Numeric sum = add(10, 20)  -- 30
```

### Lambda ile Map/Filter

```mlp
list[Numeric] numbers = [1, 2, 3, 4, 5]

-- Her elemana lambda uygula
for i = 0 to numbers.size()
    Numeric val = numbers.get(i)
    numbers.set(i, val * 2)  -- [2, 4, 6, 8, 10]
end_for
```

---

## 13. İçe Aktarma (import)

### Modül İçe Aktarma

```mlp
-- Dosya import
import "utils/helpers.mlp"
import "data/models.mlp"

-- Fonksiyon kullanımı
Numeric result = calculate_total(items)
```

### Relative Import

```mlp
-- Aynı dizin
import "config.mlp"

-- Üst dizin
import "../shared/common.mlp"

-- Alt dizin
import "modules/auth.mlp"
```

---

## 14. Try-Catch-Finally

### Hata Yakalama

```mlp
try
    Numeric result = risky_operation()
    print "Başarılı: " + str(result)
catch
    print "Hata oluştu!"
end_try
```

### Hata Türü ile

```mlp
try
    Numeric value = string_to_int("abc")
catch error
    print "Hata: " + error.message
end_try
```

### Finally ile Temizlik

```mlp
try
    String data = read_file("data.txt")
    process_data(data)
catch error
    print "Dosya okunamadı: " + error.message
finally
    print "İşlem tamamlandı"
    cleanup_resources()
end_try
```

---

## 15. Async-Await

### Async Fonksiyon

```mlp
async function fetch_data(String url)
    -- Asenkron işlem
    String response = await http_get(url)
    return response
end_function
```

### Await Kullanımı

```mlp
async function main()
    String data = await fetch_data("https://api.example.com")
    print "Veri alındı: " + data
end_function
```

### Paralel Await

```mlp
async function fetch_multiple()
    -- İki asenkron işlem paralel başlar
    String data1 = await fetch_data("url1")
    String data2 = await fetch_data("url2")
    
    print "Her ikisi de tamamlandı"
end_function
```

---

## 16. String İşlemleri

### String Birleştirme

```mlp
String first_name = "Ali"
String last_name = "Yılmaz"

String full_name = first_name + " " + last_name  -- "Ali Yılmaz"
```

### String Metodları

```mlp
String text = "Hello World"

-- Uzunluk
Numeric len = string_length(text)  -- 11

-- Alt string
String sub = substring(text, 0, 5)  -- "Hello"

-- Büyük/küçük harf
String upper = string_upper(text)  -- "HELLO WORLD"
String lower = string_lower(text)  -- "hello world"

-- Bölme
list[String] words = string_split(text, " ")  -- ["Hello", "World"]

-- Arama
Numeric pos = string_find(text, "World")  -- 6

-- Değiştirme
String new_text = string_replace(text, "World", "MLP")  -- "Hello MLP"

-- Başlangıç/bitiş kontrolü
Boolean starts = string_starts_with(text, "Hello")  -- true
Boolean ends = string_ends_with(text, "World")  -- true

-- Trim (boşluk temizleme)
String trimmed = string_trim("  test  ")  -- "test"
```

### String İnterpolasyon

```mlp
Numeric age = 25
String name = "Alice"

-- İnterpolasyon
String message = "Merhaba ${name}, yaşınız ${age}"
print message  -- "Merhaba Alice, yaşınız 25"
```

### Tip Dönüşümü

```mlp
-- Sayıdan string'e
Numeric num = 42
String str_num = int_to_string(num)  -- "42"

-- String'den sayıya
String text = "123"
Numeric value = string_to_int(text)  -- 123

-- Karakter kodu
Numeric code = char_code("A")  -- 65
String char = char_from_code(65)  -- "A"
```

---

## 17. Dosya İşlemleri

### Dosya Okuma

```mlp
-- Tüm dosyayı oku
String content = read_file("data.txt")
print content

-- Satır satır oku
list[String] lines = read_lines("data.txt")
for line in lines
    print line
end_for
```

### Dosya Yazma

```mlp
-- Dosyaya yaz (üzerine yaz)
write_file("output.txt", "Merhaba MLP")

-- Dosyaya ekle
append_file("log.txt", "Yeni log kaydı\n")
```

### Dosya Kontrolleri

```mlp
-- Dosya var mı?
Boolean exists = file_exists("config.json")

if exists then
    -- Dosya boyutu
    Numeric size = file_size("config.json")
    print "Dosya boyutu: " + str(size) + " byte"
end_if
```

---

## 18. Yorumlar

### Tek Satır Yorum

```mlp
-- Bu bir yorum satırı
Numeric x = 10  -- Satır sonu yorumu
```

### Çok Satırlı Yorum

```mlp
{-
  Bu çok satırlı
  bir yorum bloğudur
  Detaylı açıklamalar için kullanılır
-}

function calculate()
    {- 
       Karmaşık hesaplama yapan
       bir fonksiyon
    -}
    return 42
end_function
```

### Dokümantasyon Yorumu

```mlp
{-
  @function calculate_area
  @description Dikdörtgen alanını hesaplar
  @param width Genişlik (pozitif sayı)
  @param height Yükseklik (pozitif sayı)
  @return Alan değeri
-}
function calculate_area(Numeric width, Numeric height)
    return width * height
end_function
```

---

## 🎯 Tam Program Örneği

```mlp
-- MLP Örnek Program: Öğrenci Not Sistemi

-- Öğrenci yapısı
struct Student
    String name
    Numeric grade
    String status
end_struct

-- Not hesaplama
function calculate_status(Numeric grade)
    if grade >= 90 then
        return "Excellent"
    else if grade >= 70 then
        return "Good"
    else if grade >= 50 then
        return "Pass"
    else
        return "Fail"
    end_if
end_function

-- Ana program
function main()
    print "=== Öğrenci Not Sistemi ==="
    
    -- Öğrenci listesi
    list[Student] students = list()
    
    -- Öğrenci ekleme
    Student s1
    s1.name = "Ali"
    s1.grade = 85
    s1.status = calculate_status(s1.grade)
    students.add(s1)
    
    Student s2
    s2.name = "Ayşe"
    s2.grade = 92
    s2.status = calculate_status(s2.grade)
    students.add(s2)
    
    Student s3
    s3.name = "Mehmet"
    s3.grade = 67
    s3.status = calculate_status(s3.grade)
    students.add(s3)
    
    -- Öğrencileri listele
    print "\nÖğrenci Listesi:"
    for i = 0 to students.size()
        Student s = students.get(i)
        print s.name + ": " + str(s.grade) + " (" + s.status + ")"
    end_for
    
    -- Ortalama hesapla
    Numeric total = 0
    for i = 0 to students.size()
        Student s = students.get(i)
        total = total + s.grade
    end_for
    
    Numeric average = total / students.size()
    print "\nSınıf Ortalaması: " + str(average)
    
    return 0
end_function

-- Programı başlat
main()
```

---

## 📚 Ek Kaynaklar

- **Tam Spesifikasyon:** `SPECS.md`
- **API Referansı:** `API_REFERENCE.md`
- **Örnek Programlar:** `examples/` dizini
- **Dil Desteği:** `diller.json`
- **Syntax Varyantları:** `syntax.json`

---

**© 2025 MLP - Multi-Language Programming**  
**Lisans:** MIT
