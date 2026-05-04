# Research

- package: mime-types, with embedded mime-db data and media-typer-compatible helpers
- npm url: https://www.npmjs.com/package/mime-types
- source url: https://github.com/jshttp/mime-types.git
- upstream version basis: mime-types 3.0.2, mime-db 1.54.0, media-typer 1.1.0
- upstream revision analyzed: mime-types v3.0.2 `29a0302d799933a45384892df0722f3c5bb1b033`; mime-db v1.54.0 `5207a32f76e77ed2f63421641449f8addeacb0a5`; media-typer v1.1.0 `1332b73ed8584b7b25d556c55b6de9d64fa3ce2c`
- upstream default branch: master for all three upstream repositories
- license: MIT
- license evidence: `package.json` license fields and `LICENSE` files in the published npm artifacts for mime-types 3.0.2, mime-db 1.54.0, and media-typer 1.1.0
- category: foundational compatibility utility for MIME lookup, content-type construction, and RFC 6838 media type parsing

## Package purpose

`mime-types` maps extensions and paths to MIME types, returns default extensions for MIME types, and chooses default charsets using `mime-db`. This companion also exposes `media-typer` parse, format, and test helpers because the pre-libgen implementation combined those upstream surfaces into one `polycpp::mime` package.

## Runtime assumptions

- browser: upstream runtime is ordinary CommonJS JavaScript and not browser-specific; this C++ port is runtime-neutral.
- node.js: upstream `mime-types` assumes Node/CommonJS and uses `path.extname`; `mime-db` exports data through CommonJS; `media-typer` has no Node built-ins.
- filesystem: no filesystem I/O at runtime; path strings are parsed syntactically only.
- network: no network behavior at runtime.
- crypto: no crypto behavior.
- terminal: no terminal behavior except developer scripts and examples.

## Dependency summary

- package.json present: yes for all inspected upstream packages and npm artifacts.
- hard dependencies: `mime-types@3.0.2` depends on `mime-db@^1.54.0`; the existing C++ implementation embeds generated data from `mime-db@1.54.0`.
- peer dependencies: none.
- optional dependencies: none.
- package exports: none declared; Node resolves `index.js`.
- package types: none shipped by upstream packages; `@types/mime-types@3.0.1`, `@types/mime-db@1.43.6`, and `@types/media-typer@1.1.3` were inspected as declaration evidence only.
- package bin: none.
- dependency analysis report: `docs/dependency-analysis.md`

## Upstream repo layout summary

- Clone paths used for analysis: `<repo path>/.tmp/upstream/mime-types`, `<repo path>/.tmp/upstream/mime-db`, and `<repo path>/.tmp/upstream/media-typer`.
- Published artifacts inspected: `<repo path>/.tmp/npm-package` for `mime-types@3.0.2`, `<repo path>/.tmp/npm-artifacts/mime-db` for `mime-db@1.54.0`, and `<repo path>/.tmp/npm-artifacts/media-typer` for `media-typer@1.1.0`.
- `mime-types` is a small CommonJS package with `index.js`, `mimeScore.js`, `README.md`, `HISTORY.md`, `LICENSE`, and `test/test.js`.
- `mime-db` publishes `db.json` and a small `index.js` that exports it.
- `media-typer` is a small CommonJS package with all runtime behavior in `index.js` and tests in `test/test.js`.

## Entry points used by consumers

- `mime-types`: `index.js` via Node default entry resolution; public functions are `lookup`, `contentType`, `extension`, `charset`, plus `charsets.lookup`, `types`, `extensions`, and diagnostic `_extensionConflicts`.
- `mime-db`: `index.js` exporting the database object from `db.json`.
- `media-typer`: `index.js` exporting `parse`, `format`, and `test`.
- TypeScript declarations inspected: upstream packages do not ship declarations; DefinitelyTyped packages were inspected and confirm string-only function APIs, `false` sentinel returns for mime-types, optional `MediaType.suffix`, and the `mime-db` entry schema.

## Important files and why they matter

