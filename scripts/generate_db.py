#!/usr/bin/env python3
"""Generate C++ MIME database data from mime-db's db.json.

Produces include/polycpp/mime/detail/mime_db_data.hpp with:
- Sorted MIME_DB array (MIME type -> source, charset, compressibility, extension slice)
- Flat EXTENSIONS array referenced by MIME_DB offset/count fields
- Sorted EXT_TO_MIME array using mimeScore conflict resolution
- EXTENSION_CONFLICTS diagnostics comparing legacy preference to mimeScore
"""

import argparse
import json
from pathlib import Path


FACET_SCORES = {
    "prs.": 100,
    "x-": 200,
    "x.": 300,
    "vnd.": 400,
}
FACET_DEFAULT = 900

SOURCE_SCORES = {
    "nginx": 10,
    "apache": 20,
    "iana": 40,
}
SOURCE_DEFAULT = 30

TYPE_SCORES = {
    "application": 1,
    "font": 2,
    "audio": 2,
    "video": 3,
}
TYPE_DEFAULT = 0

SOURCE_RANK = {
    "nginx": 0,
    "apache": 1,
    "": 2,
    None: 2,
    "iana": 3,
}


def mime_score(mime_type, source=""):
    if mime_type == "application/octet-stream":
        return 0

    parts = mime_type.split("/")
    if len(parts) != 2:
        return 0

    type_part, subtype = parts
    for prefix in ["prs.", "x-", "x.", "vnd."]:
        if subtype.startswith(prefix):
            facet = prefix
            break
    else:
        facet = None

    facet_score = FACET_SCORES.get(facet, FACET_DEFAULT) if facet else FACET_DEFAULT
    source_score = SOURCE_SCORES.get(source, SOURCE_DEFAULT)
    type_score = TYPE_SCORES.get(type_part, TYPE_DEFAULT)
    length_score = 1 - len(mime_type) / 100

    return facet_score + source_score + type_score + length_score


def legacy_preferred_type(existing, candidate, db):
    if not existing:
        return candidate

    from_rank = SOURCE_RANK.get(db[existing].get("source", ""), 2)
    to_rank = SOURCE_RANK.get(db[candidate].get("source", ""), 2)

    if (
        existing != "application/octet-stream"
        and (
            from_rank > to_rank
            or (from_rank == to_rank and existing.startswith("application/"))
        )
    ):
        return existing

    return candidate


def find_default_db_path(repo_root):
    candidates = [
        repo_root / ".tmp/npm-artifacts/mime-db/db.json",
        repo_root / ".tmp/npm-package/node_modules/mime-db/db.json",
        repo_root / "node_modules/mime-db/db.json",
    ]
    for candidate in candidates:
        if candidate.exists():
            return candidate
    raise SystemExit(
        "could not find mime-db db.json; pass --db-json /path/to/db.json "
        "or extract the published mime-db npm artifact first"
    )


def infer_version(db_path):
    package_json = db_path.parent / "package.json"
    if not package_json.exists():
        return "unknown"
    with package_json.open() as f:
        return json.load(f).get("version", "unknown")


def cpp_string(value):
    return json.dumps(str(value))


