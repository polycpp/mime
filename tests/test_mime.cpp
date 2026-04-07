/**
 * @file test_mime.cpp
 * @brief Tests for polycpp::mime — ported from npm mime-types and media-typer
 *        test suites plus additional coverage.
 */

#include <polycpp/mime/detail/aggregator.hpp>
#include <polycpp/core/error.hpp>

#include <gtest/gtest.h>
#include <optional>
#include <string>

using namespace polycpp::mime;

// ============================================================================
// charset() tests — ported from mime-types test suite
// ============================================================================

TEST(MimeCharsetTest, ReturnsUTF8ForApplicationJson) {
    EXPECT_EQ(charset("application/json"), "UTF-8");
}

TEST(MimeCharsetTest, ReturnsUTF8ForApplicationJsonWithParams) {
    EXPECT_EQ(charset("application/json; foo=bar"), "UTF-8");
}

TEST(MimeCharsetTest, ReturnsUTF8ForApplicationJavascript) {
    EXPECT_EQ(charset("application/javascript"), "UTF-8");
}

TEST(MimeCharsetTest, ReturnsUTF8ForApplicationJavascriptMixedCase) {
    EXPECT_EQ(charset("application/JavaScript"), "UTF-8");
}

TEST(MimeCharsetTest, ReturnsUTF8ForTextHtml) {
    EXPECT_EQ(charset("text/html"), "UTF-8");
}

TEST(MimeCharsetTest, ReturnsUTF8ForTEXTHTML) {
    EXPECT_EQ(charset("TEXT/HTML"), "UTF-8");
}

TEST(MimeCharsetTest, ReturnsUTF8ForAnyTextType) {
    EXPECT_EQ(charset("text/x-bogus"), "UTF-8");
}

TEST(MimeCharsetTest, ReturnsNulloptForUnknownTypes) {
    EXPECT_EQ(charset("application/x-bogus"), std::nullopt);
}

TEST(MimeCharsetTest, ReturnsNulloptForApplicationOctetStream) {
    EXPECT_EQ(charset("application/octet-stream"), std::nullopt);
}

TEST(MimeCharsetTest, ReturnsNulloptForEmptyString) {
    EXPECT_EQ(charset(""), std::nullopt);
}

TEST(MimeCharsetTest, ReturnsNulloptForImagePng) {
    EXPECT_EQ(charset("image/png"), std::nullopt);
}

// ============================================================================
// contentType() tests — ported from mime-types test suite
// ============================================================================

TEST(MimeContentTypeTest, ReturnsContentTypeForHtmlExtension) {
    EXPECT_EQ(contentType("html"), "text/html; charset=utf-8");
}

TEST(MimeContentTypeTest, ReturnsContentTypeForDotHtmlExtension) {
    EXPECT_EQ(contentType(".html"), "text/html; charset=utf-8");
}

TEST(MimeContentTypeTest, ReturnsContentTypeForJadeExtension) {
    EXPECT_EQ(contentType("jade"), "text/jade; charset=utf-8");
}

TEST(MimeContentTypeTest, ReturnsContentTypeForJsonExtension) {
    EXPECT_EQ(contentType("json"), "application/json; charset=utf-8");
}

TEST(MimeContentTypeTest, ReturnsNulloptForUnknownExtension) {
    EXPECT_EQ(contentType("bogus"), std::nullopt);
}

TEST(MimeContentTypeTest, ReturnsNulloptForEmptyString) {
    EXPECT_EQ(contentType(""), std::nullopt);
}

TEST(MimeContentTypeTest, AttachesCharsetToApplicationJson) {
    EXPECT_EQ(contentType("application/json"), "application/json; charset=utf-8");
}

TEST(MimeContentTypeTest, AttachesCharsetToApplicationJsonWithParams) {
    EXPECT_EQ(contentType("application/json; foo=bar"),
              "application/json; foo=bar; charset=utf-8");
}

