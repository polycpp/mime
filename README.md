# polycpp/mime

C++ port of npm [mime-types](https://www.npmjs.com/package/mime-types), [mime-db](https://www.npmjs.com/package/mime-db), and [media-typer](https://www.npmjs.com/package/media-typer) for [polycpp](https://github.com/enricohuang/polycpp).

Port version: `1.0.0`

Initial port based on upstream version: `mime-types 3.0.2, mime-db 1.54.0, media-typer 1.1.0`

## Status

Implemented APIs cover MIME lookup, Content-Type generation, extension lookup, charset detection, read-only data access, conflict diagnostics, and media-typer parse/format/test helpers. Known upstream divergences are recorded in `docs/divergences.md`.

## Features

- **MIME type lookup** by file path or extension
- **Content-Type header generation** with automatic charset
- **Extension lookup** by MIME type
- **Charset detection** for MIME types
- **Read-only data access** for upstream `types` and `extensions` maps
- **Extension conflict diagnostics** matching upstream `mimeScore`
- **Media type parsing/formatting/validation** per RFC 6838
- Header-only, zero startup cost (compile-time sorted `constexpr` data)
- 2,601 MIME types from mime-db v1.54.0

## Usage

```cpp
#include <polycpp/mime/mime.hpp>

using namespace polycpp::mime;

// Lookup MIME type by extension or path
auto type = lookup("json");              // "application/json"
auto type2 = lookup("/path/to/file.js"); // "text/javascript"

// Generate Content-Type header
auto ct = contentType("html");           // "text/html; charset=utf-8"

// Get extension for MIME type
auto ext = extension("text/html");       // "html"

// Get charset
auto cs = charset("text/html");          // "UTF-8"

// Inspect read-only generated data
auto typeEntries = types();              // extension -> MIME type entries
auto exts = extensions("application/json"); // {"json", "map"}
auto conflicts = extensionConflicts();   // mimeScore conflict diagnostics

// Parse media type (RFC 6838)
auto mt = parse("application/vnd.api+json");
// mt.type == "application", mt.subtype == "vnd.api", mt.suffix == "json"

// Format media type
auto str = format(MediaType{"image", "svg", "xml"}); // "image/svg+xml"

// Validate media type
bool ok = test("text/html");  // true
```

## Requirements

- C++20 compiler (GCC 13+, Clang 16+)
- CMake 3.20+
- polycpp (fetched automatically via CMake FetchContent)

## Building

```bash
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j$(nproc)
cd build && ctest --output-on-failure
```

## License

MIT
