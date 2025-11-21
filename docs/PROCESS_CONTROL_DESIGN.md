# Process Control - Design Specification

**Status**: Phase 5.3 - In Progress
**Version**: 1.0
**Last Updated**: November 21, 2025

---

## Overview

This document specifies process control functions for MLP, enabling programs to execute external commands, capture output, and manage process information.

### Goals
- Execute shell commands and get exit codes
- Capture command output (stdout)
- Access process identification
- Enable build system automation

### Non-Goals
- Process spawning (fork/exec) - future work
- Signal handling - future work
- Pipe communication - future work
- Background processes - future work

---

## 1. Process Control Functions

### 1.1 execute_command()

**Purpose**: Run shell command and return exit code

**Signature**:
```mlp
function execute_command(cmd) then
    -- Executes command, returns exit code
end
```

**Parameters**:
- `cmd` (string): Shell command to execute

**Return Value**:
- Exit code of command (0-255)
- `-1` on execution failure

**Behavior**:
- Executes command through system shell (/bin/sh)
- Waits for command to complete
- Returns exit code from command
- Inherits stdin/stdout/stderr from parent

**Example**:
```mlp
function compile_assembly(source, output) then
    string cmd = "nasm -f elf64 "
    cmd = string_concat(cmd, source)
    cmd = string_concat(cmd, " -o ")
    cmd = string_concat(cmd, output)

    int result = execute_command(cmd)
    if result != 0 then
        print "Error: Assembly compilation failed"
        return 0
    end

    print "Assembly compiled successfully"
    return 1
end
```

**Common Exit Codes**:
- `0` - Success
- `1` - General error
- `2` - Misuse of shell command
- `126` - Command invoked cannot execute
- `127` - Command not found
- `128+n` - Fatal error signal n

**Security Warning**:
- Command is passed to shell - vulnerable to injection
- Always validate/sanitize user input before using in commands
- Use quoting for file paths with spaces

---

### 1.2 get_command_output()

**Purpose**: Run command and capture stdout

**Signature**:
```mlp
function get_command_output(cmd) then
    -- Executes command, returns stdout as string
end
```

**Parameters**:
- `cmd` (string): Shell command to execute

**Return Value**:
- String containing stdout output
- Empty string on failure

**Behavior**:
- Executes command through popen()
- Captures stdout (not stderr)
- Returns output as string (newlines preserved)
- Exit code available via `get_error_code()`

**Example**:
```mlp
function get_git_branch() then
    string output = get_command_output("git branch --show-current")

    if string_length(output) == 0 then
        print "Not a git repository"
        return ""
    end

    -- Remove trailing newline
    string branch = string_trim(output)
    return branch
end

function show_git_status() then
    string branch = get_git_branch()
    if string_length(branch) > 0 then
        print "Current branch: "
        print branch
    end

    string status = get_command_output("git status --short")
    if string_length(status) > 0 then
        print "Changes:"
        print status
    else
        print "Working directory clean"
    end
end
```

