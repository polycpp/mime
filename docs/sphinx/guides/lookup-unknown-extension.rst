Handle an unknown extension
===========================

**When to reach for this:** a user uploaded a file with an extension
mime-db doesn't know (``.flv5``, ``.heif``, a made-up in-house one) and
you still need to send a response header.

.. code-block:: cpp

   #include <polycpp/mime/mime.hpp>

   std::string contentTypeOrFallback(const std::string& pathOrExt) {
       using namespace polycpp::mime;
       if (auto ct = contentType(pathOrExt)) return *ct;
       return "application/octet-stream";  // RFC 2046 §4.5.1
   }

``application/octet-stream`` is the right default: it tells every
client "this is bytes, don't try to render it inline". Don't reach for
``text/plain`` — browsers will happily display raw binary content with
that type and corrupt the output.

If you routinely see the same unknown extension in production logs,
that's a signal to contribute it upstream to `jshttp/mime-db`_ rather
than carry a local patch.

.. _jshttp/mime-db: https://github.com/jshttp/mime-db
