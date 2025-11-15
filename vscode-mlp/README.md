# MLP Language Support for VSCode

Syntax highlighting and language support for MLP (Multi-Language Programming).

## Features

- **Syntax Highlighting**: Full syntax highlighting for MLP code
- **Code Folding**: Support for code folding
- **Auto-closing**: Auto-closing brackets, parentheses, and quotes
- **Comment Support**: Line comments (`--`) and block comments (`--[[ ]]`)
- **Multi-Syntax Support**: Highlights both English and Turkish keywords

## Supported Keywords

### English Syntax
- `class`, `constructor`, `method`, `override`, `end`
- `if`, `else`, `elsif`, `while`, `for`, `return`
- `var`, `this`, `new`
- `string`, `number`, `bool`, `dynamic`, `dict`, `array`
- `true`, `false`, `null`

### Turkish Syntax (Native MLP)
- `SINIF`, `KURUCU`, `IŞLEÇ`, `OVERRIDE`, `SON`
- `EĞER`, `İSE`, `DEGILSE`, `YOKSA`, `DONGU`, `DÖNÜŞ`
- `DEĞIŞKEN`, `BU`, `YENİ`
- `METIN`, `SAYISAL`, `BOOL`, `DİNAMİK`, `SÖZLÜK`, `DİZİ`
- `DOĞRU`, `YANLIŞ`, `HİÇLİK`
- `KULLAN` (import), `YAZDIR` (print)

## Installation

### From VSIX
1. Download the `.vsix` file
2. Open VSCode
3. Go to Extensions (Ctrl+Shift+X)
4. Click "..." menu → "Install from VSIX..."
5. Select the downloaded `.vsix` file

### From Source
1. Clone the MLP repository
2. Navigate to `vscode-mlp/`
3. Run `npm install`
4. Run `vsce package` to create the `.vsix` file
5. Install the `.vsix` file in VSCode

## Building

```bash
cd vscode-mlp
npm install
npm run package
```

This will create a `.vsix` file that can be installed in VSCode.

## Usage

Once installed, any file with the `.mlp` extension will automatically use MLP syntax highlighting.

## Example Code

```mlp
-- MLP Example
SINIF Kişi
    METIN isim;
    SAYISAL yaş;

    KURUCU(METIN isim, SAYISAL yaş)
        BU.isim = isim;
        BU.yaş = yaş;
    end

    IŞLEÇ selamla()
        YAZDIR "Merhaba, ben " + BU.isim;
    end
end
```

## License

MIT License - See main MLP repository for details.

## Contributing

Contributions are welcome! Please see the main MLP repository.

## Links

- [MLP GitHub Repository](https://github.com/guvenacar/MLP)
- [MLP Documentation](https://github.com/guvenacar/MLP/tree/main/docs)