**Output Handling**:
- Output is limited to 1MB by default
- Binary output is not supported (null bytes truncate)
- Stderr is not captured (goes to parent's stderr)

---

### 1.3 get_process_id()

**Purpose**: Get current process ID

**Signature**:
```mlp
function get_process_id() then
    -- Returns PID of current process
end
```

**Parameters**: None

**Return Value**:
- Process ID (positive integer)

**Behavior**:
- Returns result of getpid()
- Always succeeds

**Example**:
```mlp
function create_temp_filename(prefix) then
    int pid = get_process_id()
    string pid_str = int_to_string(pid)

    string filename = string_concat(prefix, "_")
    filename = string_concat(filename, pid_str)
    filename = string_concat(filename, ".tmp")

    return filename
end

function log_with_pid(message) then
    int pid = get_process_id()
    print "["
    print pid
    print "] "
    print message
end
```

---

### 1.4 get_parent_process_id()

**Purpose**: Get parent process ID

**Signature**:
```mlp
function get_parent_process_id() then
    -- Returns PPID of current process
end
```

**Parameters**: None

**Return Value**:
- Parent process ID (positive integer)

**Behavior**:
- Returns result of getppid()
- Returns 1 if parent has exited (adopted by init)

**Example**:
```mlp
function check_interactive() then
    int ppid = get_parent_process_id()

    -- Check if running from shell (simplified)
    print "Parent PID: "
    print ppid
end

function is_daemon() then
    int ppid = get_parent_process_id()
    -- Process adopted by init (PID 1) is likely a daemon
    if ppid == 1 then
        return 1
    end
    return 0
end
```

---

## 2. Implementation Details

### 2.1 Runtime Implementation (C)

```c
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

// execute_command - Run shell command
int64_t mlp_execute_command(const char* cmd) {
    if (!cmd || !*cmd) {
        mlp_set_error_code(1);
        return -1;
    }

    int status = system(cmd);

    if (status == -1) {
        mlp_set_error_code(2);  // Fork failed
        return -1;
    }

    if (WIFEXITED(status)) {
        int exit_code = WEXITSTATUS(status);
        mlp_set_error_code(exit_code);
        return exit_code;
    }

    if (WIFSIGNALED(status)) {
        int signal = WTERMSIG(status);
        mlp_set_error_code(128 + signal);
        return 128 + signal;
    }

    mlp_set_error_code(0);
    return 0;
}

// get_command_output - Run command and capture stdout
const char* mlp_get_command_output(const char* cmd) {
    static char buffer[1048576];  // 1MB max output
    buffer[0] = '\0';

    if (!cmd || !*cmd) {
        mlp_set_error_code(1);
        return buffer;
    }

    FILE* pipe = popen(cmd, "r");
    if (!pipe) {
        mlp_set_error_code(2);  // popen failed
        return buffer;
    }

    size_t total = 0;
    size_t read;
    char temp[4096];

    while ((read = fread(temp, 1, sizeof(temp) - 1, pipe)) > 0) {
        if (total + read >= sizeof(buffer) - 1) {
            // Truncate output
            read = sizeof(buffer) - 1 - total;
        }
        memcpy(buffer + total, temp, read);
        total += read;
        if (total >= sizeof(buffer) - 1) break;
    }
    buffer[total] = '\0';

    int status = pclose(pipe);
    if (WIFEXITED(status)) {
        mlp_set_error_code(WEXITSTATUS(status));
    } else {
        mlp_set_error_code(1);
    }

    return buffer;
}

// get_process_id - Get current PID
int64_t mlp_get_process_id(void) {
    return (int64_t)getpid();
}

// get_parent_process_id - Get parent PID
int64_t mlp_get_parent_process_id(void) {
    return (int64_t)getppid();
}
```

### 2.2 Compiler Integration

**Lexer Tokens**:
```c
TOKEN_EXECUTE_COMMAND,        // execute_command
TOKEN_GET_COMMAND_OUTPUT,     // get_command_output
TOKEN_GET_PROCESS_ID,         // get_process_id
TOKEN_GET_PARENT_PROCESS_ID,  // get_parent_process_id
```

**Code Generation**:
```asm
; execute_command(cmd)
mov rdi, [cmd_ptr]
call mlp_execute_command
; exit code in rax

; get_command_output(cmd)
mov rdi, [cmd_ptr]
call mlp_get_command_output
; string pointer in rax

; get_process_id()
call mlp_get_process_id
; pid in rax
```

---

## 3. Use Cases

### 3.1 Build System Integration

```mlp
function build_project() then
    print "Building project..."
    int pid = get_process_id()
    print "[PID: "
    print pid
    print "]"

    -- Compile assembly
    int result = execute_command("nasm -f elf64 output.asm -o output.o")
    if result != 0 then
        print "NASM failed with exit code: "
        print result
        return 0
    end

    -- Link
    result = execute_command("gcc -no-pie output.o runtime.o -o program")
    if result != 0 then
        print "Linking failed with exit code: "
        print result
        return 0
    end

    print "Build successful!"
    return 1
end
```

### 3.2 Git Integration

```mlp
function git_commit(message) then
    -- Check for changes
    string status = get_command_output("git status --porcelain")
    if string_length(status) == 0 then
        print "Nothing to commit"
        return 0
    end

    -- Stage all changes
    int result = execute_command("git add -A")
    if result != 0 then
        print "git add failed"
        return 0
    end

    -- Commit with message
    string cmd = "git commit -m \""
    cmd = string_concat(cmd, message)
    cmd = string_concat(cmd, "\"")

    result = execute_command(cmd)
    if result != 0 then
        print "git commit failed"
        return 0
    end

    print "Committed: "
    print message
    return 1
end
```

### 3.3 System Information

```mlp
function show_system_info() then
    print "=== System Information ==="

    -- Hostname
    string hostname = get_command_output("hostname")
    print "Hostname: "
    print string_trim(hostname)

    -- Kernel
    string kernel = get_command_output("uname -r")
    print "Kernel: "
    print string_trim(kernel)

    -- Current user
    string user = get_command_output("whoami")
    print "User: "
    print string_trim(user)

    -- Process info
    print "PID: "
    print get_process_id()
    print "Parent PID: "
    print get_parent_process_id()
end
```

### 3.4 External Tool Invocation

```mlp
function run_tests() then
    print "Running tests..."

    -- Run test executable
    int result = execute_command("./test_runner")

    if result == 0 then
        print "All tests passed!"
        return 1
    else
        print "Tests failed with exit code: "
        print result

        -- Get test output for details
        string output = get_command_output("./test_runner 2>&1")
        print "Test output:"
        print output

        return 0
    end
end
```

---

## 4. Security Considerations

### 4.1 Command Injection Prevention

**Dangerous Pattern (DO NOT USE)**:
```mlp
function dangerous_search(user_input) then
    -- DANGEROUS: user_input could be "; rm -rf /"
    string cmd = "grep "
    cmd = string_concat(cmd, user_input)
    cmd = string_concat(cmd, " file.txt")
    execute_command(cmd)  -- VULNERABLE!
end
```

**Safe Pattern**:
```mlp
function safe_search(pattern) then
    -- Validate input: only allow alphanumeric
    int i = 0
    while i < string_length(pattern)
        string ch = string_char_at(pattern, i)
        int code = char_code(ch)
        -- Check alphanumeric (simplified)
        if code < 48 then
            print "Invalid character in pattern"
            return 0
        end
        i = i + 1
    end

    -- Safe to use after validation
    string cmd = "grep '"
    cmd = string_concat(cmd, pattern)
    cmd = string_concat(cmd, "' file.txt")
    execute_command(cmd)
    return 1
end
```

### 4.2 Path Quoting

```mlp
function compile_file(path) then
    -- Quote path to handle spaces
    string cmd = "nasm -f elf64 \""
    cmd = string_concat(cmd, path)
    cmd = string_concat(cmd, "\" -o output.o")
    return execute_command(cmd)
end
```

### 4.3 Shell Metacharacter Escape

Special characters to handle: ` $ " ' \ ! * ? [ ] { } ( ) & | ; < > newline

Consider implementing `shell_escape()` function for safety.

---

## 5. Error Handling

### 5.1 Command Execution Errors

```mlp
function run_with_error_handling(cmd) then
    int result = execute_command(cmd)

    if result == -1 then
        print "Error: Failed to execute command"
        return 0
    end

    if result == 127 then
        print "Error: Command not found"
        return 0
    end

    if result == 126 then
        print "Error: Permission denied"
        return 0
    end

    if result != 0 then
        print "Command exited with code: "
        print result
        return 0
    end

    return 1
end
```

### 5.2 Output Capture Errors

```mlp
function safe_get_output(cmd) then
    string output = get_command_output(cmd)
    int exit_code = get_error_code()

    if string_length(output) == 0 then
        if exit_code != 0 then
            print "Command failed with code: "
            print exit_code
            return ""
        end
        -- Empty output is valid
    end

    return output
end
```

---

## 6. Performance Considerations

### 6.1 Overhead
- `execute_command` creates new shell process
- `get_command_output` has pipe overhead
- For multiple commands, consider single shell script

### 6.2 Output Limits
- Default output buffer: 1MB
- Long-running commands block until complete
- Consider timeout mechanisms (future)

### 6.3 Benchmarks (Expected)
- `execute_command("true")`: ~5ms
- `get_command_output("echo test")`: ~8ms
- `get_process_id()`: < 1ms

---

## 7. Testing

### 7.1 Unit Tests

```mlp
function test_execute_command() then
    -- Test successful command
    int result = execute_command("true")
    assert(result == 0, "true should return 0")

    -- Test failing command
    result = execute_command("false")
    assert(result == 1, "false should return 1")

    -- Test exit code
    result = execute_command("exit 42")
    assert(result == 42, "exit 42 should return 42")

    print "test_execute_command: PASSED"
end

function test_get_command_output() then
    -- Test simple output
    string output = get_command_output("echo hello")
    output = string_trim(output)
    assert(output == "hello", "Should output 'hello'")

    -- Test multiline output
    output = get_command_output("echo -e 'line1\nline2'")
    assert(string_find(output, "line1") >= 0, "Should contain line1")
    assert(string_find(output, "line2") >= 0, "Should contain line2")

    print "test_get_command_output: PASSED"
end

function test_process_ids() then
    int pid = get_process_id()
    assert(pid > 0, "PID should be positive")

    int ppid = get_parent_process_id()
    assert(ppid > 0, "PPID should be positive")
    assert(ppid != pid, "PPID should differ from PID")

    print "test_process_ids: PASSED"
end
```

### 7.2 Integration Tests

- Test with actual compilers (nasm, gcc)
- Test with git commands
- Test timeout scenarios (future)
- Test with various exit codes

---

## 8. Future Enhancements

- Async command execution
- Command timeout
- Stderr capture (separate or combined)
- Environment variable passing
- Working directory specification
- Process groups
- Signal sending to child processes
- Pipe communication

---

## 9. Examples

See:
- `examples/build_system.mlp` - Build automation
- `examples/log_analyzer.mlp` - External tool integration
- `test/test_process_control.mlp` - Test suite

---

## 10. API Summary

| Function | Parameters | Returns | Description |
|----------|------------|---------|-------------|
| `execute_command` | cmd | int | Run command, get exit code |
| `get_command_output` | cmd | string | Run command, capture stdout |
| `get_process_id` | - | int | Get current PID |
| `get_parent_process_id` | - | int | Get parent PID |

---

**Status**: Complete
**Next**: Implement in runtime.c
