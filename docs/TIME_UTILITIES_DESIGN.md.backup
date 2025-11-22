# Time Utilities - Design Specification

**Status**: Phase 5.3 - In Progress
**Version**: 1.0
**Last Updated**: November 21, 2025

---

## Overview

This document specifies time utility functions for MLP, enabling programs to work with timestamps, format dates, measure performance, and handle time-related operations.

### Goals
- Format timestamps to human-readable strings
- Parse date strings to timestamps
- High-precision timing for profiling
- Get current time as formatted string

### Non-Goals
- Timezone handling (future work)
- Calendar arithmetic (future work)
- Scheduling/timers (future work)

---

## 1. Time Utility Functions

### 1.1 format_timestamp()

**Purpose**: Convert Unix timestamp to formatted string

**Signature**:
```mlp
function format_timestamp(timestamp, format) then
    -- Returns formatted date/time string
end
```

**Parameters**:
- `timestamp` (int): Unix timestamp (seconds since epoch)
- `format` (string): Format string (strftime-compatible)

**Return Value**:
- Formatted date/time string
- Empty string on error

**Format Specifiers**:
| Code | Description | Example |
|------|-------------|---------|
| `%Y` | Year (4 digits) | 2025 |
| `%m` | Month (01-12) | 11 |
| `%d` | Day (01-31) | 21 |
| `%H` | Hour (00-23) | 14 |
| `%M` | Minute (00-59) | 30 |
| `%S` | Second (00-59) | 45 |
| `%a` | Weekday abbrev | Fri |
| `%A` | Weekday full | Friday |
| `%b` | Month abbrev | Nov |
| `%B` | Month full | November |
| `%%` | Literal % | % |

**Example**:
```mlp
function show_build_time(timestamp) then
    string formatted = format_timestamp(timestamp, "%Y-%m-%d %H:%M:%S")
    print "Build time: "
    print formatted
end

function format_log_entry(timestamp, message) then
    string time_str = format_timestamp(timestamp, "[%H:%M:%S]")
    string entry = string_concat(time_str, " ")
    entry = string_concat(entry, message)
    return entry
end
```

**Common Formats**:
```mlp
-- ISO 8601
format_timestamp(ts, "%Y-%m-%dT%H:%M:%S")  -- "2025-11-21T14:30:45"

-- Date only
format_timestamp(ts, "%Y-%m-%d")           -- "2025-11-21"

-- Time only
format_timestamp(ts, "%H:%M:%S")           -- "14:30:45"

-- Human readable
format_timestamp(ts, "%B %d, %Y")          -- "November 21, 2025"

-- Log format
format_timestamp(ts, "[%Y%m%d_%H%M%S]")    -- "[20251121_143045]"
```

---

### 1.2 parse_timestamp()

**Purpose**: Parse date string to Unix timestamp

**Signature**:
```mlp
function parse_timestamp(date_string) then
    -- Returns Unix timestamp
end
```

**Parameters**:
- `date_string` (string): Date string to parse

**Return Value**:
- Unix timestamp (seconds since epoch)
- `0` on parse error

**Supported Formats** (auto-detected):
- `YYYY-MM-DD` (e.g., "2025-11-21")
- `YYYY-MM-DD HH:MM:SS` (e.g., "2025-11-21 14:30:45")
- `YYYY-MM-DDTHH:MM:SS` (ISO 8601)
- `DD/MM/YYYY` (e.g., "21/11/2025")

**Example**:
```mlp
function compare_dates(date1, date2) then
    int ts1 = parse_timestamp(date1)
    int ts2 = parse_timestamp(date2)

    if ts1 == 0 then
        print "Invalid date: "
        print date1
        return 0
    end

    if ts2 == 0 then
        print "Invalid date: "
        print date2
        return 0
    end

    if ts1 > ts2 then
        print date1
        print " is after "
        print date2
    else
        print date1
        print " is before or equal to "
        print date2
    end

    return 1
end
```

---

### 1.3 get_milliseconds()

**Purpose**: Get high-precision time for profiling

