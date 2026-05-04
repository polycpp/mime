# Dependency and JavaScript API Analysis

- package: mime-types, plus manually inspected mime-db and media-typer surfaces included by the existing port
- package version: mime-types 3.0.2, mime-db 1.54.0, media-typer 1.1.0
- package root: `<repo path>/.tmp/npm-package`
- analyzer json: `.tmp/dependency-analysis.json`
- published npm artifact path: `.tmp/npm-package`, `.tmp/npm-artifacts/mime-db`, `.tmp/npm-artifacts/media-typer`
- published npm artifact analyzed: yes; mime-types was analyzed by `scripts/analyze-upstream-js.py`, and mime-db/media-typer artifacts were manually inspected
- include dev dependencies: no
- dependency source install used: analyzer temporary npm install with scripts disabled
- companion root checked: `<companion libs root>`

## Package entry metadata

- main: none declared by package metadata; Node default entry is `index.js`
- module: none
- types: none shipped by upstream packages
- exports: none
- bin: none
- missing declared entries in repo clone: none
- TypeScript source files detected: none
- TypeScript declarations reviewed: `@types/mime-types@3.0.1`, `@types/mime-db@1.43.6`, and `@types/media-typer@1.1.3`
- declaration-source decision: use DefinitelyTyped declarations as secondary API evidence only; runtime source and tests remain authoritative
- source-vs-published artifact decision: published `mime-types@3.0.2` artifact matches the source entry shape; published `mime-db@1.54.0` `db.json` is the data source of truth; published `media-typer@1.1.0` `index.js` is the runtime source of truth

## Direct dependencies

- `mime-types@3.0.2` has one hard runtime dependency: `mime-db@^1.54.0`, installed as `1.54.0`.
- `media-typer@1.1.0` has no runtime dependencies and is included here as an additional upstream surface, not as a `mime-types` dependency.
- Dev dependencies such as ESLint, Mocha, nyc, csv-parse, stream-to-array, and undici are upstream maintenance/test tools and are not shipped in this C++ companion.

## Dependency ownership decisions

| Package | Kind | Requested | Installed | License | License evidence | License impact | License strategy | Affects repo license | Deps | Source files | Node API calls | JS API calls | Recommendation | Rationale |
|---|---|---|---|---|---|---|---|---|---:|---:|---:|---:|---|---|
| `mime-db` | hard runtime dependency of `mime-types` | `^1.54.0` | `1.54.0` | MIT | `package.json` license field and published `LICENSE` file inspected | permissive | permissive dependency ok with notice | no | 0 | 1 | 0 | 0 | implement private helper in this repo | Existing implementation already embeds generated `db.json` data; creating a separate companion would add dependency overhead for a static compatibility table. |
| `media-typer` | supplemental upstream surface combined by existing port | not a dependency | `1.1.0` artifact inspected | MIT | `package.json` license field and published `LICENSE` file inspected | permissive | permissive dependency ok with notice | no | 0 | 1 | 0 | 0 | implement private helper in this repo | Existing implementation exposes parse, format, and test as `polycpp::mime`; the package is small and has no runtime dependency graph. |

## License impact summary

- upstream package license: MIT for mime-types, mime-db, and media-typer.
- repo license decision: keep repo MIT.
- GPL/AGPL dependencies: none.
- LGPL/MPL dependencies: none.
- permissive dependencies requiring notices: mime-types, mime-db, and media-typer MIT notices.
- dev/test-only dependencies excluded from shipped artifacts: ESLint, Mocha, nyc, csv-parse, stream-to-array, undici, and DefinitelyTyped packages used only for planning evidence.
- dependency license notices to add to `THIRD_PARTY_LICENSES.md`: mime-types 3.0.2, mime-db 1.54.0, and media-typer 1.1.0 notice text and data provenance.

## Transitive dependency summary

- Runtime transitive dependency graph is one level: `mime-types -> mime-db`.
- `mime-db` and `media-typer` have no runtime dependencies in their published artifacts.
- The generated C++ target links only to `polycpp`.

## Runtime API usage

### Target package

- entry points analyzed: `mime-types/index.js` and `mime-types/mimeScore.js`.
- source files analyzed by analyzer: 2 for mime-types, 1 for installed mime-db.
- source files manually inspected: `mime-types/index.js`, `mime-types/mimeScore.js`, `mime-types/test/test.js`, `mime-db/db.json`, `mime-db/index.js`, `media-typer/index.js`, `media-typer/test/test.js`, and DefinitelyTyped declaration files.
- external imports seen from target: `mime-db`, Node `path`, and local `./mimeScore`.

### Analyzer porting gates

- polycpp reuse hints consumed: `path.extname` mapped to `polycpp::path::extname`.
- Node parity hints consumed: callback signal reviewed as internal JS callback style; filesystem-path signal reviewed as syntactic path extension parsing.
- security hints consumed: analyzer reported `securitySensitive: false`; manual review still treats parser validation as user-input fail-closed behavior.
- security-sensitive package: no auth, crypto, or protocol boundary; low-risk parser/lookup boundary only.
- polycpp capability snapshot consumed: `75bc07dfca6ac0aaca07c8748476246e8c18df74` on 2026-05-04.
- transport/listener capability hints consumed: base TCP, Unix/IPC, adopted handle, TLS client, and TLS server primitives were searched and marked not applicable.

### Node.js API usage

- Analyzer found `path.extname` in `mime-types` and no other Node built-ins.
- The C++ implementation uses `polycpp::path::extname` to preserve extension extraction semantics.
- No filesystem access is performed; path strings are not opened, resolved, normalized, or permission-checked.

### Node parity surface usage

- callbacks: no public callback API; analyzer signal comes from internal JavaScript `forEach(function ...)`.
- Promise APIs: none.
- EventEmitter APIs: none.
- server/listener APIs: none; listener modes are irrelevant after current polycpp transport inspection.
- diagnostic/tracing APIs: upstream `_extensionConflicts` diagnostic export maps to read-only `extensionConflicts()` records.
- streams: none.
- Buffer and binary data: none.
- URL/timer/process/filesystem APIs: Node `path.extname` only, adapted to `polycpp::path::extname`; no deadlines or post-timeout resource state exist.
- crypto/compression/TLS/network/HTTP APIs: none; `compressible` appears as data in `mime-db` but no public compression API is implemented.

### JavaScript API usage

- Analyzer found `String.prototype.toLowerCase`, `String.prototype.indexOf`, `Array.prototype.indexOf`, `Array.prototype.push`, `Object.create`, `Object.keys`, `RegExp.prototype.exec`, and `RegExp.prototype.test`.
- C++ equivalents are ordinary `std::string`, `std::optional`, generated sorted arrays, binary search, and validation helpers.
- Dynamic false sentinel returns map to `std::optional<std::string>` for lookup-style APIs and `bool` for `test()`.

### Framework object boundary usage

- analyzer-reported target-package framework object accesses: none.
- analyzer-reported dependency framework object accesses: none.
- manual review decision: pure string/data APIs; there is no request, response, context, header object mutation, or stream boundary.

## Porting decisions

- Keep `mime-db` as generated package data in this repo because it is the compatibility source of truth and has no runtime behavior beyond exporting JSON.
- Keep `media-typer` helpers in this repo for catch-up because the existing public surface already combines them under `polycpp::mime`.
- Prefer generated upstream-derived data over a native MIME database.
- Preserve typed C++ return values and exceptions instead of JavaScript dynamic argument and `false` sentinel overloads.
- Record behavior changes around charset parameter detection and generic whitespace handling as divergences.

## Analyzer warnings

- none emitted by analyzer
