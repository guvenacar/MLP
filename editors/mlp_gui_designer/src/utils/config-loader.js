// Konfigürasyon Yükleyici - Diller ve Syntax Stilleri

class ConfigLoader {
  constructor() {
    this.languages = null;
    this.syntaxes = null;
    this.currentLanguage = 'tr-TR'; // Default
    this.currentSyntax = 'mlp-default'; // Default
  }

  // Diller ve syntax dosyalarını yükle
  async loadConfigs() {
    try {
      // diller.json yükle
      const dillerResponse = await fetch('./diller.json');
      const dillerData = await dillerResponse.json();
      this.languages = dillerData.languages;

      // syntax.json yükle
      const syntaxResponse = await fetch('./syntax.json');
      const syntaxData = await syntaxResponse.json();
      this.syntaxes = syntaxData.syntaxes;

      console.log('Configs loaded successfully:', {
        languages: this.languages.length,
        syntaxes: Object.keys(this.syntaxes).length
      });

      return true;
    } catch (error) {
      console.error('Config loading error:', error);
      return false;
    }
  }

  // Mevcut dilleri getir
  getLanguages() {
    return this.languages || [];
  }

  // Mevcut syntax stillerini getir
  getSyntaxes() {
    return this.syntaxes || {};
  }

  // Dil seç
  setLanguage(langId) {
    const lang = this.languages?.find(l => l.id === langId);
    if (lang) {
      this.currentLanguage = langId;
      return true;
    }
    return false;
  }

  // Syntax seç
  setSyntax(syntaxId) {
    if (this.syntaxes && this.syntaxes[syntaxId]) {
      this.currentSyntax = syntaxId;
      return true;
    }
    return false;
  }

  // Seçili dili getir
  getCurrentLanguage() {
    return this.languages?.find(l => l.id === this.currentLanguage);
  }

  // Seçili syntax'ı getir
  getCurrentSyntax() {
    return this.syntaxes?.[this.currentSyntax];
  }

  // Keyword çevir (İngilizce'den seçili dile)
  translateKeyword(englishKeyword) {
    const lang = this.getCurrentLanguage();
    if (!lang || lang.id === 'en-US') {
      return englishKeyword; // English, çevirme
    }

    // İlgili keyword'ün karşılığını bul
    for (const [key, values] of Object.entries(lang.keywords)) {
      if (key === englishKeyword.toLowerCase()) {
        return values[0]; // İlk eşleşmeyi kullan
      }
    }

    return englishKeyword; // Bulunamadıysa orijinali döndür
  }

  // Syntax marker'ları getir
  getSyntaxMarkers() {
    const syntax = this.getCurrentSyntax();
    return syntax?.markers || {};
  }

  // Syntax block style'ı getir
  getBlockStyle() {
    const syntax = this.getCurrentSyntax();
    return syntax?.block_style || 'keyword';
  }

  // Noktalı virgül gerekli mi?
  requiresSemicolons() {
    const syntax = this.getCurrentSyntax();
    return syntax?.requires_semicolons || false;
  }

  // Girinti hassas mı?
  isIndentSensitive() {
    const syntax = this.getCurrentSyntax();
    return syntax?.indent_sensitive || false;
  }
}

// Global instance
const configLoader = new ConfigLoader();