**Signature**:
```mlp
function get_milliseconds() then
    -- Returns milliseconds since epoch (or program start)
end
```

**Parameters**: None

**Return Value**:
- Milliseconds as 64-bit integer
- Suitable for measuring elapsed time

**Behavior**:
- Uses `clock_gettime(CLOCK_MONOTONIC)`
- Monotonic (never goes backward)
- Not affected by system time changes

**Example**:
```mlp
function measure_execution(name) then
    int start = get_milliseconds()

    -- Execute work
    do_heavy_computation()

    int end = get_milliseconds()
    int elapsed = end - start

    print name
    print " took "
    print elapsed
    print " ms"

    return elapsed
end

function benchmark_function(iterations) then
    int total = 0
    int i = 0

    while i < iterations
        int start = get_milliseconds()
        test_function()
        int elapsed = get_milliseconds() - start
        total = total + elapsed
        i = i + 1
    end

    int average = total / iterations
    print "Average time: "
    print average
    print " ms over "
    print iterations
    print " iterations"
end
```

---

### 1.4 get_time_string()

**Purpose**: Get current time as formatted string

**Signature**:
```mlp
function get_time_string() then
    -- Returns current time as "YYYY-MM-DD HH:MM:SS"
end
```

**Parameters**: None

**Return Value**:
- Current time in ISO-like format
- "YYYY-MM-DD HH:MM:SS"

**Behavior**:
- Equivalent to `format_timestamp(current_timestamp(), "%Y-%m-%d %H:%M:%S")`
- Convenience function for common use case

**Example**:
```mlp
function log_message(level, message) then
    string timestamp = get_time_string()
    print timestamp
    print " ["
    print level
    print "] "
    print message
end

function create_build_report() then
    print "Build Report"
    print "============"
    print "Generated: "
    print get_time_string()
    print ""
    -- ... rest of report
end
```

---

## 2. Implementation Details

### 2.1 Runtime Implementation (C)

```c
#include <time.h>
#include <string.h>

// format_timestamp - Format Unix timestamp
const char* mlp_format_timestamp(int64_t timestamp, const char* format) {
    static char buffer[256];
    buffer[0] = '\0';

    time_t t = (time_t)timestamp;
    struct tm* tm_info = localtime(&t);

    if (!tm_info) {
        mlp_set_error_code(1);
        return buffer;
    }

    size_t result = strftime(buffer, sizeof(buffer), format, tm_info);
    if (result == 0) {
        mlp_set_error_code(2);  // Format error
        buffer[0] = '\0';
    }

    mlp_set_error_code(0);
    return buffer;
}

// parse_timestamp - Parse date string
int64_t mlp_parse_timestamp(const char* date_string) {
    struct tm tm_info = {0};
    char* result;

    // Try ISO format: YYYY-MM-DD HH:MM:SS
    result = strptime(date_string, "%Y-%m-%d %H:%M:%S", &tm_info);
    if (result && *result == '\0') {
        mlp_set_error_code(0);
        return (int64_t)mktime(&tm_info);
    }

    // Try ISO format: YYYY-MM-DDTHH:MM:SS
    memset(&tm_info, 0, sizeof(tm_info));
    result = strptime(date_string, "%Y-%m-%dT%H:%M:%S", &tm_info);
    if (result && *result == '\0') {
        mlp_set_error_code(0);
        return (int64_t)mktime(&tm_info);
    }

    // Try date only: YYYY-MM-DD
    memset(&tm_info, 0, sizeof(tm_info));
    result = strptime(date_string, "%Y-%m-%d", &tm_info);
    if (result && *result == '\0') {
        mlp_set_error_code(0);
        return (int64_t)mktime(&tm_info);
    }

    // Try DD/MM/YYYY
    memset(&tm_info, 0, sizeof(tm_info));
    result = strptime(date_string, "%d/%m/%Y", &tm_info);
    if (result && *result == '\0') {
        mlp_set_error_code(0);
        return (int64_t)mktime(&tm_info);
    }

    mlp_set_error_code(1);  // Parse error
    return 0;
}

// get_milliseconds - High-precision time
int64_t mlp_get_milliseconds(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (int64_t)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

// get_time_string - Current time as string
const char* mlp_get_time_string(void) {
    return mlp_format_timestamp((int64_t)time(NULL), "%Y-%m-%d %H:%M:%S");
}
```

