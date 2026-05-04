Serve a static file with the right Content-Type
===============================================

**You'll build:** a tiny function that, given a request path rooted in a
``public/`` directory, returns the bytes of the requested file along
with a properly charset-annotated ``Content-Type`` header — exactly
what a static-file middleware would do.

**You'll use:** :cpp:func:`polycpp::mime::contentType`,
:cpp:func:`polycpp::mime::lookup`, and
:cpp:func:`polycpp::mime::charset` for the fallback case.

**Prerequisites:** installed and linking ``polycpp::mime``. See
:doc:`../getting-started/installation` if not.

Step 1 — resolve the on-disk path
---------------------------------

Before you touch MIME types you need a safe path. The library has no
opinion about filesystem safety, but a typical handler rejects any path
that escapes the document root:

.. code-block:: cpp

   #include <filesystem>

   std::optional<std::filesystem::path>
   resolveWithinRoot(const std::filesystem::path& root,
                     std::string_view urlPath) {
       auto full = std::filesystem::weakly_canonical(root / urlPath.substr(1));
       auto rel  = std::filesystem::relative(full, root);
       if (rel.empty() || rel.native().starts_with("..")) return std::nullopt;
       return full;
   }

This is orthogonal to MIME handling, but most people trip over it first.
Everything below assumes the request path is trusted.

Step 2 — pick a Content-Type
----------------------------

``contentType`` takes either a MIME string or a bare extension. The
filesystem path works because it treats anything without a ``/`` as an
extension candidate:

.. code-block:: cpp

   #include <polycpp/mime/mime.hpp>

   std::string pickContentType(const std::filesystem::path& file) {
       using namespace polycpp::mime;
       if (auto ct = contentType(file.extension().string())) {
           return *ct;
       }
       return "application/octet-stream";  // universal fallback
   }

Two things to notice. First, we pass the extension *with* the leading
dot (``".html"``); :cpp:func:`polycpp::mime::lookup` handles that form.
Second, ``application/octet-stream`` is the defensible fallback when
the extension is unknown — RFC 2046 §4.5.1 says so, and every browser
treats it as "download, don't render".

Step 3 — sniff binaries separately from text
--------------------------------------------

For images and other binary formats, you want ``Content-Type`` but
*not* ``Content-Length`` inflation from chunked encoding or an implicit
charset. The charset helper lets you branch:

.. code-block:: cpp

   bool isTextual(std::string_view mimeType) {
       using namespace polycpp::mime;
       if (auto cs = charset(std::string(mimeType))) {
           return true;  // any type with a known charset is textual
       }
       return mimeType.starts_with("text/");
   }

:cpp:func:`polycpp::mime::charset` already returns a UTF-8 fallback for
``text/*``, but checking the prefix too catches hand-crafted types that
aren't in mime-db yet.

Step 4 — wire it together
-------------------------

.. code-block:: cpp

   #include <fstream>
   #include <sstream>

   struct StaticResponse {
       int         status;
       std::string contentType;
       std::string body;
   };

   StaticResponse serveStatic(const std::filesystem::path& root,
                              std::string_view urlPath) {
       auto file = resolveWithinRoot(root, urlPath);
       if (!file || !std::filesystem::is_regular_file(*file)) {
           return {404, "text/plain; charset=utf-8", "not found"};
       }

       std::ifstream in(*file, std::ios::binary);
       std::stringstream buf;
       buf << in.rdbuf();

       return {200, pickContentType(*file), buf.str()};
   }

Drop that into whatever server loop you already have — polycpp's
``http`` module, cpp-httplib, Crow, Boost.Beast, a raw socket — and
the Content-Type column in the browser devtools Network pane should
match what you'd see from Express or Nginx.

What you learned
----------------

- ``contentType`` is the right entry point for response headers; it
  combines ``lookup`` and ``charset`` in one call.
- ``application/octet-stream`` is the canonical fallback for unknown
  extensions — don't invent your own.
- The ``charset`` helper is the easiest way to tell "is this textual?"
  without hard-coding a list.

Next: :doc:`vendor-media-type` — using ``parse`` / ``format`` /
``test`` to accept a bespoke vendor media type in an API.
