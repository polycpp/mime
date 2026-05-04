#pragma once

/**
 * @file mime.hpp
 * @brief C++ port of npm mime-types, mime-db, and media-typer for polycpp.
 *
 * Provides MIME type lookup by file extension, content-type generation,
 * extension lookup by MIME type, charset detection, and RFC 6838 media
 * type parsing/formatting/validation.
 *
 * @see https://www.npmjs.com/package/mime-types
 * @see https://www.npmjs.com/package/mime-db
 * @see https://www.npmjs.com/package/media-typer
 */

#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace polycpp {
namespace mime {

// ============================================================================
// mime-types API
// ============================================================================

/**
 * @brief Look up the MIME type for a file path or extension.
 *
 * Accepts a file path, extension with dot (`.html`), or bare extension
 * (`html`). The lookup is case-insensitive.
 *
 * @param pathOrExt A file path, `.ext`, or bare extension.
 * @return The MIME type string, or std::nullopt if not found.
 *
 * @par Example
 * @code{.cpp}
 *   mime::lookup("file.html");       // "text/html"
 *   mime::lookup(".json");           // "application/json"
 *   mime::lookup("txt");             // "text/plain"
 *   mime::lookup("/path/to/f.js");   // "text/javascript"
 *   mime::lookup("bogus");           // std::nullopt
 * @endcode
 *
 * @see https://www.npmjs.com/package/mime-types#mimelookuppath
 */
std::optional<std::string> lookup(const std::string& pathOrExt);

/**
 * @brief Create a full Content-Type header value.
 *
 * If the input has no `/`, it is treated as an extension and looked up.
 * If the MIME type has a known charset and the input does not already
 * contain a `charset` parameter, `; charset=...` is appended.
 *
 * @param typeOrExt A MIME type or file extension.
 * @return The full content-type string, or std::nullopt if the extension
 *         is unknown.
 *
 * @par Example
 * @code{.cpp}
 *   mime::contentType("html");
 *   // "text/html; charset=utf-8"
 *   mime::contentType("application/json");
 *   // "application/json; charset=utf-8"
 *   mime::contentType("text/html; charset=iso-8859-1");
 *   // "text/html; charset=iso-8859-1"  (unchanged)
 * @endcode
 *
 * @see https://www.npmjs.com/package/mime-types#mimecontenttypestr
 */
std::optional<std::string> contentType(const std::string& typeOrExt);

/**
 * @brief Get the default file extension for a MIME type.
 *
 * Strips any parameters (e.g., `; charset=utf-8`) before lookup.
 *
 * @param mimeType A MIME type string, optionally with parameters.
 * @return The default extension (without dot), or std::nullopt.
 *
 * @par Example
 * @code{.cpp}
 *   mime::extension("text/html");                // "html"
 *   mime::extension("text/html; charset=UTF-8"); // "html"
 *   mime::extension("application/x-bogus");      // std::nullopt
 * @endcode
 *
 * @see https://www.npmjs.com/package/mime-types#mimeextensiontype
 */
std::optional<std::string> extension(const std::string& mimeType);

/**
 * @brief Get the default charset for a MIME type.
 *
 * Returns the charset from the mime-db if one is defined. Falls back
 * to `"UTF-8"` for all `text/` types. Returns std::nullopt for
 * types with no known charset.
 *
 * @param mimeType A MIME type string, optionally with parameters.
 * @return The charset string (e.g., `"UTF-8"`), or std::nullopt.
 *
 * @par Example
 * @code{.cpp}
 *   mime::charset("text/html");          // "UTF-8"
 *   mime::charset("application/json");   // "UTF-8"
 *   mime::charset("image/png");          // std::nullopt
 * @endcode
 *
 * @see https://www.npmjs.com/package/mime-types#mimecharsettype
 */
std::optional<std::string> charset(const std::string& mimeType);

/**
 * @brief Read-only entry from the extension-to-MIME-type map.
 *
 * This is the C++ equivalent of one entry in upstream `mime.types`.
 */
struct TypeEntry {
    std::string_view extension;  ///< File extension without a leading dot.
    std::string_view type;       ///< Preferred MIME type for the extension.
};

/**
 * @brief Diagnostic record for an extension whose preferred MIME type changed
 *        under upstream mimeScore conflict resolution.
 *
 * This mirrors the upstream `_extensionConflicts` diagnostic export without
 * exposing mutable global maps.
 */
struct ExtensionConflict {
    std::string_view extension;      ///< File extension without a leading dot.
    std::string_view legacyType;     ///< MIME type selected by legacy scoring.
    std::string_view preferredType;  ///< MIME type selected by mimeScore.
};

/**
 * @brief Return the read-only extension-to-MIME-type map.
 *
 * The returned span points at static generated data and remains valid for the
 * lifetime of the program.
 *
 * @return Extension-to-type entries sorted by extension.
 */
std::span<const TypeEntry> types();

/**
 * @brief Return all known extensions for a MIME type.
 *
 * This is the read-only C++ equivalent of upstream `mime.extensions[type]`.
 * Parameters are stripped and lookup is case-insensitive.
 *
 * @param mimeType A MIME type string, optionally with parameters.
 * @return Extensions without leading dots; empty when the type is unknown or
 *         has no extensions.
 */
std::vector<std::string> extensions(const std::string& mimeType);

/**
 * @brief Return extension conflict diagnostics from upstream mimeScore.
 *
 * The returned span points at static generated data and remains valid for the
 * lifetime of the program.
 *
 * @return Extension conflict records sorted by generation order.
 */
std::span<const ExtensionConflict> extensionConflicts();

// ============================================================================
// media-typer API
// ============================================================================

/**
 * @brief Parsed media type components per RFC 6838.
 *
 * @see https://www.npmjs.com/package/media-typer
 */
struct MediaType {
    std::string type;       ///< Primary type (e.g., "text", "application")
    std::string subtype;    ///< Subtype (e.g., "html", "vnd.api")
    std::string suffix;     ///< Structured syntax suffix (e.g., "xml", "json"), empty if none
};

/**
 * @brief Parse a media type string into its components.
 *
 * Validates the format per RFC 6838. Input is lowercased. Leading and
 * trailing whitespace is trimmed.
 *
 * @param mediaType A media type string (e.g., `"application/vnd.api+json"`).
 * @return A MediaType with type, subtype, and optional suffix.
 * @throws polycpp::TypeError If the string is not a valid media type.
 *
 * @par Example
 * @code{.cpp}
 *   auto mt = mime::parse("application/vnd.api+json");
 *   // mt.type == "application"
 *   // mt.subtype == "vnd.api"
 *   // mt.suffix == "json"
 * @endcode
 *
 * @see https://www.npmjs.com/package/media-typer#typerparsestring
 */
MediaType parse(const std::string& mediaType);

/**
 * @brief Format a MediaType object into a media type string.
 *
 * Validates type, subtype, and suffix components.
 *
 * @param mt The MediaType to format.
 * @return The formatted string (e.g., `"application/vnd.api+json"`).
 * @throws polycpp::TypeError If any component is invalid.
 *
 * @par Example
 * @code{.cpp}
 *   MediaType mt{"image", "svg", "xml"};
 *   mime::format(mt); // "image/svg+xml"
 * @endcode
 *
 * @see https://www.npmjs.com/package/media-typer#typerformatobj
 */
std::string format(const MediaType& mt);

/**
 * @brief Test whether a string is a valid media type.
 *
 * @param mediaType A string to test.
 * @return true if the string matches the RFC 6838 media type format.
 *
 * @par Example
 * @code{.cpp}
 *   mime::test("text/html");       // true
 *   mime::test("image/svg+xml");   // true
 *   mime::test("bogus");           // false
 * @endcode
 *
 * @see https://www.npmjs.com/package/media-typer#typerteststring
 */
bool test(const std::string& mediaType);

} // namespace mime
} // namespace polycpp

#include <polycpp/mime/detail/aggregator.hpp>