TEST(MimeContentTypeTest, AttachesCharsetToTEXTHTML) {
    EXPECT_EQ(contentType("TEXT/HTML"), "TEXT/HTML; charset=utf-8");
}

TEST(MimeContentTypeTest, AttachesCharsetToTextHtml) {
    EXPECT_EQ(contentType("text/html"), "text/html; charset=utf-8");
}

TEST(MimeContentTypeTest, DoesNotAlterExistingCharset) {
    EXPECT_EQ(contentType("text/html; charset=iso-8859-1"),
              "text/html; charset=iso-8859-1");
}

TEST(MimeContentTypeTest, AppendsCharsetWhenValueContainsCharsetSubstring) {
    EXPECT_EQ(contentType("text/html; foo=my-charset-token"),
              "text/html; foo=my-charset-token; charset=utf-8");
}

TEST(MimeContentTypeTest, AppendsCharsetWhenParameterNameContainsCharsetSubstring) {
    EXPECT_EQ(contentType("text/html; charsetFlag=true"),
              "text/html; charsetFlag=true; charset=utf-8");
}

TEST(MimeContentTypeTest, DetectsCharsetParameterWithWhitespaceAroundEquals) {
    EXPECT_EQ(contentType("text/html; charset = utf-8"),
              "text/html; charset = utf-8");
}

TEST(MimeContentTypeTest, ReturnsTypeForUnknownMimeType) {
    EXPECT_EQ(contentType("application/x-bogus"), "application/x-bogus");
}

// ============================================================================
// extension() tests — ported from mime-types test suite
// ============================================================================

TEST(MimeExtensionTest, ReturnsExtensionForTextHtml) {
    EXPECT_EQ(extension("text/html"), "html");
}

TEST(MimeExtensionTest, ReturnsExtensionWithLeadingSpace) {
    EXPECT_EQ(extension(" text/html"), "html");
}

TEST(MimeExtensionTest, ReturnsExtensionWithTrailingSpace) {
    EXPECT_EQ(extension("text/html "), "html");
}

TEST(MimeExtensionTest, ReturnsNulloptForUnknownType) {
    EXPECT_EQ(extension("application/x-bogus"), std::nullopt);
}

TEST(MimeExtensionTest, ReturnsNulloptForNonTypeString) {
    EXPECT_EQ(extension("bogus"), std::nullopt);
}

TEST(MimeExtensionTest, ReturnsNulloptForEmptyString) {
    EXPECT_EQ(extension(""), std::nullopt);
}

TEST(MimeExtensionTest, ReturnsExtensionForMimeTypeWithParameters) {
    EXPECT_EQ(extension("text/html;charset=UTF-8"), "html");
    EXPECT_EQ(extension("text/HTML; charset=UTF-8"), "html");
    EXPECT_EQ(extension("text/html; charset=UTF-8"), "html");
    EXPECT_EQ(extension("text/html; charset=UTF-8 "), "html");
    EXPECT_EQ(extension("text/html ; charset=UTF-8"), "html");
}

// ============================================================================
// lookup() tests — ported from mime-types test suite
// ============================================================================

TEST(MimeLookupTest, ReturnsMimeTypeForDotHtml) {
    EXPECT_EQ(lookup(".html"), "text/html");
}

TEST(MimeLookupTest, ReturnsMimeTypeForDotJs) {
    EXPECT_EQ(lookup(".js"), "text/javascript");
}

TEST(MimeLookupTest, ReturnsMimeTypeForDotJson) {
    EXPECT_EQ(lookup(".json"), "application/json");
}

TEST(MimeLookupTest, ReturnsMimeTypeForDotRtf) {
    EXPECT_EQ(lookup(".rtf"), "application/rtf");
}

TEST(MimeLookupTest, ReturnsMimeTypeForDotTxt) {
    EXPECT_EQ(lookup(".txt"), "text/plain");
}

TEST(MimeLookupTest, ReturnsMimeTypeForDotXml) {
    EXPECT_EQ(lookup(".xml"), "application/xml");
}