### 2.2 Compiler Integration

**Lexer Tokens**:
```c
TOKEN_FORMAT_TIMESTAMP,   // format_timestamp
TOKEN_PARSE_TIMESTAMP,    // parse_timestamp
TOKEN_GET_MILLISECONDS,   // get_milliseconds
TOKEN_GET_TIME_STRING,    // get_time_string
```

**Code Generation**:
```asm
; format_timestamp(timestamp, format)
mov rdi, [timestamp_val]
mov rsi, [format_ptr]
call mlp_format_timestamp
; string pointer in rax

; get_milliseconds()
call mlp_get_milliseconds
; milliseconds in rax
```

---

## 3. Use Cases

### 3.1 Build System Timestamps

```mlp
function log_build_step(step_name) then
    string time = get_time_string()
    print "["
    print time
    print "] "
    print step_name
end

function check_file_freshness(source, object) then
    int src_info = get_file_info(source)
    int obj_info = get_file_info(object)

    if obj_info == 0 then
        mlp_free(src_info)
        return 0  -- Object doesn't exist, needs build
    end

    int src_mtime = file_info_modified(src_info)
    int obj_mtime = file_info_modified(obj_info)

    mlp_free(src_info)
    mlp_free(obj_info)

    -- Compare modification times
    if src_mtime > obj_mtime then
        print "Source newer than object by "
        print (src_mtime - obj_mtime)
        print " seconds"
        return 0  -- Needs rebuild
    end

    return 1  -- Object is up to date
end
```

### 3.2 Performance Profiling

```mlp
function profile_compiler() then
    print "=== Compiler Performance Profile ==="

    -- Lexing
    int start = get_milliseconds()
    tokenize(source_code)
    int lex_time = get_milliseconds() - start
    print "Lexing: "
    print lex_time
    print " ms"

    -- Parsing
    start = get_milliseconds()
    parse(tokens)
    int parse_time = get_milliseconds() - start
    print "Parsing: "
    print parse_time
    print " ms"

    -- Code generation
    start = get_milliseconds()
    generate_code(ast)
    int codegen_time = get_milliseconds() - start
    print "Code generation: "
    print codegen_time
    print " ms"

    -- Total
    int total = lex_time + parse_time + codegen_time
    print "Total: "
    print total
    print " ms"
end
```

### 3.3 Log File Management

```mlp
function create_log_filename() then
    int ts = current_timestamp()
    string date_part = format_timestamp(ts, "%Y%m%d")

    string filename = "log_"
    filename = string_concat(filename, date_part)
    filename = string_concat(filename, ".txt")

    return filename
end

function write_log_entry(level, message) then
    string timestamp = get_time_string()

    string entry = timestamp
    entry = string_concat(entry, " [")
    entry = string_concat(entry, level)
    entry = string_concat(entry, "] ")
    entry = string_concat(entry, message)
    entry = string_concat(entry, "\n")

    string log_file = create_log_filename()
    append_file(log_file, entry)
end
```

### 3.4 Duration Calculation

```mlp
function calculate_age(birth_date) then
    int birth_ts = parse_timestamp(birth_date)
    if birth_ts == 0 then
        print "Invalid date format"
        return -1
    end

    int now = current_timestamp()
    int seconds_diff = now - birth_ts

    -- Convert to days (rough)
    int days = seconds_diff / 86400

    return days
end

function time_since(timestamp) then
    int now = current_timestamp()
    int diff = now - timestamp

    if diff < 60 then
        print diff
        print " seconds ago"
    else if diff < 3600 then
        print diff / 60
        print " minutes ago"
    else if diff < 86400 then
        print diff / 3600
        print " hours ago"
    else
        print diff / 86400
        print " days ago"
    end
end
```

