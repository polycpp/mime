#!/usr/bin/env python3
"""Generate C++ mime database header from mime-db's db.json.

Produces include/polycpp/mime/detail/mime_db_data.hpp with:
- Sorted MIME_DB array (mime-type -> {source, charset, compressible, ext_offset, ext_count})
- Flat EXTENSIONS array (all extensions, referenced by offset+count from MIME_DB)
- Sorted EXT_TO_MIME array (extension -> mime-type), using mimeScore for conflict resolution
"""

import json
import sys
import os
from pathlib import Path


# mimeScore logic ported from mime-types/mimeScore.js
FACET_SCORES = {
    'prs.': 100,
    'x-': 200,
    'x.': 300,
    'vnd.': 400,
}
FACET_DEFAULT = 900

SOURCE_SCORES = {
    'nginx': 10,
    'apache': 20,
    'iana': 40,
}
SOURCE_DEFAULT = 30

TYPE_SCORES = {
    'application': 1,
    'font': 2,
    'audio': 2,
    'video': 3,
}
TYPE_DEFAULT = 0


def mime_score(mime_type, source=''):
    if mime_type == 'application/octet-stream':
        return 0

    parts = mime_type.split('/')
    if len(parts) != 2:
        return 0
    type_part, subtype = parts

    # Extract facet
    import re
    facet_match = re.match(r'(\.|x-)', subtype)
    facet = facet_match.group(1) if facet_match else None
    # More precise: match the JS regex behavior
    # facet = subtype.replace(/(\.|x-).*/, '$1')
    for prefix in ['prs.', 'x-', 'x.', 'vnd.']:
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


def main():
    db_path = Path('<mime-db checkout>/db.json')
    out_path = Path('<repo path>/include/polycpp/mime/detail/mime_db_data.hpp')

    with open(db_path) as f:
        db = json.load(f)

    # Collect all entries sorted by mime type
    entries = []
    all_extensions = []  # flat list of all extensions
    ext_to_mime = {}  # extension -> best mime type (using mimeScore)

    for mime_type in sorted(db.keys()):
        info = db[mime_type]
        source = info.get('source', '')
        charset = info.get('charset', '')
        compressible = info.get('compressible', False)
        # compressible can be True, False, or absent
        # We'll encode as: 0=unknown, 1=true, 2=false
        if 'compressible' not in info:
            comp_val = 0
        elif compressible:
            comp_val = 1
        else:
            comp_val = 2
        exts = info.get('extensions', [])

        ext_offset = len(all_extensions)
        ext_count = len(exts)
        all_extensions.extend(exts)

        entries.append({
            'type': mime_type,
            'source': source,
            'charset': charset,
            'compressible': comp_val,
            'ext_offset': ext_offset,
            'ext_count': ext_count,
        })

        # Build ext -> mime mapping with conflict resolution
        for ext in exts:
            if ext in ext_to_mime:
                existing = ext_to_mime[ext]
                existing_score = mime_score(existing, db[existing].get('source', ''))
                new_score = mime_score(mime_type, source)
                if new_score > existing_score:
                    ext_to_mime[ext] = mime_type
            else:
                ext_to_mime[ext] = mime_type

    # Build sorted ext_to_mime array
    ext_entries = sorted(ext_to_mime.items())

    # Collect unique strings for dedup
    sources = sorted(set(e['source'] for e in entries))
    charsets = sorted(set(e['charset'] for e in entries))

    print(f"Total MIME entries: {len(entries)}")
    print(f"Total extensions (flat): {len(all_extensions)}")
    print(f"Unique ext->mime mappings: {len(ext_entries)}")
    print(f"Sources: {sources}")
    print(f"Charsets: {charsets}")

    # Generate the header
    lines = []
    lines.append('#pragma once')
    lines.append('')
    lines.append('// AUTO-GENERATED from mime-db v1.54.0 db.json')
    lines.append('// Do not edit manually. Regenerate with scripts/generate_db.py')
    lines.append('')
    lines.append('#include <array>')
    lines.append('#include <cstdint>')
    lines.append('#include <string_view>')
    lines.append('')
    lines.append('namespace polycpp {')
    lines.append('namespace mime {')
    lines.append('namespace detail {')
    lines.append('')

    # Compressible enum
    lines.append('enum class Compressible : uint8_t {')
    lines.append('    Unknown = 0,')
    lines.append('    Yes = 1,')
    lines.append('    No = 2')
    lines.append('};')
    lines.append('')

    # MimeEntry struct
    lines.append('struct MimeEntry {')
    lines.append('    std::string_view type;')
    lines.append('    std::string_view source;')
    lines.append('    std::string_view charset;')
    lines.append('    Compressible compressible;')
    lines.append('    uint16_t ext_offset;')
    lines.append('    uint8_t ext_count;')
    lines.append('};')
    lines.append('')

    # ExtEntry struct
    lines.append('struct ExtEntry {')
    lines.append('    std::string_view ext;')
    lines.append('    std::string_view mime_type;')
    lines.append('};')
    lines.append('')

    # MIME_DB array
    comp_map = {0: 'Compressible::Unknown', 1: 'Compressible::Yes', 2: 'Compressible::No'}
    lines.append(f'inline constexpr std::array<MimeEntry, {len(entries)}> MIME_DB = {{{{')
    for e in entries:
        c = comp_map[e['compressible']]
        lines.append(f'    {{"{e["type"]}", "{e["source"]}", "{e["charset"]}", {c}, {e["ext_offset"]}, {e["ext_count"]}}},')
    lines.append('}};')
    lines.append('')

    # EXTENSIONS flat array
    lines.append(f'inline constexpr std::array<std::string_view, {len(all_extensions)}> EXTENSIONS = {{{{')
    for ext in all_extensions:
        lines.append(f'    "{ext}",')
    lines.append('}};')
    lines.append('')

    # EXT_TO_MIME array (sorted by extension for binary search)
    lines.append(f'inline constexpr std::array<ExtEntry, {len(ext_entries)}> EXT_TO_MIME = {{{{')
    for ext, mime_type in ext_entries:
        lines.append(f'    {{"{ext}", "{mime_type}"}},')
    lines.append('}};')
    lines.append('')

    lines.append('} // namespace detail')
    lines.append('} // namespace mime')
    lines.append('} // namespace polycpp')
    lines.append('')

    os.makedirs(out_path.parent, exist_ok=True)
    with open(out_path, 'w') as f:
        f.write('\n'.join(lines))

    print(f"\nGenerated: {out_path}")
    print(f"Lines: {len(lines)}")


if __name__ == '__main__':
    main()