- `mime-types/index.js`: authoritative runtime behavior for `lookup`, `contentType`, `extension`, `charset`, public maps, and conflict-resolution population.
- `mime-types/mimeScore.js`: current extension conflict scoring algorithm; the generator ports this logic.
- `mime-types/test/test.js`: compatibility cases for lookup, extension, charset, contentType, and path handling.
- `mime-db/db.json`: authoritative MIME records and generated-data source.
- `mime-db/LICENSE`: license notice required for generated embedded data.
- `media-typer/index.js`: authoritative validation grammar and parse/format behavior.
- `media-typer/test/test.js`: compatibility cases for media type parsing, formatting, validation, and error behavior.
- `@types/*/index.d.ts`: supplemental API contract evidence for exported functions, maps, `MediaType`, and `MimeEntry`.

## Files likely irrelevant to the C++ port

- `.github/*`, `.eslintrc*`, `.eslintignore`, `nyc`, `mocha`, and lint configuration are JavaScript project infrastructure.
- Upstream `HISTORY.md` is useful for version context but not runtime behavior.
- `mime-db` fetch/build scripts document data provenance but are not runtime code in this companion.

## Test directories worth mining first

- `mime-types/test/test.js`: primary compatibility source for string return behavior, `false` sentinel cases, path extension handling, and extension conflict smoke reporting.
- `media-typer/test/test.js`: primary compatibility source for RFC 6838 component validation, lowercasing, suffix parsing, and thrown errors.
- `mime-db` has data-generation and database integrity tests upstream; the current C++ port instead needs generated-data provenance tests and representative lookup/extension fixtures.

## Implementation risks discovered from the source layout

- Public upstream mutable map exports are adapted to read-only C++ accessors rather than mutable globals.
- The C++ implementation is header-only; public examples and docs include `<polycpp/mime/mime.hpp>`, while the detail aggregator remains an internal implementation include.
- `scripts/generate_db.py` is path-portable and regenerates `mime_db_data.hpp` from an explicit upstream `db.json`.
- `media-typer` regexes are ASCII-only; the C++ implementation uses explicit ASCII ranges and regression tests reject non-ASCII restricted-name bytes.
- The existing port intentionally improves charset-parameter detection beyond upstream's substring check, so exact upstream parity is not complete for malformed or unusual parameter names and values.

## Companion repo alignment

- companion repos inspected: `cookie`, `content-type`, `commander`, `qs`, and `yaml` locally under `<companion libs root>`.
- CMake target and alias pattern: existing `polycpp_mime` and `polycpp::mime` match the shared target/alias convention; the test target follows the `polycpp_mime_test_<area>` convention.
- public header layout: existing declarations live in `include/polycpp/mime/mime.hpp`, with implementation split under `include/polycpp/mime/detail/`.
- detail/private header strategy: generated data and inline implementations are in `detail/`; this matches older header-only companion style but current public docs should not require detail includes.
- aggregator header strategy: `detail/aggregator.hpp` remains the implementation aggregation point, included internally by the public header so consumers use `<polycpp/mime/mime.hpp>`.
- examples strategy: two runnable examples exist under `examples/` and are wired behind `POLYCPP_MIME_BUILD_EXAMPLES`.
- documentation site strategy: Sphinx/Doxygen docs exist and `docs/build.py` was added during catch-up; old `docs/build.sh` remains for compatibility.
- deliberate deviations from existing companions: the existing port is header-only, combines three npm package surfaces, and keeps a detail aggregator as an internal implementation include; these are recorded in `docs/divergences.md`.

## Polycpp ecosystem reuse analysis