---

## 4. Error Handling

### 4.1 Invalid Timestamps

```mlp
function safe_format_timestamp(timestamp, format) then
    -- Validate timestamp (reasonable range)
    if timestamp < 0 then
        print "Error: Negative timestamp"
        return ""
    end

    -- Far future check (year 3000+)
    if timestamp > 32503680000 then
        print "Warning: Far future timestamp"
    end

    string result = format_timestamp(timestamp, format)
    if string_length(result) == 0 then
        print "Error: Format failed"
        return ""
    end

    return result
end
```

### 4.2 Parse Errors

```mlp
function parse_date_safe(date_string) then
    int timestamp = parse_timestamp(date_string)

    if timestamp == 0 then
        -- Check if it's actually epoch or an error
        int error = get_error_code()
        if error != 0 then
            print "Invalid date format: "
            print date_string
            return -1
        end
    end

    return timestamp
end
```

---

## 5. Performance Considerations

### 5.1 Time Function Overhead
- `get_milliseconds()`: ~100ns (very fast)
- `format_timestamp()`: ~1-5us (strftime overhead)
- `parse_timestamp()`: ~2-10us (multiple format attempts)
- `get_time_string()`: ~2us (combines calls)

### 5.2 Static Buffers
- Format functions use static buffers
- Not thread-safe (single-threaded MLP is fine)
- Buffer size: 256 characters max

### 5.3 Caching
- For hot paths, cache formatted timestamps
- Refresh cached time every second if needed

---

## 6. Testing

### 6.1 Unit Tests

```mlp
function test_format_timestamp() then
    -- Known timestamp: 2025-11-21 00:00:00 UTC
    int ts = 1763740800

    string result = format_timestamp(ts, "%Y-%m-%d")
    assert(result == "2025-11-21", "Date format failed")

    result = format_timestamp(ts, "%H:%M:%S")
    -- Note: depends on timezone
    assert(string_length(result) == 8, "Time format wrong length")

    print "test_format_timestamp: PASSED"
end

function test_parse_timestamp() then
    int ts = parse_timestamp("2025-11-21")
    assert(ts > 0, "Parse failed")

    ts = parse_timestamp("2025-11-21 14:30:00")
    assert(ts > 0, "Parse with time failed")

    ts = parse_timestamp("invalid")
    assert(ts == 0, "Should fail on invalid")

    print "test_parse_timestamp: PASSED"
end

function test_get_milliseconds() then
    int t1 = get_milliseconds()
    -- Small delay
    int i = 0
    while i < 100000
        i = i + 1
    end
    int t2 = get_milliseconds()

    assert(t2 >= t1, "Time should not go backward")
    assert(t2 - t1 < 1000, "Loop shouldn't take > 1 second")

    print "test_get_milliseconds: PASSED"
end

function test_get_time_string() then
    string time = get_time_string()
    assert(string_length(time) == 19, "Should be YYYY-MM-DD HH:MM:SS")
    assert(string_char_at(time, 4) == "-", "Should have dash at pos 4")

    print "test_get_time_string: PASSED"
end
```

---

## 7. Future Enhancements

- Timezone support (UTC, local, named zones)
- Calendar arithmetic (add days, months)
- Duration formatting (human-readable intervals)
- Recurring events / scheduling
- High-resolution timestamps (nanoseconds)
- Date validation

---

## 8. Examples

See:
- `examples/build_system.mlp` - Build timestamps and profiling
- `examples/log_analyzer.mlp` - Log parsing with timestamps
- `test/test_time_utilities.mlp` - Test suite

---

## 9. API Summary

| Function | Parameters | Returns | Description |
|----------|------------|---------|-------------|
| `format_timestamp` | timestamp, format | string | Format timestamp to string |
| `parse_timestamp` | date_string | int | Parse string to timestamp |
| `get_milliseconds` | - | int | High-precision time (ms) |
| `get_time_string` | - | string | Current time as string |

---

**Status**: Complete
**Next**: Implement in runtime.c
