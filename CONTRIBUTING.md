# Contributing to MLP

First off, thank you for considering contributing to MLP! It's people like you that make MLP such a great project.

## Code of Conduct

This project and everyone participating in it is governed by our Code of Conduct. By participating, you are expected to uphold this code.

## How Can I Contribute?

### Reporting Bugs

Before creating bug reports, please check the existing issues as you might find out that you don't need to create one. When you are creating a bug report, please include as many details as possible:

* **Use a clear and descriptive title**
* **Describe the exact steps to reproduce the problem**
* **Provide specific examples**
* **Describe the behavior you observed and what behavior you expected to see**
* **Include code samples** (use Markdown code blocks)
* **Include your environment** (OS, MLP version, etc.)

### Suggesting Enhancements

Enhancement suggestions are tracked as GitHub issues. When creating an enhancement suggestion, please include:

* **Use a clear and descriptive title**
* **Provide a step-by-step description of the suggested enhancement**
* **Provide specific examples to demonstrate the enhancement**
* **Describe the current behavior** and **explain the desired behavior**
* **Explain why this enhancement would be useful**

### Pull Requests

* **Follow the coding style** used in the project
* **Write clear commit messages** following [Conventional Commits](https://www.conventionalcommits.org/)
* **Include tests** for new features
* **Update documentation** as needed
* **Ensure all tests pass** before submitting

## Development Setup

### Prerequisites

* GCC or Clang compiler
* Python 3.8+ (for bootstrap tools)
* Git

### Setup

```bash
# Clone your fork
git clone https://github.com/yourusername/MLP.git
cd MLP

# Add upstream remote
git remote add upstream https://github.com/guvenacar/MLP.git

# Install dependencies
./setup.sh

# Build
./build.sh

# Run tests
./test.sh
```

### Project Structure

```
MLP/
├── src/              # MLP compiler source code (written in MLP)
├── runtime/          # C runtime library
├── stdlib/           # Standard library (MLP)
├── tests/            # Test suite
├── docs/             # Documentation
└── examples/         # Example programs
```

## Coding Style

### MLP Code Style

```mlp
-- Use descriptive names
SINIF Person
    ÖZEL METIN name;

    -- Comments should explain "why", not "what"
    KURUCU(METIN name)
        BU.name = name;
    KURUCU SON

    -- Keep functions short and focused
    IŞLEÇ greet()
        YAZDIR "Hello, " + BU.name;
    IŞLEÇ SON
SINIF SON
```

### C Code Style (Runtime)

```c
/* Use K&R style for C runtime code */
void mlp_print(const char* str) {
    printf("%s\n", str);
}
```

### Commit Message Format

We follow [Conventional Commits](https://www.conventionalcommits.org/):

```
<type>(<scope>): <subject>

<body>

<footer>
```

**Types:**
* `feat`: New feature
* `fix`: Bug fix
* `docs`: Documentation only changes
* `style`: Code style changes (formatting, etc.)
* `refactor`: Code refactoring
* `test`: Adding or updating tests
* `chore`: Maintenance tasks

**Examples:**
```
feat(lexer): Add support for unicode identifiers

Implements UTF-8 support in the lexer to allow Unicode characters
in variable names and comments.

Closes #123
```

```
fix(parser): Correct operator precedence for power operator

The ** (power) operator was incorrectly parsed with lower precedence
than multiplication. This fix ensures it has higher precedence.

Fixes #456
```

## Testing

### Running Tests

```bash
# Run all tests
./test.sh

# Run specific test suite
./test.sh unit
./test.sh integration
./test.sh benchmark

# Run a specific test file
python -m pytest tests/unit/test_lexer.py -v
```

### Writing Tests

Tests should be placed in the appropriate directory under `tests/`:

```mlp
-- tests/unit/test_lexer.mlp
KULLAN testing;

@test
IŞLEÇ test_tokenize_number()
    DİZİ tokens = tokenize("123");
    ASSERT_EQUAL(tokens[0].type, "NUMBER");
    ASSERT_EQUAL(tokens[0].value, "123");
IŞLEÇ SON

@test
IŞLEÇ test_tokenize_keyword()
    DİZİ tokens = tokenize("SAYISAL");
    ASSERT_EQUAL(tokens[0].type, "KEYWORD");
    ASSERT_EQUAL(tokens[0].value, "SAYISAL");
IŞLEÇ SON
```

## Documentation

### Code Documentation

```mlp
--[[
 Calculate the factorial of a number recursively.

 @param n The number to calculate factorial for
 @return The factorial of n
 @throws Error if n is negative
]]--
IŞLEÇ factorial(SAYISAL n) -> SAYISAL
    EĞER n < 0 İSE
        HATA_FIRLATIR "Negative number";
    EĞER SON

    EĞER n <= 1 İSE
        DÖNÜŞ 1;
    EĞER SON

    DÖNÜŞ n * factorial(n - 1);
IŞLEÇ SON
```

### Updating Documentation

When adding new features, please update:

* `README.md` - If it's a major feature
* `docs/LANGUAGE_REFERENCE.md` - For language features
* `CHANGELOG.md` - For all user-facing changes
* Inline code comments
* Example programs in `examples/`

## Branch Naming

Use descriptive branch names:

* `feature/add-generics`
* `fix/parser-crash-on-empty-file`
* `docs/update-readme`
* `refactor/simplify-lexer`

## Review Process

1. All submissions require review
2. We aim to review PRs within 48 hours
3. Address review comments promptly
4. Once approved, maintainers will merge your PR

## Community

* **Discord**: [MLP Community](https://discord.gg/mlp-lang)
* **Forum**: [forum.mlp-lang.org](https://forum.mlp-lang.org)
* **Email**: guven.acar@gmail.com

## Recognition

Contributors will be listed in:
* `CONTRIBUTORS.md`
* Release notes
* Project website

## Questions?

Feel free to ask questions in:
* GitHub Discussions
* Discord server
* Email to maintainers

---

**Thank you for contributing to MLP!** 🚀
