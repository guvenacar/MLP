#!/usr/bin/env python3
"""Apply Stage 0 (C bootstrap) compatibility transformations.

The legacy compiler lacks support for imports, typed collections, boolean
literals, and some control-flow syntax. This script normalizes a flattened MLP
file to the limited subset understood by Stage 0 so that we can generate the
first self-hosted binary.
"""
from __future__ import annotations

import argparse
import pathlib
import re
from typing import List

STRING_SPLIT_REGEX = re.compile(r'(".*?(?<!\\)")')
BOOL_TRUE_REGEX = re.compile(r"\btrue\b", re.IGNORECASE)
BOOL_FALSE_REGEX = re.compile(r"\bfalse\b", re.IGNORECASE)
LIST_REGEX = re.compile(r"\blist\s*\[[^\]]+\]")
MAP_REGEX = re.compile(r"\bmap\s*\[[^\]]+\]")
OPTIONAL_REGEX = re.compile(r"\boptional\s*\[[^\]]+\]")
END_BLOCK_REGEX = re.compile(r"(\s*)end\s+(enum|struct|function|if|while|for|try|catch)\s*$", re.IGNORECASE)
MLP_LIST_METHOD_REGEX = re.compile(r"(\b[\w]+)\.(mlp_list_[\w]+)\(")
SPLIT_FOR_REGEX = re.compile(r"^(\s*)for\s+([A-Za-z_][\w]*)\s*\n$")
FOR_CONT_REGEX = re.compile(r"^(\s*)([A-Za-z_][\w]*)\s*=(.*)$")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Make Stage 0 friendly source")
    parser.add_argument("input", type=pathlib.Path, help="Flattened .mlp file")
    parser.add_argument("output", type=pathlib.Path, help="Transformed output")
    return parser.parse_args()


def strip_line_comment(line: str) -> str:
    """Remove `-- comment` parts while respecting quoted strings."""
    in_string = False
    escaped = False
    result: List[str] = []
    i = 0
    length = len(line)
    while i < length:
        char = line[i]
        if char == "\\" and not escaped:
            escaped = True
            result.append(char)
            i += 1
            continue
        if char == '"' and not escaped:
            in_string = not in_string
            result.append(char)
            i += 1
            continue
        if not in_string and char == "-" and i + 1 < length and line[i + 1] == "-":
            break
        result.append(char)
        escaped = False
        i += 1
    return "".join(result).rstrip() + "\n"


def replace_outside_strings(line: str, pattern: re.Pattern[str], repl: str) -> str:
    parts = STRING_SPLIT_REGEX.split(line)
    for idx in range(0, len(parts), 2):
        if idx < len(parts):
            parts[idx] = pattern.sub(repl, parts[idx])
    return "".join(parts)


def fix_boolean_literals(line: str) -> str:
    line = replace_outside_strings(line, BOOL_TRUE_REGEX, "1")
    line = replace_outside_strings(line, BOOL_FALSE_REGEX, "0")
    return line


def fix_typed_collections(line: str) -> str:
    line = replace_outside_strings(line, LIST_REGEX, "numeric")
    line = replace_outside_strings(line, MAP_REGEX, "numeric")
    line = replace_outside_strings(line, OPTIONAL_REGEX, "numeric")
    return line


def fix_else_if(line: str) -> str:
    match = re.match(r"(\s*)else\s+if(.*)", line)
    if not match:
        return line
    indent = match.group(1)
    remainder = match.group(2).lstrip()
    return f"{indent}else\n{indent}    if {remainder}\n"


def fix_end_block(line: str) -> List[str] | None:
    match = END_BLOCK_REGEX.match(line.strip("\n"))
    if not match:
        return None
    indent = match.group(1)
    return [f"{indent}end\n"]


def process_line(line: str) -> List[str]:
    stripped = strip_line_comment(line)
    stripped = fix_boolean_literals(stripped)
    stripped = fix_typed_collections(stripped)
    stripped = MLP_LIST_METHOD_REGEX.sub(r"\2(", stripped)
    else_fixed = fix_else_if(stripped)
    if else_fixed != stripped:
        return [segment if segment.endswith("\n") else f"{segment}\n" for segment in else_fixed.strip("\n").split("\n")]
    end_fixed = fix_end_block(stripped)
    if end_fixed:
        return end_fixed
    return [stripped]


def merge_split_for_loops(lines: List[str]) -> List[str]:
    result: List[str] = []
    idx = 0
    total = len(lines)
    while idx < total:
        line = lines[idx]
        m = re.match(r"^(\s*)for\s+([A-Za-z_][\w]*)\s*\n$", line)
        if m and idx + 1 < total:
            nxt = lines[idx + 1]
            n = FOR_CONT_REGEX.match(nxt)
            if n and n.group(2) == m.group(2):
                indent = m.group(1)
                var = m.group(2)
                rhs = n.group(3).rstrip()
                result.append(f"{indent}for {var} ={rhs}\n")
                idx += 2
                continue
        result.append(line)
        idx += 1
    return result


def main() -> None:
    args = parse_args()
    transformed: List[str] = []

    with args.input.open("r", encoding="utf-8") as handle:
        for line in handle:
            transformed.extend(process_line(line))

    transformed = merge_split_for_loops(transformed)

    args.output.parent.mkdir(parents=True, exist_ok=True)
    with args.output.open("w", encoding="utf-8") as handle:
        handle.writelines(transformed)

    print(f"✅ Stage 0 compatibility fixes written to {args.output}")


if __name__ == "__main__":
    main()
