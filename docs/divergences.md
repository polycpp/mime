# Divergences From Upstream

## Deferred Features

- None remaining from the 2026-05-04 libgen catch-up audit. The read-only data accessors, conflict diagnostics, portable generator workflow, example targets, public header cleanup, and ASCII-only media type validation follow-ups are resolved.

## Deliberate Behavior Changes

- `contentType()` detects a real `charset` parameter instead of using upstream's broad substring check, so strings such as `text/html; foo=my-charset-token` still receive a default charset.
- `media-typer` parsing and testing currently trim generic C whitespace, while upstream `media-typer@1.1.0` regexes accept literal spaces around the type.
- JavaScript `false` sentinel returns are mapped to `std::optional<std::string>` for lookup-style APIs.
- `mediaTyper.test("")` returns `false` in the current C++ API, while upstream throws for missing or falsy arguments.
- The existing companion combines `mime-types`, `mime-db`, and `media-typer` under one `polycpp::mime` namespace instead of publishing separate companion packages.

## Unsupported Runtime-Specific Features

- CommonJS package loading, `require`, and object export mutation are not meaningful C++ library surfaces.
- Dynamic non-string argument behavior is not reproduced because C++ public functions are typed.
- Public mutable JavaScript map exports (`types`, `extensions`, `charsets`) are not exposed as mutable C++ globals.
- Upstream dev-tool behavior from ESLint, Mocha, nyc, and package scripts is not part of this C++ companion.

## Audit findings (libgen catch-up)

| ID | Severity | Location | Description | Resolution |
|---|---|---|---|---|
| AF-2026-05-04-A | medium | `include/polycpp/mime/mime.hpp` | Public declarations did not include inline definitions, so README, docs, tests, and examples included `detail/aggregator.hpp`. | resolved 2026-05-04: public users include `<polycpp/mime/mime.hpp>`; the public header includes the implementation aggregator internally and `MimeDataAccessTest.ExposesReadOnlyTypesMap` pins the new read-only surface. |
| AF-2026-05-04-B | medium | `include/polycpp/mime/detail/media_typer_impl.hpp` | RFC 6838 validation used locale-sensitive `std::isalnum`, while upstream regexes are ASCII-only. | resolved 2026-05-04: validation uses explicit ASCII ranges and non-ASCII restricted-name bytes are covered by parse/test regression tests. |
| AF-2026-05-04-C | low | `scripts/generate_db.py` | The data generator hardcoded local absolute input and output paths, so future mime-db regeneration was not reproducible from a fresh clone. | resolved 2026-05-04: generator accepts `--db-json`, writes repo-relative output by default, records mime-db version provenance, and regenerated `mime_db_data.hpp` exposes conflict diagnostics. |
| AF-2026-05-04-D | low | `CMakeLists.txt` | Tests defaulted on when consumed as a subproject, the test target name was generic, and examples were not wired by CMake. | resolved 2026-05-04: tests/examples default on only for standalone builds, the test target is `polycpp_mime_test_mime`, GoogleTest fetch uses HTTPS, and examples are built behind `POLYCPP_MIME_BUILD_EXAMPLES`. |