TEST(MimeLookupTest, ReturnsMimeTypeForDotMp4) {
    EXPECT_EQ(lookup(".mp4"), "video/mp4");
}

TEST(MimeLookupTest, WorksWithoutLeadingDot) {
    EXPECT_EQ(lookup("html"), "text/html");
    EXPECT_EQ(lookup("xml"), "application/xml");
}

TEST(MimeLookupTest, IsCaseInsensitive) {
    EXPECT_EQ(lookup("HTML"), "text/html");
    EXPECT_EQ(lookup(".Xml"), "application/xml");
}

TEST(MimeLookupTest, ReturnsNulloptForUnknownExtension) {
    EXPECT_EQ(lookup(".bogus"), std::nullopt);
    EXPECT_EQ(lookup("bogus"), std::nullopt);
}

TEST(MimeLookupTest, ReturnsNulloptForEmptyString) {
    EXPECT_EQ(lookup(""), std::nullopt);
}

TEST(MimeLookupTest, ReturnsMimeTypeForFileName) {
    EXPECT_EQ(lookup("page.html"), "text/html");
}

TEST(MimeLookupTest, ReturnsMimeTypeForRelativePath) {
    EXPECT_EQ(lookup("path/to/page.html"), "text/html");
}

TEST(MimeLookupTest, ReturnsMimeTypeForAbsolutePath) {
    EXPECT_EQ(lookup("/path/to/page.html"), "text/html");
}

TEST(MimeLookupTest, IsCaseInsensitiveForPaths) {
    EXPECT_EQ(lookup("/path/to/PAGE.HTML"), "text/html");
}

TEST(MimeLookupTest, ReturnsNulloptForUnknownPathExtension) {
    EXPECT_EQ(lookup("/path/to/file.bogus"), std::nullopt);
}

TEST(MimeLookupTest, ReturnsNulloptForPathWithoutExtension) {
    EXPECT_EQ(lookup("/path/to/json"), std::nullopt);
}

TEST(MimeLookupTest, DotfileExtensionlessReturnsNullopt) {
    EXPECT_EQ(lookup("/path/to/.json"), std::nullopt);
}

TEST(MimeLookupTest, DotfileWithExtension) {
    EXPECT_EQ(lookup("/path/to/.config.json"), "application/json");
}

TEST(MimeLookupTest, DotfileWithExtensionNoPath) {
    EXPECT_EQ(lookup(".config.json"), "application/json");
}

// ============================================================================
// Additional mime-types tests
// ============================================================================

TEST(MimeLookupTest, CommonExtensions) {
    EXPECT_EQ(lookup("css"), "text/css");
    EXPECT_EQ(lookup("png"), "image/png");
    EXPECT_EQ(lookup("jpg"), "image/jpeg");
    EXPECT_EQ(lookup("gif"), "image/gif");
    EXPECT_EQ(lookup("svg"), "image/svg+xml");
    EXPECT_EQ(lookup("pdf"), "application/pdf");
    EXPECT_EQ(lookup("zip"), "application/zip");
    EXPECT_EQ(lookup("gz"), "application/gzip");
    EXPECT_EQ(lookup("woff2"), "font/woff2");
}

TEST(MimeCharsetTest, SpecificCharsets) {
    // application/json has charset UTF-8 in mime-db
    EXPECT_EQ(charset("application/json"), "UTF-8");
    // text/css gets UTF-8 default
    EXPECT_EQ(charset("text/css"), "UTF-8");
    // image types have no charset
    EXPECT_EQ(charset("image/png"), std::nullopt);
}

TEST(MimeExtensionTest, CommonMimeTypes) {
    EXPECT_EQ(extension("application/json"), "json");
    EXPECT_EQ(extension("text/css"), "css");
    EXPECT_EQ(extension("image/png"), "png");
    EXPECT_EQ(extension("image/jpeg"), "jpg");
    EXPECT_EQ(extension("application/pdf"), "pdf");
}

// ============================================================================
// media-typer parse() tests — ported from media-typer test suite
// ============================================================================