- polycpp core paths inspected: `<polycpp checkout>/include/polycpp`, including `path.hpp`, `http/headers.hpp`, `http/http.hpp`, `net/net.hpp`, `tls/tls.hpp`, `io/*`, `stream/*`, `buffer/*`, `events/*`, and `core/error.hpp`.
- polycpp capability snapshot: `75bc07dfca6ac0aaca07c8748476246e8c18df74` from `git -C <polycpp checkout> rev-parse HEAD` on 2026-05-04.
- transport/listener capability review: TCP (`polycpp::io::TcpAcceptor`, `polycpp::net::Server`), Unix/IPC path (`polycpp::io::PipeAcceptor` and `PipeSocket`), cross-transport (`StreamSocket` and `StreamAcceptor`), adopted handles (`polycpp::net::NativeListenHandle`), TLS client (`polycpp::io::TlsStream`, `polycpp::tls::TLSSocket`), and TLS server (`polycpp::tls::Server`, `polycpp::tls::createServer`) exist but are not selected because this package has no listener or transport API.
- polycpp core types/functions selected: `polycpp::path::extname` for `mime-types` path extension semantics and `polycpp::TypeError` for media-typer validation errors.
- polycpp core types/functions rejected: `polycpp::http::Headers` and the `content-type` companion are not used in the existing `contentType()` helper because it returns a single header value string and does not mutate a header object; `polycpp::Buffer`, streams, events, HTTP requests/responses, sockets, TLS, timers, and JSON are not required by the implemented public API.
- public polycpp interop review: ordinary public values are text strings and a stable `MediaType` struct; no `polycpp::String`, `JsonValue`, `Date`, `Buffer`, stream, or HTTP header object is required for the current surface.
- string policy: public text APIs use `std::string` and UTF-8-compatible byte strings; upstream behavior is ASCII-oriented for MIME labels and does not require JavaScript UTF-16 code-unit semantics.
- JsonValue/Object/Array policy: not selected because upstream APIs are string functions and stable data structs, not open JSON object inputs.
- Date/time interop policy: not applicable because upstream exposes no date/time values.
- diagnostic/config object policy: no configuration object is exposed; upstream `_extensionConflicts` is adapted to read-only `extensionConflicts()` diagnostic records.
- toJSON/stringify policy: not applicable because public objects do not model upstream `toJSON()` behavior.
- companion libs inspected for reusable APIs: `content-type` for header parsing/formatting, `cookie` and `qs` for small utility style, `commander` and `yaml` for public API/header organization, and the current `mime` implementation for existing behavior.
- companion libs selected for reuse: none in the existing implementation; the core `polycpp` dependency is sufficient.
- companion libs rejected or not selected: `content-type` could parse parameters, but pulling it in only to detect an existing charset parameter is unnecessary for the current small helper and would be a future design change.
- new local abstractions introduced: `MediaType`, public `TypeEntry` and `ExtensionConflict`, generated `MimeEntry`, and `Compressible`; these are package-specific representations of upstream media type components, map entries, conflict diagnostics, and `mime-db` data.
- reuse risks or integration gaps: none remaining from the 2026-05-04 catch-up audit; future work should track new upstream releases and data refreshes.

## Node parity surface audit

- callback APIs: none; analyzer's callback signal comes from ordinary `forEach(function ...)` implementation style, not a public callback API.
- Promise APIs: none.
- EventEmitter APIs: none.
- server/listener APIs: none; base TCP, Unix/IPC, adopted-handle, TLS, HTTP, and HTTPS listener primitives were inspected and are irrelevant to this package.
- diagnostic/tracing APIs: upstream `_extensionConflicts` is a diagnostic export in `mime-types@3.0.2`; the C++ port exposes read-only equivalent records through `extensionConflicts()`.
- stream APIs: none.
- Buffer and binary APIs: none.
- URL, timer, process, and filesystem APIs: `mime-types` uses Node `path.extname`; the C++ port maps this to `polycpp::path::extname` and performs no filesystem access, permissions checks, symlink handling, or path normalization beyond extension parsing.
- crypto, compression, TLS, network, and HTTP APIs: none; `mime-db` stores a `compressible` data field but the current public C++ API does not expose compression behavior.
- unsupported Node-specific APIs and audit reason: CommonJS exports, dynamic non-string argument handling, object maps with mutable properties, and JavaScript `false` sentinel values are adapted or omitted in favor of typed C++ parameters, read-only accessors, and `std::optional`.

## External SDK and native driver strategy

- upstream external services/protocols: not applicable because the package is a local lookup/parser utility with no external service or wire protocol.
- native SDKs/client libraries to use: not applicable; a native MIME database would risk changing package-specific aliases and extension conflict behavior.
- SDKs/protocols explicitly not reimplemented: not applicable because no external SDK or protocol is part of upstream runtime behavior.
- adapter/linking strategy: no SDK links; the target links only to `polycpp`.
- test environment needs: standard C++20 build, GoogleTest, and generated data fixture coverage; no service process.

## Compatibility foundation review

