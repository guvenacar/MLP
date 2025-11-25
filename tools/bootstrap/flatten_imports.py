#!/usr/bin/env python3
"""Flatten MLP import graph into a single file for the Stage 0 compiler.

The legacy C bootstrap compiler does not understand `import` statements. This
script recursively resolves them and produces a monolithic `.mlp` file that can
be consumed by Stage 0 without modifying the original sources.
"""
from __future__ import annotations

import argparse
import pathlib
from typing import Iterable, List, Set


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Flatten MLP imports")
    parser.add_argument("input", type=pathlib.Path, help="Entry .mlp file")
    parser.add_argument("output", type=pathlib.Path, help="Flattened output path")
    parser.add_argument(
        "--extra-root",
        action="append",
        default=[],
        help="Additional directories to search for imported files",
    )
    return parser.parse_args()


def resolve_import(
    import_path: str,
    current_dir: pathlib.Path,
    search_roots: Iterable[pathlib.Path],
) -> pathlib.Path:
    """Try all search roots until we find a matching file."""
    candidates: List[pathlib.Path] = []

    # First look relative to the current file
    candidates.append(current_dir / import_path)

    for root in search_roots:
        candidates.append(root / import_path)

    for candidate in candidates:
        if candidate.exists():
            return candidate.resolve()

    target_name = pathlib.Path(import_path).name
    for root in search_roots:
        for candidate in root.rglob(target_name):
            if candidate.is_file():
                return candidate.resolve()

    raise FileNotFoundError(f"Cannot resolve import '{import_path}'")


def flatten_file(
    path: pathlib.Path,
    root_dirs: Iterable[pathlib.Path],
    visited: Set[pathlib.Path],
) -> List[str]:
    path = path.resolve()
    if path in visited:
        return [f"-- SKIP IMPORT: {path}\n"]

    visited.add(path)
    lines: List[str] = []
    current_dir = path.parent

    with path.open("r", encoding="utf-8") as handle:
        for line in handle:
            stripped = line.strip()
            if stripped.startswith("import ") and "\"" in stripped:
                import_target = stripped.split("\"", 2)[1]
                resolved = resolve_import(import_target, current_dir, root_dirs)
                lines.append(f"-- BEGIN IMPORT: {import_target}\n")
                lines.extend(flatten_file(resolved, root_dirs, visited))
                lines.append(f"-- END IMPORT: {import_target}\n")
            else:
                lines.append(line)

    return lines


def main() -> None:
    args = parse_args()
    entry = args.input.resolve()

    # Default search roots: entry dir, repo root, compiler_core folder
    repo_root = entry.parent
    for parent in entry.parents:
        if (parent / "self_host").exists():
            repo_root = parent
            break

    search_roots: List[pathlib.Path] = [entry.parent, repo_root, repo_root / "self_host", repo_root / "self_host" / "compiler_core"]
    search_roots.extend(pathlib.Path(p).resolve() for p in args.extra_root)

    flattened = flatten_file(entry, search_roots, set())
    args.output.parent.mkdir(parents=True, exist_ok=True)
    with args.output.open("w", encoding="utf-8") as handle:
        handle.writelines(flattened)

    print(f"✅ Flattened {entry} -> {args.output} ({len(flattened)} lines)")


if __name__ == "__main__":
    main()