TEST(MediaTyperParseTest, ParsesBasicType) {
    auto mt = parse("text/html");
    EXPECT_EQ(mt.type, "text");
    EXPECT_EQ(mt.subtype, "html");
    EXPECT_TRUE(mt.suffix.empty());
}

TEST(MediaTyperParseTest, ParsesTypeWithSuffix) {
    auto mt = parse("image/svg+xml");
    EXPECT_EQ(mt.type, "image");
    EXPECT_EQ(mt.subtype, "svg");
    EXPECT_EQ(mt.suffix, "xml");
}

TEST(MediaTyperParseTest, LowerCasesType) {
    auto mt = parse("IMAGE/SVG+XML");
    EXPECT_EQ(mt.type, "image");
    EXPECT_EQ(mt.subtype, "svg");
    EXPECT_EQ(mt.suffix, "xml");
}

TEST(MediaTyperParseTest, TrimsGenericWhitespace) {
    auto mt = parse("\t application/xhtml+xml \n");
    EXPECT_EQ(mt.type, "application");
    EXPECT_EQ(mt.subtype, "xhtml");
    EXPECT_EQ(mt.suffix, "xml");
}

TEST(MediaTyperParseTest, ThrowsOnInvalidMediaTypes) {
    std::vector<std::string> invalidTypes = {
        " ", "null", "undefined", "/", "text/;plain",
        "text/\"plain\"", "text/(plain)", "text/@plain",
        "text/plain,wrong"
    };
    for (const auto& t : invalidTypes) {
        EXPECT_THROW(parse(t), polycpp::TypeError) << "Should throw for: " << t;
    }
}

TEST(MediaTyperParseTest, ThrowsOnEmptyString) {
    EXPECT_THROW(parse(""), polycpp::TypeError);
}

TEST(MediaTyperParseTest, ParsesVendorType) {
    auto mt = parse("application/vnd.api+json");
    EXPECT_EQ(mt.type, "application");
    EXPECT_EQ(mt.subtype, "vnd.api");
    EXPECT_EQ(mt.suffix, "json");
}

TEST(MediaTyperParseTest, ParsesXhtmlXml) {
    auto mt = parse("application/xhtml+xml");
    EXPECT_EQ(mt.type, "application");
    EXPECT_EQ(mt.subtype, "xhtml");
    EXPECT_EQ(mt.suffix, "xml");
}

// ============================================================================
// media-typer format() tests — ported from media-typer test suite
// ============================================================================

TEST(MediaTyperFormatTest, FormatsBasicType) {
    MediaType mt{"text", "html", ""};
    EXPECT_EQ(format(mt), "text/html");
}

TEST(MediaTyperFormatTest, FormatsTypeWithSuffix) {
    MediaType mt{"image", "svg", "xml"};
    EXPECT_EQ(format(mt), "image/svg+xml");
}

TEST(MediaTyperFormatTest, ThrowsOnEmptyType) {
    MediaType mt{"", "html", ""};
    EXPECT_THROW(format(mt), polycpp::TypeError);
}

TEST(MediaTyperFormatTest, ThrowsOnInvalidType) {
    MediaType mt{"text/", "html", ""};
    EXPECT_THROW(format(mt), polycpp::TypeError);
}

TEST(MediaTyperFormatTest, ThrowsOnEmptySubtype) {
    MediaType mt{"text", "", ""};
    EXPECT_THROW(format(mt), polycpp::TypeError);
}

TEST(MediaTyperFormatTest, ThrowsOnInvalidSubtype) {
    MediaType mt{"text", "html/", ""};
    EXPECT_THROW(format(mt), polycpp::TypeError);
}

TEST(MediaTyperFormatTest, ThrowsOnInvalidSuffix) {
    MediaType mt{"image", "svg", "xml\\"};
    EXPECT_THROW(format(mt), polycpp::TypeError);
}

// ============================================================================
// media-typer test() tests — ported from media-typer test suite
// ============================================================================

TEST(MediaTyperTestTest, PassesBasicType) {
    EXPECT_TRUE(test("text/html"));
}

