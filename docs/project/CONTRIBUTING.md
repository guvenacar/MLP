# Contributing to MLP (Multi-Language Programming)

Thank you for your interest in contributing to MLP! 🎉

This document provides guidelines for contributing to the project, whether you're fixing bugs, adding features, or improving documentation.

---

## 📋 Table of Contents

1. [Code of Conduct](#code-of-conduct)
2. [Getting Started](#getting-started)
3. [How to Contribute](#how-to-contribute)
4. [Development Process](#development-process)
5. [Coding Standards](#coding-standards)
6. [Language Support](#language-support)
7. [Testing Guidelines](#testing-guidelines)
8. [Documentation](#documentation)
9. [Commit Guidelines](#commit-guidelines)
10. [Pull Request Process](#pull-request-process)

---

## 📜 Code of Conduct

### Our Pledge

We are committed to making MLP a welcoming project for everyone, regardless of:
- Programming experience level
- Native language
- Age, gender, or nationality
- Technical background

### Expected Behavior

✅ **DO:**
- Be respectful and inclusive
- Welcome newcomers
- Give constructive feedback
- Help others learn
- Celebrate multilingual contributions

❌ **DON'T:**
- Use offensive language
- Harass or discriminate
- Dismiss non-English contributions
- Make assumptions about skill level

---

## 🚀 Getting Started

### Prerequisites

**For Contributors:**
```bash
# Linux/macOS required
# Windows: Use WSL2

# Required tools:
sudo apt install build-essential   # GCC, Make
sudo apt install nasm              # Assembler
sudo apt install python3           # Preprocessor
```

**For GUI Development (Optional):**
```bash
sudo apt install libsdl2-dev       # SDL2 library
```

### Clone & Build

```bash
# 1. Fork the repository on GitHub
# 2. Clone your fork
git clone https://github.com/YOUR_USERNAME/MLP.git
cd MLP

# 3. Build the compiler
cd c_compiler
make
cd ..

# 4. Test it works
./mlpc test.mlp -o test
./test
```

---

## 🤝 How to Contribute

### Areas of Contribution

| Area | Difficulty | Skills Needed |
|------|-----------|---------------|
| **Bug Fixes** | 🟢 Easy | Basic C/Python |
| **New Language Support** | 🟢 Easy | JSON editing |
| **Documentation** | 🟢 Easy | Writing |
| **Test Cases** | 🟡 Medium | MLP syntax |
| **Compiler Features** | 🔴 Hard | C, Assembly |
| **Runtime Library** | 🔴 Hard | C, Syscalls |

### Good First Issues

Look for issues tagged with:
- `good-first-issue` 👶 - Perfect for newcomers
- `easy` 🟢 - 1-2 hours work
- `documentation` 📖 - No coding required
- `help-wanted` 🙋 - Maintainers need help

---

## 🔧 Development Process

### Workflow

```
1. Find an issue (or create one)
   ↓
2. Comment: "I'd like to work on this"
   ↓
3. Wait for assignment/approval
   ↓
4. Fork + Create branch
   ↓
5. Make changes + Test
   ↓
6. Create Pull Request
   ↓
7. Address review comments
   ↓
8. Merge! 🎉
```

### Branch Naming

```bash
# Feature
git checkout -b feature/array-support

# Bug fix
git checkout -b fix/parser-segfault

# Language addition
git checkout -b lang/arabic-support

# Documentation
git checkout -b docs/update-readme
```

---

## 📐 Coding Standards

### C Code (Compiler/Runtime)

**Style:**
```c
// ✅ GOOD: Clear naming, comments
typedef struct {
    TokenType type;
    char* value;
    int line;
} Token;

Token* lexer_next_token(Lexer* lexer) {
    // Skip whitespace
    while (is_whitespace(lexer->current_char)) {
        lexer_advance(lexer);
    }
    
    // Handle identifiers
    if (is_alpha(lexer->current_char)) {
        return lexer_read_identifier(lexer);
    }
    
    return NULL;
}

// ❌ BAD: No comments, unclear names
Token* gnt(L* l) {
    while(iw(l->c))la(l);
    if(ia(l->c))return ri(l);
    return 0;
}
```

**Rules:**
- Use descriptive names
- Add comments for complex logic
- Keep functions small (<100 lines)
- Handle errors gracefully
- Free allocated memory

### Python Code (Preprocessor)

**Style:**
```python
# ✅ GOOD: Type hints, docstrings
def translate_keyword(word: str, language_map: dict) -> str:
    """
    Translate a keyword from any language to English.
    
    Args:
        word: The keyword to translate
        language_map: Language definition from diller.json
    
    Returns:
        English keyword or original word if not found
    """
    for english_kw, translations in language_map.items():
        if word in translations:
            return english_kw
    return word

# ❌ BAD: No documentation, unclear logic
def t(w,m):
    for k,v in m.items():
        if w in v:return k
    return w
```

### MLP Code (Examples/Tests)

**Style:**
```mlp
-- ✅ GOOD: Clear, commented

-- lang: tr-TR
-- Faktöriyel hesaplama fonksiyonu

İŞLEÇ faktoriyel(n) İSE
    -- Base case: 0! = 1, 1! = 1
    EĞER n <= 1 İSE
        DÖNÜŞ 1
    SON
    
    -- Recursive case: n! = n * (n-1)!
    DÖNÜŞ n * faktoriyel(n - 1)
SON

-- Test
SAYISAL sonuc = faktoriyel(5);
YAZDIR sonuc  -- Beklenen: 120


-- ❌ BAD: No comments, unclear purpose

İŞLEÇ f(n) İSE
    EĞER n <= 1 İSE
        DÖNÜŞ 1
    SON
    DÖNÜŞ n * f(n - 1)
SON
YAZDIR f(5)
```

---

## 🌍 Language Support

### Adding a New Language

**Easy! Just 3 steps:**

#### Step 1: Edit `diller.json`

```json
{
  "languages": [
    {
      "id": "ar-SA",
      "name": "العربية (Arabic)",
      "description": "Arabic programming keywords",
      "keywords": {
        "int": ["عدد"],
        "string": ["نص"],
        "if": ["إذا"],
        "then": ["فإن"],
        "else": ["وإلا"],
        "end": ["نهاية"],
        "while": ["بينما"],
        "break": ["توقف"],
        "function": ["دالة"],
        "return": ["ارجع"],
        "print": ["اطبع"],
        "true": ["صحيح"],
        "false": ["خطأ"]
      }
    }
  ]
}
```

#### Step 2: Create Test File

```mlp
-- lang: ar-SA
-- Test: Arabic language support

عدد x = 42;
نص message = "مرحبا بالعالم";

اطبع message
اطبع x

دالة جمع(a, b) فإن
    ارجع a + b
نهاية

عدد result = جمع(10, 20);
اطبع result
```

#### Step 3: Test It!

```bash
./mlpc test_arabic.mlp -o test_ar
./test_ar
```

**That's it!** 🎉

### Language Guidelines

**When adding keywords:**
- ✅ Use native script (Arabic: عدد, not 'adad)
- ✅ Choose natural words (common usage)
- ✅ Add variants (uppercase/lowercase)
- ✅ Test all keywords
- ✅ Add multiple variants if needed

**Example (Turkish):**
```json
"if": ["eğer", "EĞER", "eger", "EGER"]
```
Supports: Turkish keyboard (ğ) + ASCII (g) + Case variants

---

## 🧪 Testing Guidelines

### Test Categories

1. **Unit Tests** (Compiler Components)
2. **Integration Tests** (Full Pipeline)
3. **Language Tests** (Multi-language)
4. **Regression Tests** (Bug fixes)

### Writing Tests

**Example Test File:**

```mlp
-- lang: en-US
-- Test: Integer arithmetic

int a = 10;
int b = 20;
int sum = a + b;
int product = a * b;

print sum       -- Expected: 30
print product   -- Expected: 200
```

**Test Checklist:**
```markdown
- [ ] Test compiles successfully
- [ ] Output matches expected
- [ ] Works in all supported languages
- [ ] No memory leaks (valgrind)
- [ ] Handles edge cases
```

### Running Tests

```bash
# Run all tests
make test

# Run specific test
./mlpc test_arithmetic.mlp -o test
./test

# Memory check
valgrind ./test
```

---

## 📖 Documentation

### What to Document

**Code:**
- Complex algorithms
- Public functions/APIs
- Non-obvious design decisions

**Examples:**
```c
// ✅ GOOD: Explains WHY
// We use a state machine here to correctly handle strings and comments
// during preprocessing. Without states, we'd incorrectly translate keywords
// inside string literals (e.g., "EĞER" → "if")
typedef enum {
    STATE_CODE,     // Normal code - translate keywords
    STATE_STRING,   // Inside "..." - preserve as-is
    STATE_COMMENT   // Inside comment - preserve as-is
} PreprocessorState;
```

**README Updates:**
- New features
- Changed behavior
- Breaking changes
- New languages

**SPECS.md Updates:**
- Syntax changes
- New keywords
- Type system changes
- **IMPORTANT:** Get approval first!

---

## 📝 Commit Guidelines

### Format

```
<type>(<scope>): <subject>

<body>

<footer>
```

### Types

| Type | When to Use | Example |
|------|-------------|---------|
| `feat` | New feature | `feat(lang): Add Arabic support` |
| `fix` | Bug fix | `fix(parser): Handle empty files` |
| `docs` | Documentation | `docs(readme): Update examples` |
| `style` | Formatting | `style(lexer): Fix indentation` |
| `refactor` | Code restructure | `refactor(parser): Extract function` |
| `test` | Add tests | `test(runtime): Add string tests` |
| `chore` | Maintenance | `chore(build): Update Makefile` |

### Examples

**Good commits:**
```bash
feat(lang): Add Spanish language support

- Added Spanish keywords to diller.json
- Created test_spanish.mlp test case
- Updated README.md with Spanish example

Closes #42

---

fix(parser): Fix segfault on empty function

Parser was dereferencing NULL when function had no body.
Added NULL check before accessing function->body.

Fixes #38

---

docs(specs): Clarify semicolon usage rules

Added examples showing correct/incorrect semicolon usage
to address confusion from issue #35.
```

**Bad commits:**
```bash
❌ "fix"
❌ "update code"
❌ "çalışıyor artık"
❌ "asdfasdf"
```

---

## 🔀 Pull Request Process

### Before Opening PR

```markdown
✅ Checklist:
- [ ] Code compiles without warnings
- [ ] All tests pass
- [ ] New tests added (if applicable)
- [ ] Documentation updated
- [ ] SPECS.md unchanged (or approved)
- [ ] Follows coding standards
- [ ] Commit messages are clear
```

### PR Template

```markdown
## Description
Brief description of changes (1-2 sentences)

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Breaking change
- [ ] Documentation update

## Changes Made
- Change 1
- Change 2
- Change 3

## Testing
How was this tested?

```bash
# Commands to test
./mlpc test.mlp -o test
./test
```

## Screenshots/Output (if applicable)
```
Output from running the test
```

## Checklist
- [ ] Code compiles
- [ ] Tests pass
- [ ] Documentation updated
- [ ] Follows style guide
```

### Review Process

1. **Automated Checks**
   - CI/CD runs tests
   - Code quality checks
   - Build verification

2. **Maintainer Review**
   - Code quality
   - Design decisions
   - Tests coverage

3. **Address Feedback**
   - Make requested changes
   - Push updates
   - Re-request review

4. **Merge**
   - Maintainer merges PR
   - Branch deleted
   - Issue closed

---

## 🎯 Development Guidelines

### SPECS.md is Sacred

**SPECS.md defines the language!**

❌ **Never change SPECS.md without approval:**
- Syntax rules
- Keyword definitions
- Type system
- Operator precedence

✅ **OK to change:**
- Typos
- Clarifications
- Examples

**Process for SPECS changes:**
1. Open issue discussing the change
2. Get maintainer approval
3. Update SPECS.md
4. Update all affected code
5. Update all tests
6. Update documentation

### Data Types

**MLP's actual types:**

| User-Facing | Internal (Compiler) | Description |
|-------------|---------------------|-------------|
| SAYISAL | `int` | BigDecimal (unlimited precision) |
| SÖZEL | `string` | BigString (unlimited length) |
| ZITLIK | `bool` | Boolean |
| HİÇLİK | `null` (future) | Null value |

**When talking about types:**
- To users → Use language-specific terms (SAYISAL, целое, 整数)
- In code → Use internal terms (int, string, bool)
- In docs → Explain both!

### Semicolon Rules

**Only use semicolons for declarations:**

```mlp
✅ CORRECT:
SAYISAL x;              -- Declaration
SAYISAL y = 10;         -- Initialization
x = 20                  -- Assignment (NO semicolon)
DÖNÜŞ x + y             -- Return (NO semicolon)
SON                     -- Block end (NO semicolon)

❌ WRONG:
x = 20;                 -- NO semicolon on assignment!
DÖNÜŞ x + y;            -- NO semicolon on return!
SON;                    -- NO semicolon on end!
```

---

## 🌟 Special Contributions

### Multi-Language Contributions

**We celebrate contributions in any language!**

- 🇹🇷 Turkish PR? Harika! (Great!)
- 🇷🇺 Russian PR? Отлично! (Excellent!)
- 🇨🇳 Chinese PR? 太好了! (Wonderful!)
- 🇮🇳 Hindi PR? बहुत बढ़िया! (Very good!)

**Guidelines:**
- PR title/description can be in English or native language
- Code comments should be in language being tested
- Commit messages preferably in English (for git history)

### Documentation Translations

**Want to translate README/docs to your language?**

1. Create `README.[lang].md` (e.g., `README.tr.md`)
2. Translate content
3. Update main README with link
4. Open PR!

---

## 💡 Tips for Success

### For New Contributors

1. **Start Small**
   - Fix typos
   - Add language support
   - Improve examples

2. **Ask Questions**
   - No question is too simple
   - Use GitHub Discussions
   - Check existing issues

3. **Be Patient**
   - Review takes time
   - Feedback helps you learn
   - Iteration is normal

### For Experienced Contributors

1. **Mentor Others**
   - Review PRs
   - Answer questions
   - Guide newcomers

2. **Plan Big Changes**
   - Open issue first
   - Discuss design
   - Get consensus

3. **Test Thoroughly**
   - Edge cases
   - Performance
   - Cross-language

---

## 📞 Getting Help

### Where to Ask

| Question Type | Where to Ask |
|---------------|--------------|
| How do I... | [GitHub Discussions](https://github.com/guvenacar/MLP/discussions) |
| Bug report | [GitHub Issues](https://github.com/guvenacar/MLP/issues) |
| Feature idea | [GitHub Discussions](https://github.com/guvenacar/MLP/discussions) |
| PR help | Comment on PR |
| Private matter | Email: guven.acar@gmail.com |

### Response Time

- Simple questions: 1-2 days
- Bug reports: 2-3 days
- PR reviews: 3-5 days
- Feature discussions: 1 week

*We're a small team, please be patient!*

---

## 🎉 Recognition

### Contributors

All contributors are listed in:
- GitHub Contributors page
- README.md (for major contributions)
- Release notes

### Types of Recognition

- 🏆 **Core Contributor** - 10+ merged PRs
- 🌟 **Language Champion** - Added new language
- 📖 **Documentation Hero** - Improved docs significantly
- 🐛 **Bug Hunter** - Found/fixed critical bugs
- 🎨 **Design Contributor** - UI/UX improvements

---

## 📜 License

By contributing to MLP, you agree that your contributions will be licensed under the MIT License.

---

## 🙏 Thank You!

Every contribution matters, no matter how small!

- Fixed a typo? **Thank you!** 📝
- Added a language? **Amazing!** 🌍
- Reported a bug? **Helpful!** 🐛
- Improved docs? **Awesome!** 📖
- Added a feature? **Incredible!** 🚀

**Together, we're making programming accessible in every language!** 🌟

---

**Questions?** Open a [Discussion](https://github.com/guvenacar/MLP/discussions)  
**Found a bug?** Open an [Issue](https://github.com/guvenacar/MLP/issues)  
**Ready to contribute?** Open a [Pull Request](https://github.com/guvenacar/MLP/pulls)  

**Let's code in every language! 🌍**
