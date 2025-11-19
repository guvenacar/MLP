# MLP Language Support for Visual Studio Code

Syntax highlighting extension for **MLP (Multi-Language Programming)** - a revolutionary programming language that supports 6 natural languages!

## Features

✨ **Multi-Language Support**: Write code in your native language
- 🇹🇷 **Turkish** (Türkçe): `SAYISAL x = 42; YAZDIR x;`
- 🇬🇧 **English**: `int x = 42; print x;`
- 🇷🇺 **Russian** (Русский): `целое x = 42; печать x;`
- 🇨🇳 **Chinese** (中文): `整数 x = 42; 打印 x;`
- 🇮🇳 **Hindi** (हिन्दी): `संख्या x = 42; लिखो x;`
- 🎨 **Custom**: Define your own keywords!

🎨 **Comprehensive Syntax Highlighting**:
- Keywords (if/eğer/если/如果/अगर)
- Types (int/sayisal/целое/整数/संख्या)
- Strings, Numbers, Comments
- Operators and Functions
- Boolean constants (true/doğru/истина/真/सत्य)

## Installation

### From VSIX (Recommended)
```bash
cd vscode-mlp
vsce package
code --install-extension mlp-language-1.0.0.vsix
```

### From Source
1. Copy the `vscode-mlp` folder to:
   - **Linux/macOS**: `~/.vscode/extensions/`
   - **Windows**: `%USERPROFILE%\.vscode\extensions\`
2. Restart VS Code
3. Open any `.mlp` file to see syntax highlighting!

## Usage

Create a file with `.mlp` extension and start coding in any supported language:

### Turkish Example (Türkçe)
```mlp
SAYISAL fibonacci(SAYISAL n) {
    EĞER n <= 1 İSE
        DÖNÜŞ n;
    DEĞİLSE
        DÖNÜŞ fibonacci(n-1) + fibonacci(n-2);
    SON
}

YAZDIR fibonacci(10);
```

### English Example
```mlp
int factorial(int n) {
    if n <= 1 then
        return 1;
    else
        return n * factorial(n-1);
    end
}

print factorial(5);
```

### Russian Example (Русский)
```mlp
целое сумма(целое a, целое b) {
    если a > b то
        вернуть a;
    иначе
        вернуть b;
    конец
}

печать сумма(42, 58);
```

## MLP Compiler

This extension works with the MLP compiler. To compile and run:

```bash
./mlpc yourfile.mlp -o yourprogram
./yourprogram
```

## Language Reference

| Concept | English | Turkish | Russian | Chinese | Hindi |
|---------|---------|---------|---------|---------|-------|
| Integer | int | sayisal/SAYISAL | целое | 整数 | संख्या |
| String | string | metin/METIN | строка | 字符串 | पाठ |
| If | if | eğer/EĞER | если | 如果 | अगर |
| Else | else | değilse | иначе | 否则 | नहीं_तो |
| While | while | döngü | пока | 当 | जब_तक |
| Function | function | işleç/fonksiyon | функция | 函数 | फलन |
| Return | return | dönüş/DONUS | вернуть | 返回 | वापसी |
| Print | print | yazdir/YAZDIR | печать | 打印 | लिखो |
| True | true | doğru/DOGRU | истина | 真 | सत्य |
| False | false | yanlış/YANLIS | ложь | 假 | झूठ |

## Project Links

- GitHub: https://github.com/guvenacar/MLP
- Issues: https://github.com/guvenacar/MLP/issues

## License

MIT License - See [LICENSE](https://github.com/guvenacar/MLP/blob/main/LICENSE)

## Author

**Güven Acar** - Creator of MLP Language

---

**Enjoy coding in your native language! 🌍💻**
