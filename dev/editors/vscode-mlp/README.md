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
npm install -g vsce
vsce package
code --install-extension mlp-language-1.1.0.vsix
```

### From Source
1. Copy the `vscode-mlp` folder to:
   - **Linux/macOS**: `~/.vscode/extensions/`
   - **Windows**: `%USERPROFILE%\.vscode\extensions\`
2. Restart VS Code
3. Open any `.mlp` file to see syntax highlighting!

## ✨ What's New in v1.1.0

- 🎯 **Code Snippets**: 15+ ready-to-use snippets for faster coding
- 📦 **Project Templates**: 5 starter templates (Hello World, Calculator, Fibonacci, List Demo, File I/O)
- 📚 **Quick Start Guide**: Complete guide for starting new MLP projects
- 🚀 **Improved Workflow**: Better developer experience with auto-complete

## Usage

### Quick Start
1. Create a new folder for your project
2. Open in VS Code: `code my-mlp-project`
3. Create a new file: `main.mlp`
4. Type `mlp-hello` and press `Tab` for Hello World template
5. Or explore other snippets (see below)

### Code Snippets

Type these prefixes and press `Tab`:

| Prefix | Description |
|--------|-------------|
| `mlp-hello` | Hello World program |
| `mlp-main` | Main function |
| `mlp-func` | Function definition |
| `mlp-if` | If statement |
| `mlp-ifelse` | If-else statement |
| `mlp-while` | While loop |
| `mlp-for` | For loop |
| `mlp-list` | Create List<T> |
| `mlp-struct` | Struct definition |
| `mlp-enum` | Enum definition |
| `mlp-switch` | Switch statement |
| `mlp-fileread` | Read file |
| `mlp-filewrite` | Write file |
| `mlp-tr-hello` | Turkish Hello World |
| `mlp-tr-func` | Turkish function |

### Project Templates

Ready-to-use templates in `templates/` folder:
- `hello-world.mlp` - Simple starter
- `calculator.mlp` - Calculator with functions
- `fibonacci.mlp` - Fibonacci sequence
- `list-demo.mlp` - List<T> operations
- `file-demo.mlp` - File I/O examples

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
