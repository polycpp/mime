#pragma once

/**
 * @file mime_types_impl.hpp
 * @brief Implementation of mime-types lookup functions.
 *
 * Uses binary search on the compile-time sorted arrays from mime_db_data.hpp.
 */

#include <polycpp/mime/mime.hpp>
#include <polycpp/mime/detail/mime_db_data.hpp>
#include <polycpp/path.hpp>

#include <algorithm>
#include <cctype>
#include <string>

namespace polycpp {
namespace mime {
namespace detail {

/**
 * @brief Convert a string to lowercase in-place and return it.
 */
inline std::string toLower(std::string s) {
    for (auto& c : s) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return s;
}

/**
 * @brief Binary search for a MIME type in the sorted MIME_DB array.
 * @return Pointer to the MimeEntry, or nullptr if not found.
 */
inline const MimeEntry* findMimeEntry(std::string_view type) {
    auto it = std::lower_bound(
        MIME_DB.begin(), MIME_DB.end(), type,
        [](const MimeEntry& entry, std::string_view t) {
            return entry.type < t;
        }
    );
    if (it != MIME_DB.end() && it->type == type) {
        return &(*it);
    }
    return nullptr;
}

/**
 * @brief Binary search for an extension in the sorted EXT_TO_MIME array.
 * @return Pointer to the ExtEntry, or nullptr if not found.
 */
inline const ExtEntry* findExtEntry(std::string_view ext) {
    auto it = std::lower_bound(
        EXT_TO_MIME.begin(), EXT_TO_MIME.end(), ext,
        [](const ExtEntry& entry, std::string_view e) {
            return entry.ext < e;
        }
    );
    if (it != EXT_TO_MIME.end() && it->ext == ext) {
        return &(*it);
    }
    return nullptr;
}

/**
 * @brief Extract the MIME type portion from a content-type string.
 *
 * Strips leading whitespace, captures everything up to the first
 * semicolon, space, or end-of-string.
 */
inline std::string extractType(const std::string& str) {
    // Skip leading whitespace
    size_t start = 0;
    while (start < str.size() && std::isspace(static_cast<unsigned char>(str[start]))) {
        ++start;
    }
    // Find end: semicolon, space, or end of string
    size_t end = start;
    while (end < str.size() &&
           str[end] != ';' &&
           !std::isspace(static_cast<unsigned char>(str[end]))) {
        ++end;
    }
    return std::string(str, start, end - start);
}

/**
 * @brief Check whether a content-type string already has a charset parameter.
 *
 * Parses semicolon-separated parameters and only matches an actual parameter
 * named `charset`, ignoring other keys or values that merely contain the
 * substring.
 */
inline bool hasCharsetParameter(const std::string& str) {
    size_t pos = 0;

    while (true) {
        pos = str.find(';', pos);
        if (pos == std::string::npos) {
            return false;
        }
        ++pos;

        while (pos < str.size() && std::isspace(static_cast<unsigned char>(str[pos]))) {
            ++pos;
        }

        size_t keyStart = pos;
        while (pos < str.size() && str[pos] != '=' && str[pos] != ';') {
            ++pos;
        }

        size_t keyEnd = pos;
        while (keyEnd > keyStart &&
               std::isspace(static_cast<unsigned char>(str[keyEnd - 1]))) {
            --keyEnd;
        }

        if (keyEnd > keyStart &&
            detail::toLower(std::string(str, keyStart, keyEnd - keyStart)) == "charset") {
            return pos < str.size() && str[pos] == '=';
        }
    }
}

} // namespace detail

// ============================================================================
// mime-types function implementations
// ============================================================================

inline std::optional<std::string> lookup(const std::string& pathOrExt) {
    if (pathOrExt.empty()) {
        return std::nullopt;
    }

    // Use polycpp::path::extname to extract extension.
    // Prepend "x." to handle bare extensions and ".ext" inputs correctly,
    // matching the JS: extname('x.' + path).toLowerCase().slice(1)
    std::string ext = polycpp::path::extname("x." + pathOrExt);

    if (ext.empty()) {
        return std::nullopt;
    }

    // Remove leading dot and lowercase
    ext = detail::toLower(ext.substr(1));

    if (ext.empty()) {
        return std::nullopt;
    }

    auto* entry = detail::findExtEntry(ext);
    if (!entry) {
        return std::nullopt;
    }
    return std::string(entry->mime_type);
}

inline std::optional<std::string> contentType(const std::string& typeOrExt) {
    if (typeOrExt.empty()) {
        return std::nullopt;
    }

    std::string mime;

    // If no '/', treat as extension and look up
    if (typeOrExt.find('/') == std::string::npos) {
        auto result = lookup(typeOrExt);
        if (!result) {
            return std::nullopt;
        }
        mime = std::move(*result);
    } else {
        mime = typeOrExt;
    }

    // Only skip if an actual `charset` parameter is already present (case
    // insensitive).  A plain substring check would misfire on parameter
    // values or names that merely contain "charset".
    if (detail::hasCharsetParameter(mime)) {
        return mime;
    }

    // Try to append charset
    auto cs = charset(mime);
    if (cs) {
        std::string csLower = detail::toLower(*cs);
        mime += "; charset=" + csLower;
    }

    return mime;
}

inline std::optional<std::string> extension(const std::string& mimeType) {
    if (mimeType.empty()) {
        return std::nullopt;
    }

    // Extract the type portion (strip parameters)
    std::string type = detail::toLower(detail::extractType(mimeType));
    if (type.empty()) {
        return std::nullopt;
    }

    // Look up in MIME_DB
    auto* entry = detail::findMimeEntry(type);
    if (!entry || entry->ext_count == 0) {
        return std::nullopt;
    }

    // Return the first extension
    return std::string(detail::EXTENSIONS[entry->ext_offset]);
}

inline std::optional<std::string> charset(const std::string& mimeType) {
    if (mimeType.empty()) {
        return std::nullopt;
    }

    // Extract the type portion (strip parameters)
    std::string type = detail::toLower(detail::extractType(mimeType));
    if (type.empty()) {
        return std::nullopt;
    }

    // Look up in MIME_DB for explicit charset
    auto* entry = detail::findMimeEntry(type);
    if (entry && !entry->charset.empty()) {
        return std::string(entry->charset);
    }

    // Default: all text/* types get UTF-8
    if (type.substr(0, 5) == "text/") {
        return std::string("UTF-8");
    }

    return std::nullopt;
}

} // namespace mime
} // namespace polycpp