- downstream dependency role: foundational compatibility package; many HTTP and web helpers depend on MIME lookup semantics rather than only IANA standards.
- native substitution risk: high for data and conflict scoring because `mime-db` aliases, sources, extensions, and `mimeScore` conflict resolution are package-specific.
- upstream implementation data to preserve: `mime-db@1.54.0` `db.json`, `mime-types` `mimeScore.js` scoring, media-typer RFC 6838 restricted-name regex behavior, and representative upstream tests.
- generated or vendored data plan: generated header `include/polycpp/mime/detail/mime_db_data.hpp` comes from `mime-db v1.54.0 db.json` through path-portable `scripts/generate_db.py`; license notices are recorded in `THIRD_PARTY_LICENSES.md`.
- compatibility fixture strategy: keep representative `mime-types/test/test.js` and `media-typer/test/test.js` cases in `tests/test_mime.cpp`, plus read-only data access, conflict diagnostics, and locale-independent ASCII rejection tests.

## Security and fail-closed review

- security-sensitive behavior: low; this package does not authenticate or authorize, but media type validation and charset detection operate on user-controlled header/path strings.
- trust boundary: callers may pass untrusted file names, extensions, or header values; the port must return `std::nullopt`, `false`, or throw `polycpp::TypeError` without mutating external state.
- supported protocol or algorithm matrix: not applicable because there is no protocol or cryptographic algorithm; supported grammar is MIME lookup plus RFC 6838 media type names.
- unsupported behavior and fail-closed policy: unsupported dynamic JS values are unrepresentable in typed C++; invalid media type strings fail by returning false from `test()` or throwing `polycpp::TypeError` from `parse()` and `format()`.
- result-set/framing drain policy, if protocol client: not applicable because there is no protocol client.
- binary payload type-mapping policy, if protocol client: not applicable because there are no binary payload APIs.
- stateful parser/session-state policy, if protocol client/server: not applicable because the parser is stateless and has no session.
- server/listener response writer matrix, if protocol server surface exists: not applicable because no server surface exists.
- key, secret, credential, or user-controlled input handling: no secrets or credentials; user-controlled strings are bounded by `std::string` size and parsed synchronously.
- misuse cases that must be tested: unknown extensions, extensionless paths, invalid media type grammar, empty strings, non-ASCII restricted-name bytes, existing charset parameters, and strings containing `charset` only as a substring.

## Core use cases

- Determine a MIME type from a file path, dotted extension, or bare extension.
- Build a Content-Type header with a default charset when appropriate.
- Find the default extension for a MIME type.
- Determine the default charset for text and database-declared MIME types.
- Parse, validate, and format RFC 6838 media type values and structured suffixes.

## Key features to port first

- Already implemented: `lookup`, `contentType`, `extension`, `charset`, `MediaType`, `parse`, `format`, and `test`.
- Already implemented: generated `mime-db@1.54.0` table with `mimeScore` conflict resolution.
- Already implemented: read-only `types`, `extensions`, and `extensionConflicts` accessors.
- Already implemented: representative compatibility tests from `mime-types` and `media-typer`.

## Features to defer

- None remaining from the 2026-05-04 libgen catch-up audit.

## Non-parity extension candidates

- Integration helpers for `polycpp::http::Headers` could set Content-Type directly, but upstream does not expose a header-object mutator and this is only a future C++ convenience proposal.
- Optional content negotiation helpers belong in other companion packages and are not part of mime-types, mime-db, or media-typer parity.

## v0 scope

- port version: 1.0.0
- versioning note: port version is independent from upstream versioning
- supported APIs: `polycpp::mime::lookup`, `contentType`, `extension`, `charset`, `types`, `extensions`, `extensionConflicts`, `MediaType`, `parse`, `format`, and `test`.
- unsupported APIs: alias object `charsets.lookup`, CommonJS package mechanics, mutable JavaScript object export semantics, and dynamic non-string argument behavior.
- dependency plan: keep `mime-db` data generated in this repo with MIT notice; do not create separate companion repos for mime-db or media-typer because the existing port intentionally combines these tiny foundational surfaces.
- polycpp modules to use: `polycpp::path::extname`, `polycpp::TypeError`, and base `polycpp` CMake target.
- missing polycpp primitives: none for the current synchronous lookup/parser scope; no connect deadlines, per-operation deadlines, streams, sockets, TLS, or server lifecycle exist in this package.
