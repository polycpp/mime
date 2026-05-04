Accept a vendor media type in a JSON API
========================================

**You'll build:** a content-negotiation helper that accepts requests of
the form ``Content-Type: application/vnd.acme.v2+json`` — the pattern
every REST-style API eventually reaches for when it needs to version
without breaking URLs.

**You'll use:** :cpp:func:`polycpp::mime::parse`,
:cpp:func:`polycpp::mime::format`, and
:cpp:func:`polycpp::mime::test`, together with the
:cpp:struct:`polycpp::mime::MediaType` aggregate.

**Prerequisites:** read :doc:`serve-static` first for the basic shape
of the API.

Step 1 — validate early, validate once
--------------------------------------

RFC 6838 has strict rules for what's a legal media type. Rather than
writing your own regex, hand the string to
:cpp:func:`polycpp::mime::test` at the request edge:

.. code-block:: cpp

   #include <polycpp/mime/mime.hpp>

   bool isValidContentType(std::string_view header) {
       using namespace polycpp::mime;
       // Strip parameters (e.g., "; charset=utf-8") before testing.
       auto semi = header.find(';');
       auto core = (semi == std::string_view::npos)
           ? std::string(header)
           : std::string(header.substr(0, semi));
       return test(core);
   }

Reject any request that fails this check with a ``415 Unsupported
Media Type`` — don't try to "fix" it. Garbage in the request line
almost always indicates a misconfigured client, not a user error.

Step 2 — extract the version from the subtype
---------------------------------------------

Once a type passes ``test``, :cpp:func:`polycpp::mime::parse` splits
it into :cpp:member:`polycpp::mime::MediaType::type`,
:cpp:member:`polycpp::mime::MediaType::subtype`, and
:cpp:member:`polycpp::mime::MediaType::suffix`:

.. code-block:: cpp

   struct VendorVersion {
       std::string vendor;   // e.g. "acme"
       int         major;    // e.g. 2
       std::string format;   // e.g. "json"
   };

   std::optional<VendorVersion> decodeAcme(const MediaType& mt) {
       // Expect "application/vnd.acme.v<N>+<fmt>".
       if (mt.type != "application") return std::nullopt;
       if (mt.suffix.empty())        return std::nullopt;
       auto& s = mt.subtype;
       if (!s.starts_with("vnd.acme.v")) return std::nullopt;
       int n = 0;
       for (char c : s.substr(10)) {
           if (c < '0' || c > '9') return std::nullopt;
           n = n * 10 + (c - '0');
       }
       return VendorVersion{"acme", n, mt.suffix};
   }

``parse`` lowercases its input and trims whitespace, so this comparison
is case-insensitive by default — exactly what the RFC calls for.

Step 3 — respond with a matching content type
---------------------------------------------

When you write the response, build the type back up with
:cpp:func:`polycpp::mime::format`. Round-tripping through the struct
guarantees your output matches what you accepted:

.. code-block:: cpp

   std::string responseContentType(const VendorVersion& v) {
       using namespace polycpp::mime;
       MediaType mt{
           "application",
           "vnd." + v.vendor + ".v" + std::to_string(v.major),
           v.format,
       };
       return format(mt);  // "application/vnd.acme.v2+json"
   }

If any component is invalid, ``format`` throws :cpp:class:`polycpp::TypeError`
— treat that as a programmer bug, not a runtime condition.

Step 4 — the full negotiator
----------------------------

Chain the steps. The result is short, readable, and has a single place
where we fail the request:

.. code-block:: cpp

   #include <polycpp/mime/mime.hpp>
   #include <polycpp/core/error.hpp>

   struct NegotiationResult {
       int                       status;        // 0 on success
       std::optional<VendorVersion> version;
       std::string               responseType;
   };

   NegotiationResult negotiate(std::string_view header) {
       using namespace polycpp::mime;

       if (!isValidContentType(header)) return {415, {}, ""};

       try {
           auto semi = header.find(';');
           auto mt = parse(std::string(
               (semi == std::string_view::npos)
                   ? header : header.substr(0, semi)));

           auto v = decodeAcme(mt);
           if (!v) return {415, {}, ""};
           return {0, v, responseContentType(*v)};
       } catch (const polycpp::TypeError&) {
           return {415, {}, ""};
       }
   }

What you learned
----------------

- ``test`` is cheaper than ``parse``; use it for validation where you
  don't need the parts.
- ``parse`` lowercases and trims, so you don't need to pre-normalise.
- ``format`` is the right way to build a response header — it
  validates every component so a typo fails fast.
- The structured-syntax suffix (``+json``, ``+xml``) is a first-class
  field; don't try to parse it out of the subtype yourself.