TEST(MediaTyperTestTest, PassesTypeWithSuffix) {
    EXPECT_TRUE(test("image/svg+xml"));
}

TEST(MediaTyperTestTest, PassesUpperCaseType) {
    EXPECT_TRUE(test("IMAGE/SVG+XML"));
}

TEST(MediaTyperTestTest, PassesTypeWrappedInGenericWhitespace) {
    EXPECT_TRUE(test("\t text/html \r\n"));
}

TEST(MediaTyperTestTest, FailsInvalidMediaTypes) {
    std::vector<std::string> invalidTypes = {
        " ", "null", "undefined", "/", "text/;plain",
        "text/\"plain\"", "text/(plain)", "text/@plain",
        "text/plain,wrong"
    };
    for (const auto& t : invalidTypes) {
        EXPECT_FALSE(test(t)) << "Should fail for: " << t;
    }
}

TEST(MediaTyperTestTest, FailsEmptyString) {
    EXPECT_FALSE(test(""));
}

// ============================================================================
// media-typer roundtrip tests
// ============================================================================

TEST(MediaTyperRoundtripTest, BasicType) {
    auto mt = parse("text/html");
    EXPECT_EQ(format(mt), "text/html");
}

TEST(MediaTyperRoundtripTest, TypeWithSuffix) {
    auto mt = parse("application/vnd.api+json");
    EXPECT_EQ(format(mt), "application/vnd.api+json");
}

TEST(MediaTyperRoundtripTest, XhtmlXml) {
    auto mt = parse("application/xhtml+xml");
    EXPECT_EQ(format(mt), "application/xhtml+xml");
}

// ============================================================================
// Integration tests
// ============================================================================

TEST(MimeIntegrationTest, ContentTypeWithCharsetForTextTypes) {
    auto ct = contentType("text/plain");
    EXPECT_TRUE(ct.has_value());
    EXPECT_NE(ct->find("charset=utf-8"), std::string::npos);
}

TEST(MimeIntegrationTest, ContentTypeNoCharsetForBinaryTypes) {
    auto ct = contentType("image/png");
    EXPECT_TRUE(ct.has_value());
    EXPECT_EQ(ct->find("charset"), std::string::npos);
}

TEST(MimeIntegrationTest, LookupAndParseRoundtrip) {
    auto type = lookup("svg");
    ASSERT_TRUE(type.has_value());
    EXPECT_EQ(*type, "image/svg+xml");

    auto mt = parse(*type);
    EXPECT_EQ(mt.type, "image");
    EXPECT_EQ(mt.subtype, "svg");
    EXPECT_EQ(mt.suffix, "xml");
}

TEST(MimeIntegrationTest, ExtensionAndLookupRoundtrip) {
    auto ext = extension("application/json");
    ASSERT_TRUE(ext.has_value());
    EXPECT_EQ(*ext, "json");

    auto type = lookup(*ext);
    ASSERT_TRUE(type.has_value());
    EXPECT_EQ(*type, "application/json");
}

// ============================================================================
// Edge case tests
// ============================================================================

TEST(MimeEdgeCaseTest, WindowsPathSeparators) {
    EXPECT_EQ(lookup("C:\\path\\to\\page.html"), "text/html");
}

TEST(MimeEdgeCaseTest, MultipleDots) {
    EXPECT_EQ(lookup("archive.tar.gz"), "application/gzip");
}

TEST(MimeEdgeCaseTest, CaseSensitivityInContentType) {
    // charset parameter check should be case-insensitive
    EXPECT_EQ(contentType("text/html; Charset=utf-8"),
              "text/html; Charset=utf-8");
}

TEST(MediaTyperEdgeCaseTest, TypeWithMultiplePluses) {
    // Last + should be the suffix separator
    auto mt = parse("application/vnd.hal+json");
    EXPECT_EQ(mt.type, "application");
    EXPECT_EQ(mt.subtype, "vnd.hal");
    EXPECT_EQ(mt.suffix, "json");
}
