Strip parameters off a Content-Type
===================================

**When to reach for this:** you've read a ``Content-Type`` header off
a request and want only the ``type/subtype`` portion — routing,
logging, or cache-key composition.

:cpp:func:`polycpp::mime::extension` already strips parameters before
lookup. The ``parse`` / ``test`` helpers are stricter: they accept only
the bare media type, not a full header value with parameters. Strip the
header at the first ``;``, then validate and parse:

.. code-block:: cpp

   #include <optional>
   #include <polycpp/mime/mime.hpp>
   #include <string>
   #include <string_view>

   std::string_view stripContentTypeParameters(std::string_view header) {
       return header.substr(0, header.find(';'));
   }

   std::optional<std::string> baseType(const std::string& header) {
       using namespace polycpp::mime;
       auto bare = std::string(stripContentTypeParameters(header));
       if (!test(bare)) return std::nullopt;

       auto mt = parse(bare);  // trims whitespace, lowercases
       return format(mt);
   }

   // baseType("text/html; charset=UTF-8")        == "text/html"
   // baseType("application/vnd.api+json; q=0.9") == "application/vnd.api+json"
   // baseType("APPLICATION/JSON")                 == "application/json"
   // baseType("image/svg+xml")                    == "image/svg+xml"

Use the split only to isolate the media type. Keep ``test`` / ``parse``
responsible for validation and normalization; :cpp:func:`polycpp::mime::parse`
throws ``polycpp::TypeError`` if invalid input reaches it.
