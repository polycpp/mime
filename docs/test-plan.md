# Test Plan

## Unit tests

- `lookup()` extension and path extraction, including bare extensions, dotted extensions, paths, Windows separators, dotfiles, multiple dots, mixed case, empty strings, and missing extensions.
- `contentType()` extension lookup, MIME type passthrough, default charset append, existing charset parameter detection, and unknown extension behavior.
- `extension()` MIME type lookup, parameter stripping, whitespace around the type, unknown types, and empty strings.
- `charset()` database charset lookup, `text/*` fallback, case-insensitive input, unknown types, and empty strings.
- `types()`, `extensions()`, and `extensionConflicts()` read-only data accessors for upstream map and diagnostic parity.
- `parse()`, `format()`, and `test()` validation for RFC 6838 type, subtype, suffix, invalid separators, and invalid characters.
- Generated-data lookup fixtures for representative mime-db entries and extension conflict scoring.

## Integration tests

- Content-Type generation for text and binary types using public `polycpp::mime` APIs together.
- Lookup and parse roundtrip for structured suffix media types such as `image/svg+xml`.
- Extension and lookup roundtrip for common database records.
- Example workflows from `examples/content_type_cli.cpp` and `examples/media_type_router.cpp` compile through CMake when `POLYCPP_MIME_BUILD_EXAMPLES` is enabled.

## Compatibility tests adapted from upstream

- upstream compatibility layout: current aggregate file `tests/test_mime.cpp` adapts representative cases from `mime-types/test/test.js` and `media-typer/test/test.js`.
- upstream-to-local coverage map:
  - `mime-types/test/test.js` charset cases -> `MimeCharsetTest.*` in `tests/test_mime.cpp`
  - `mime-types/test/test.js` contentType extension/type cases -> `MimeContentTypeTest.*`
  - `mime-types/test/test.js` extension cases -> `MimeExtensionTest.*`
  - `mime-types/test/test.js` lookup extension/path/dotfile cases -> `MimeLookupTest.*`
  - `mime-types` public map and `_extensionConflicts` diagnostics -> `MimeDataAccessTest.*`
  - `media-typer/test/test.js` parse cases -> `MediaTyperParseTest.*`
  - `media-typer/test/test.js` format cases -> `MediaTyperFormatTest.*`
  - `media-typer/test/test.js` validation cases -> `MediaTyperTestTest.*`
- omitted upstream cases:
  - non-string JavaScript arguments are not applicable because C++ signatures accept typed `std::string`.
  - Mocha leak checks, lint checks, and package loading mechanics are JavaScript runtime infrastructure.
  - `_extensionConflicts` console output is adapted to read-only `extensionConflicts()` records instead of JavaScript console output.
  - Full mime-db generated-data integrity can expand during future mime-db version bumps; current tests pin representative records, extension roundtrips, and conflict diagnostics.

## Security and fail-closed tests

- Invalid media type strings must fail closed by throwing `polycpp::TypeError` from `parse()` and `format()` or returning false from `test()`.
- Existing charset parameter detection must only suppress appending when the parameter name is exactly `charset`, case-insensitively and allowing whitespace around `=`.
- Strings containing `charset` in another parameter name or value must not suppress the default charset append.
- Locale-independent ASCII rejection is covered for non-ASCII restricted-name bytes such as `text/p£ain`.
- Empty strings and unknown MIME values must return `std::nullopt` or false without external mutation.

## Protocol/client tests

Not applicable because this package has no database, cache, queue, cloud-service, wire-protocol, server, listener, transport, authentication, packet, binary payload, pool, or session lifecycle behavior.

## Release-blocking behaviors

- Representative upstream compatibility cases in `tests/test_mime.cpp` pass.
- Generated `mime-db` data provenance and third-party license notices are documented.
- Public API divergences around read-only maps, dynamic arguments, charset detection, and media-typer whitespace are documented.
- The libgen catch-up audit findings table is resolved or explicitly documented before release.
- Documentation builds with `python3 docs/build.py` after catch-up scaffold updates.

## Current validation

- `python3 scripts/generate_db.py --db-json .tmp/upstream/mime-db/db.json` -> passed; regenerated 2,601 MIME entries, 1,246 unique extension mappings, and 3 conflict diagnostics.
- `cmake -S . -B build-deferred-fix -DCMAKE_BUILD_TYPE=Debug -DPOLYCPP_SOURCE_DIR=<polycpp checkout>` -> passed; standalone defaults built tests and examples.
- `cmake --build build-deferred-fix -j2` -> passed, including `polycpp_mime_test_mime`, `content_type_cli`, and `media_type_router`.
- `ctest --test-dir build-deferred-fix --output-on-failure` -> passed, 93 tests passed and 0 failed.
- `./build-deferred-fix/examples/content_type_cli index.html .json bogus` -> passed smoke check for text, JSON, and fallback output.
- `printf 'application/vnd.api+json\nbogus\n' | ./build-deferred-fix/examples/media_type_router` -> passed smoke check for JSON API routing and invalid input.
- `python3 docs/build.py` -> passed.
- `python3 <libgen checkout>/scripts/check-port-readiness.py --strict <repo path>` -> passed, `port readiness strict checks passed`.
- `python3 <libgen checkout>/scripts/check-port-validation.py --run-docs-build <repo path>` -> passed, `post-implementation validation checks passed`.
- Environment notes: GCC 11.4.0 emitted a polycpp recommendation warning for GCC 12+; local base polycpp checkout was `<polycpp checkout>` at `75bc07dfca6ac0aaca07c8748476246e8c18df74`.