def main():
    repo_root = Path(__file__).resolve().parents[1]

    parser = argparse.ArgumentParser(description="Generate C++ MIME database data from mime-db db.json.")
    parser.add_argument("--db-json", type=Path, default=None, help="path to mime-db db.json")
    parser.add_argument(
        "--output",
        type=Path,
        default=repo_root / "include/polycpp/mime/detail/mime_db_data.hpp",
        help="output header path",
    )
    parser.add_argument("--mime-db-version", default=None, help="version string for the provenance comment")
    args = parser.parse_args()

    db_path = args.db_json or find_default_db_path(repo_root)
    out_path = args.output
    version = args.mime_db_version or infer_version(db_path)

    with db_path.open() as f:
        db = json.load(f)

    entries = []
    all_extensions = []
    ext_to_mime = {}
    legacy_ext_to_mime = {}

    for mime_type in sorted(db.keys()):
        info = db[mime_type]
        source = info.get("source", "")
        charset = info.get("charset", "")
        compressible = info.get("compressible", False)
        if "compressible" not in info:
            comp_val = 0
        elif compressible:
            comp_val = 1
        else:
            comp_val = 2

        exts = info.get("extensions", [])
        ext_offset = len(all_extensions)
        ext_count = len(exts)
        all_extensions.extend(exts)

        entries.append({
            "type": mime_type,
            "source": source,
            "charset": charset,
            "compressible": comp_val,
            "ext_offset": ext_offset,
            "ext_count": ext_count,
        })

        for ext in exts:
            existing = ext_to_mime.get(ext)
            if existing:
                existing_score = mime_score(existing, db[existing].get("source", ""))
                new_score = mime_score(mime_type, source)
                if new_score >= existing_score:
                    ext_to_mime[ext] = mime_type
            else:
                ext_to_mime[ext] = mime_type

            legacy_ext_to_mime[ext] = legacy_preferred_type(
                legacy_ext_to_mime.get(ext),
                mime_type,
                db,
            )

    ext_entries = sorted(ext_to_mime.items())
    conflicts = [
        (ext, legacy_ext_to_mime[ext], ext_to_mime[ext])
        for ext in sorted(ext_to_mime)
        if legacy_ext_to_mime.get(ext) != ext_to_mime[ext]
    ]

    sources = sorted(set(e["source"] for e in entries))
    charsets = sorted(set(e["charset"] for e in entries))

    print(f"Total MIME entries: {len(entries)}")
    print(f"Total extensions (flat): {len(all_extensions)}")
    print(f"Unique ext->mime mappings: {len(ext_entries)}")
    print(f"Extension conflicts: {len(conflicts)}")
    print(f"Sources: {sources}")
    print(f"Charsets: {charsets}")

    lines = []
    lines.append("#pragma once")
    lines.append("")
    lines.append(f"// AUTO-GENERATED from mime-db v{version} db.json")
    lines.append("// Source artifact: pass the upstream mime-db db.json path with --db-json")
    lines.append("// Do not edit manually. Regenerate with scripts/generate_db.py --db-json <path-to-db.json>")
    lines.append("")
    lines.append("#include <array>")
    lines.append("#include <cstdint>")
    lines.append("#include <string_view>")
    lines.append("")
    lines.append("namespace polycpp {")
    lines.append("namespace mime {")
    lines.append("namespace detail {")
    lines.append("")

    lines.append("enum class Compressible : uint8_t {")
    lines.append("    Unknown = 0,")
    lines.append("    Yes = 1,")
    lines.append("    No = 2")
    lines.append("};")
    lines.append("")

    lines.append("struct MimeEntry {")
    lines.append("    std::string_view type;")
    lines.append("    std::string_view source;")
    lines.append("    std::string_view charset;")
    lines.append("    Compressible compressible;")
    lines.append("    uint16_t ext_offset;")
    lines.append("    uint8_t ext_count;")
    lines.append("};")
    lines.append("")

    comp_map = {0: "Compressible::Unknown", 1: "Compressible::Yes", 2: "Compressible::No"}
    lines.append(f"inline constexpr std::array<MimeEntry, {len(entries)}> MIME_DB = {{{{")
    for e in entries:
        c = comp_map[e["compressible"]]
        lines.append(
            f"    {{{cpp_string(e['type'])}, {cpp_string(e['source'])}, "
            f"{cpp_string(e['charset'])}, {c}, {e['ext_offset']}, {e['ext_count']}}},"
        )
    lines.append("}};")
    lines.append("")

    lines.append(f"inline constexpr std::array<std::string_view, {len(all_extensions)}> EXTENSIONS = {{{{")
    for ext in all_extensions:
        lines.append(f"    {cpp_string(ext)},")
    lines.append("}};")
    lines.append("")

    lines.append(f"inline constexpr std::array<TypeEntry, {len(ext_entries)}> EXT_TO_MIME = {{{{")
    for ext, mime_type in ext_entries:
        lines.append(f"    {{{cpp_string(ext)}, {cpp_string(mime_type)}}},")
    lines.append("}};")
    lines.append("")

    lines.append(f"inline constexpr std::array<ExtensionConflict, {len(conflicts)}> EXTENSION_CONFLICTS = {{{{")
    for ext, legacy_type, preferred_type in conflicts:
        lines.append(
            f"    {{{cpp_string(ext)}, {cpp_string(legacy_type)}, {cpp_string(preferred_type)}}},"
        )
    lines.append("}};")
    lines.append("")

    lines.append("} // namespace detail")
    lines.append("} // namespace mime")
    lines.append("} // namespace polycpp")
    lines.append("")

    out_path.parent.mkdir(parents=True, exist_ok=True)
    with out_path.open("w") as f:
        f.write("\n".join(lines))

    print(f"\nGenerated: {out_path}")
    print(f"Lines: {len(lines)}")


if __name__ == "__main__":
    main()
