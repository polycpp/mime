#pragma once

/**
 * @file media_typer_impl.hpp
 * @brief Implementation of media-typer parse/format/test functions.
 *
 * Hand-written character-by-character parsing per RFC 6838 restricted-name
 * rules. No regex dependency.
 */

#include <polycpp/mime/mime.hpp>
#include <polycpp/core/error.hpp>

#include <cctype>
#include <string>

namespace polycpp {
namespace mime {
namespace detail {

/**
 * @brief Check if a character is valid as the first character of a
 *        restricted-name (ALPHA / DIGIT).
 */
inline bool isRestrictedNameFirst(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) != 0;
}

/**
 * @brief Check if a character is valid in a type-name (restricted-name-chars
 *        without '.' and '+').
 *
 * restricted-name-chars = ALPHA / DIGIT / "!" / "#" / "$" / "&" / "-" / "^" / "_"
 * (plus "." for subtype, and "+" for full type/subtype string, handled separately)
 */
inline bool isTypeNameChar(char c) {
    if (std::isalnum(static_cast<unsigned char>(c))) return true;
    switch (c) {
        case '!': case '#': case '$': case '&':
        case '-': case '^': case '_':
            return true;
        default:
            return false;
    }
}

/**
 * @brief Check if a character is valid in a subtype-name (includes '.').
 */
inline bool isSubtypeNameChar(char c) {
    return isTypeNameChar(c) || c == '.';
}

/**
 * @brief Validate a type-name component.
 *
 * TYPE_NAME_REGEXP = /^[A-Za-z0-9][A-Za-z0-9!#$&^_-]{0,126}$/
 */
inline bool isValidTypeName(const std::string& name) {
    if (name.empty() || name.size() > 127) return false;
    if (!isRestrictedNameFirst(name[0])) return false;
    for (size_t i = 1; i < name.size(); ++i) {
        if (!isTypeNameChar(name[i])) return false;
    }
    return true;
}

/**
 * @brief Validate a subtype-name component (allows '.').
 *
 * SUBTYPE_NAME_REGEXP = /^[A-Za-z0-9][A-Za-z0-9!#$&^_.-]{0,126}$/
 */
inline bool isValidSubtypeName(const std::string& name) {
    if (name.empty() || name.size() > 127) return false;
    if (!isRestrictedNameFirst(name[0])) return false;
    for (size_t i = 1; i < name.size(); ++i) {
        if (!isSubtypeNameChar(name[i])) return false;
    }
    return true;
}

/**
 * @brief Check if a character is valid in a full subtype+suffix string
 *        (includes '.', '+').
 */
inline bool isFullSubtypeChar(char c) {
    return isSubtypeNameChar(c) || c == '+';
}

/**
 * @brief Match the full TYPE_REGEXP pattern manually.
 *
 * TYPE_REGEXP = /^ *([A-Za-z0-9][A-Za-z0-9!#$&^_-]{0,126})\/([A-Za-z0-9][A-Za-z0-9!#$&^_.+-]{0,126}) *$/
 *
 * Returns true if matched, and sets type_out and full_subtype_out.
 */
inline bool matchTypeRegexp(const std::string& str,
                             std::string& type_out,
                             std::string& full_subtype_out) {
    size_t pos = 0;
    size_t len = str.size();

    // Skip leading generic whitespace (tabs/CR/LF/space).
    while (pos < len && std::isspace(static_cast<unsigned char>(str[pos]))) ++pos;

    // Type: first char must be alnum
    if (pos >= len || !isRestrictedNameFirst(str[pos])) return false;

    size_t type_start = pos;
    ++pos;
    size_t type_chars = 1; // count chars in type name
    while (pos < len && isTypeNameChar(str[pos]) && type_chars < 127) {
        ++pos;
        ++type_chars;
    }

    // Must have '/'
    if (pos >= len || str[pos] != '/') return false;
    type_out = str.substr(type_start, pos - type_start);
    ++pos;

    // Subtype: first char must be alnum
    if (pos >= len || !isRestrictedNameFirst(str[pos])) return false;

    size_t sub_start = pos;
    ++pos;
    size_t sub_chars = 1;
    while (pos < len && isFullSubtypeChar(str[pos]) && sub_chars < 127) {
        ++pos;
        ++sub_chars;
    }

    // Skip trailing generic whitespace.
    while (pos < len && std::isspace(static_cast<unsigned char>(str[pos]))) ++pos;

    // Must be at end
    if (pos != len) return false;

    full_subtype_out = str.substr(sub_start, pos - sub_start);
    // Trim trailing whitespace from subtype
    while (!full_subtype_out.empty() &&
           std::isspace(static_cast<unsigned char>(full_subtype_out.back()))) {
        full_subtype_out.pop_back();
    }

    return true;
}

} // namespace detail

// ============================================================================
// media-typer function implementations
// ============================================================================

inline MediaType parse(const std::string& mediaType) {
    if (mediaType.empty()) {
        throw polycpp::TypeError("argument string is required");
    }

    // Lowercase the input
    std::string lower = detail::toLower(mediaType);

    std::string type_str, full_subtype;
    if (!detail::matchTypeRegexp(lower, type_str, full_subtype)) {
        throw polycpp::TypeError("invalid media type");
    }

    std::string subtype = full_subtype;
    std::string suffix;

    // Extract suffix after last '+'
    auto plus_pos = full_subtype.rfind('+');
    if (plus_pos != std::string::npos) {
        suffix = full_subtype.substr(plus_pos + 1);
        subtype = full_subtype.substr(0, plus_pos);
    }

    MediaType result;
    result.type = std::move(type_str);
    result.subtype = std::move(subtype);
    result.suffix = std::move(suffix);
    return result;
}

inline std::string format(const MediaType& mt) {
    if (!detail::isValidTypeName(mt.type)) {
        throw polycpp::TypeError("invalid type");
    }

    if (!detail::isValidSubtypeName(mt.subtype)) {
        throw polycpp::TypeError("invalid subtype");
    }

    std::string result = mt.type + "/" + mt.subtype;

    if (!mt.suffix.empty()) {
        if (!detail::isValidTypeName(mt.suffix)) {
            throw polycpp::TypeError("invalid suffix");
        }
        result += "+" + mt.suffix;
    }

    return result;
}

inline bool test(const std::string& mediaType) {
    if (mediaType.empty()) {
        return false;
    }

    std::string lower = detail::toLower(mediaType);
    std::string type_str, full_subtype;
    return detail::matchTypeRegexp(lower, type_str, full_subtype);
}

} // namespace mime
} // namespace polycpp
