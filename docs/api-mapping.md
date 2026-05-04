# API Mapping

| Upstream symbol | C++ symbol | Status | Notes |
|---|---|---|---|
| `mime.lookup(path)` | `polycpp::mime::lookup(const std::string&)` | direct | Uses `polycpp::path::extname` and returns `std::optional<std::string>` instead of `false`. |
| `mime.contentType(str)` | `polycpp::mime::contentType(const std::string&)` | adapted | Same broad behavior, but charset-parameter detection is more precise than upstream's substring check. |
| `mime.extension(type)` | `polycpp::mime::extension(const std::string&)` | direct | Strips parameters and returns the first extension from generated `mime-db` data, using `std::optional` for not found. |
| `mime.charset(type)` | `polycpp::mime::charset(const std::string&)` | direct | Uses generated charset data and defaults `text/*` to `UTF-8`; returns `std::optional` for no charset. |
| `mime.charsets.lookup` | no separate C++ alias | omitted | C++ exposes the canonical `charset()` function only; duplicating a namespace alias does not add typed value. |
| `mime.types[extension]` | `polycpp::mime::types()` plus `lookup()` | adapted | Exposes a read-only span of extension-to-type entries instead of a mutable JavaScript object. |
| `mime.extensions[type]` | `polycpp::mime::extensions(const std::string&)` plus `extension()` | adapted | Exposes per-type read-only extension vectors instead of a mutable JavaScript object map. |
| `mime._extensionConflicts` | `polycpp::mime::extensionConflicts()` | adapted | Exposes read-only diagnostic records from upstream `mimeScore` conflict resolution. |
| `mime-db` exported database object | generated `detail::MIME_DB`, `detail::EXTENSIONS`, `detail::EXT_TO_MIME`, plus public read-only accessors | compatibility layer | Generated data preserves lookup behavior; the full database object remains implementation detail. |
| `mediaTyper.parse(string)` | `polycpp::mime::parse(const std::string&)` | adapted | Returns typed `MediaType`; throws `polycpp::TypeError` on invalid input. |
| `mediaTyper.format(obj)` | `polycpp::mime::format(const MediaType&)` | adapted | Accepts typed struct instead of JavaScript object; validates components and throws `polycpp::TypeError`. |
| `mediaTyper.test(string)` | `polycpp::mime::test(const std::string&)` | adapted | Returns bool for typed string input; empty string returns false in current C++ behavior. |
| `MediaType` object with optional `suffix` | `polycpp::mime::MediaType` | adapted | Uses `std::string suffix`, empty when absent, instead of JavaScript `undefined`. |
| Dynamic non-string arguments | typed C++ signatures | omitted | Non-string JavaScript values are unrepresentable at the C++ call boundary. |
| CommonJS package exports | CMake target `polycpp::mime` and headers under `include/polycpp/mime/` | adapted | Package loading is replaced by C++ target/include conventions. |

Status values:

- `direct`: same behavior with an idiomatic C++ spelling.
- `compatibility layer`: same user-facing behavior through a different implementation shape.
- `adapted`: preserves the upstream intent with a typed C++ API.
- `deferred`: planned future work for an upstream surface or explicitly accepted C++ extension that is intentionally not implemented yet.
- `omitted`: deliberately not part of this port.

## TypeScript Declaration Review

- Declaration source used: upstream packages ship no declarations; `@types/mime-types@3.0.1`, `@types/mime-db@1.43.6`, and `@types/media-typer@1.1.3` were inspected.
- Public APIs, overloads, options, callbacks, streams, or literal unions found only or most clearly in declarations: declarations confirm string-only public functions, `string | false` mime-types results, `charsets.lookup`, public `types` and `extensions` maps, `MimeEntry` fields, and optional `MediaType.suffix`.
- Declaration-only globals, caches, deprecated fields, or runtime-specific surfaces mapped as unsupported/not-applicable: public map exports are adapted to read-only C++ accessors, `charsets.lookup` is omitted in favor of `charset()`, and no callbacks, streams, options, or literal unions are present.

## Framework object boundary review

- Upstream reads or mutates framework/request/response/context objects: none.
- Upstream fields or methods read: none.
- Upstream fields or methods written: none.
- C++ adapter boundary: pure functions over `std::string` and typed `MediaType`; no `polycpp::http::Headers`, request, response, context, or stream adapter is involved.
- Partial mutation risk on validation failure: none because APIs do not mutate caller-owned framework objects.

## Node parity surface review

- Callback APIs: none; internal JavaScript callbacks do not map to public C++ APIs.
- Promise APIs: none.
- EventEmitter APIs: none.
- Server/listener APIs: none; current base polycpp TCP, Unix/IPC, adopted-handle, TLS, HTTP, and HTTPS listener primitives are irrelevant to this package.
- Diagnostic/tracing APIs: upstream `_extensionConflicts` diagnostic export maps to read-only `extensionConflicts()`.
- Stream APIs: none.
- Buffer and binary APIs: none.
- URL, timer, process, and filesystem APIs: Node `path.extname` maps to `polycpp::path::extname`; there is no filesystem access or timeout lifecycle.
- Crypto, compression, TLS, network, and HTTP APIs: none; no transport or protocol API exists.
- Unsupported or non-meaningful Node-specific APIs and audit reason: CommonJS module mechanics, dynamic non-string argument checks, mutable object maps, and JavaScript `false` sentinel returns are omitted or adapted to typed C++ APIs.
