Strip parameters off a Content-Type
===================================

**When to reach for this:** you've read a ``Content-Type`` header off
a request and want only the ``type/subtype`` portion — routing,
logging, or cache-key composition.

:cpp:func:`polycpp::mime::extension` already strips parameters before
lookup, but when you want the type itself, parse it and re-format:

.. code-block:: cpp

   #include <polycpp/mime/detail/aggregator.hpp>

   std::string baseType(const std::string& header) {
       using namespace polycpp::mime;
       // parse() throws on invalid input; guard with test() first if
       // the header came from an untrusted source.
       auto mt = parse(header);  // trims whitespace, lowercases
       return format(mt);
   }

   // baseType("text/html; charset=UTF-8") == "text/html"
   // baseType("APPLICATION/JSON")          == "application/json"
   // baseType("image/svg+xml")             == "image/svg+xml"

Avoid hand-rolling a semicolon split — it doesn't handle quoted
parameter values (``filename="a;b.txt"``) and it won't lowercase the
type, which RFC 2045 §5.1 declares case-insensitive.
