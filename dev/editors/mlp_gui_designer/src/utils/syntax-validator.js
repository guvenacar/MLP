// MLP Syntax Validator - Basit Pattern Matching

class SyntaxValidator {
  constructor() {
    this.errors = [];
  }

  // Kodu validate et
  validate(code) {
    this.errors = [];
    const lines = code.split('\n');

    lines.forEach((line, index) => {
      const lineNum = index + 1;
      const trimmed = line.trim();

      // Boş satırları ve yorumları atla
      if (!trimmed || trimmed.startsWith('--') || trimmed.startsWith('//')) {
        return;
      }

      // 1. Eşleşmeyen parantezler
      this.checkBalancedParentheses(line, lineNum);

      // 2. Eşleşmeyen tırnaklar
      this.checkBalancedQuotes(line, lineNum);

      // 3. Geçersiz karakterler
      this.checkInvalidCharacters(line, lineNum);

      // 4. Fonksiyon çağrılarında parantez eksikliği
      this.checkFunctionCalls(line, lineNum);

      // 5. if/while/for yapılarında end eksikliği (context'e bakarak)
      // Bu daha karmaşık, şimdilik basitçe
    });

    return this.errors;
  }

  // Parantez dengesi kontrolü
  checkBalancedParentheses(line, lineNum) {
    let count = 0;
    let inString = false;
    let stringChar = null;

    for (let i = 0; i < line.length; i++) {
      const char = line[i];
      const prevChar = i > 0 ? line[i - 1] : null;

      // String içinde miyiz?
      if ((char === '"' || char === "'") && prevChar !== '\\') {
        if (!inString) {
          inString = true;
          stringChar = char;
        } else if (char === stringChar) {
          inString = false;
          stringChar = null;
        }
      }

      if (!inString) {
        if (char === '(') count++;
        if (char === ')') count--;

        if (count < 0) {
          this.addError(lineNum, line.indexOf(char), 'Fazla kapatma parantezi', 'error');
          return;
        }
      }
    }

    if (count > 0) {
      this.addError(lineNum, line.lastIndexOf('('), 'Eksik kapatma parantezi', 'error');
    }
  }

  // Tırnak dengesi kontrolü
  checkBalancedQuotes(line, lineNum) {
    let doubleQuoteCount = 0;
    let singleQuoteCount = 0;

    for (let i = 0; i < line.length; i++) {
      const char = line[i];
      const prevChar = i > 0 ? line[i - 1] : null;

      if (char === '"' && prevChar !== '\\') {
        doubleQuoteCount++;
      }
      if (char === "'" && prevChar !== '\\') {
        singleQuoteCount++;
      }
    }

    if (doubleQuoteCount % 2 !== 0) {
      this.addError(lineNum, line.indexOf('"'), 'Eşleşmeyen çift tırnak', 'error');
    }
    if (singleQuoteCount % 2 !== 0) {
      this.addError(lineNum, line.indexOf("'"), 'Eşleşmeyen tek tırnak', 'error');
    }
  }

  // Geçersiz karakterler
  checkInvalidCharacters(line, lineNum) {
    // ASCII dışı karakterler (Türkçe hariç)
    const invalidChars = /[^\x00-\x7FçÇğĞıİöÖşŞüÜ]/g;
    const matches = [...line.matchAll(invalidChars)];

    matches.forEach(match => {
      this.addError(lineNum, match.index, `Geçersiz karakter: '${match[0]}'`, 'warning');
    });
  }

  // Fonksiyon çağrılarında parantez kontrolü
  checkFunctionCalls(line, lineNum) {
    // YAZDIR, GUI_PENCERE_AC gibi fonksiyonlar parantez olmadan kullanılmamalı
    const functionPattern = /\b(YAZDIR|GUI_\w+|print|func)\s+[^(]/i;

    if (functionPattern.test(line) && !line.includes('=')) {
      const match = line.match(functionPattern);
      if (match) {
        this.addError(
          lineNum,
          match.index,
          `Fonksiyon çağrısında parantez eksik: ${match[0].trim()}`,
          'warning'
        );
      }
    }
  }

  // Hata ekle
  addError(line, column, message, severity = 'error') {
    this.errors.push({
      line,
      column,
      message,
      severity // 'error', 'warning', 'info'
    });
  }

  // Satıra göre hataları getir
  getErrorsForLine(lineNum) {
    return this.errors.filter(err => err.line === lineNum);
  }

  // Tüm hataları getir
  getAllErrors() {
    return this.errors;
  }

  // Hata var mı?
  hasErrors() {
    return this.errors.some(err => err.severity === 'error');
  }

  // Uyarı var mı?
  hasWarnings() {
    return this.errors.some(err => err.severity === 'warning');
  }
}

// Global instance
const syntaxValidator = new SyntaxValidator();
