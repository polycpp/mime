Detect the charset for a MIME type
==================================

**When to reach for this:** you're decoding bytes that came with a
``Content-Type`` header and need to pick a text codec.

.. code-block:: cpp

   #include <polycpp/mime/mime.hpp>

   std::string pickCharset(const std::string& mimeType) {
       using namespace polycpp::mime;
       if (auto cs = charset(mimeType)) return *cs;
       return "binary";  // not textual — don't decode
   }

   // pickCharset("text/html")               == "UTF-8"
   // pickCharset("application/json")        == "UTF-8"
   // pickCharset("application/octet-stream") == "binary"
   // pickCharset("image/png")               == "binary"

The logic follows npm ``mime-types``: look up the explicit charset in
mime-db first, then fall back to ``UTF-8`` for any ``text/*`` type.
``application/json`` is textual and gets UTF-8 from mime-db directly
(RFC 8259 mandates UTF-8).

Do not override a charset the caller already put in the header — it's
authoritative, even when it disagrees with what mime-db would pick.
