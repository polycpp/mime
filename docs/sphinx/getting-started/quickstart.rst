Quickstart
==========

This page walks through a minimal mime program end-to-end. Copy the
snippet, run it, then jump to :doc:`../tutorials/index` for task-oriented
walkthroughs or :doc:`../api/index` for the full reference.

We'll take a handful of file paths and MIME strings, feed them through
every public function, and print the results. The output line-for-line
mirrors what an HTTP server would attach to a response.

Full example
------------

.. code-block:: cpp

   #include <iostream>
   #include <polycpp/mime/detail/aggregator.hpp>

   using namespace polycpp::mime;

   int main() {
       // 1. Look up a MIME type from a file path.
       if (auto type = lookup("index.html")) {
           std::cout << "index.html -> " << *type << '\n';
       }

       // 2. Build a full Content-Type header. The charset is added
       //    automatically for types that have one in mime-db.
       if (auto ct = contentType("json")) {
           std::cout << "Content-Type: " << *ct << '\n';
       }
       if (auto ct = contentType("image/png")) {
           std::cout << "Content-Type: " << *ct << '\n';
       }

       // 3. Walk in the other direction: MIME -> canonical extension.
       if (auto ext = extension("image/svg+xml")) {
           std::cout << "image/svg+xml -> ." << *ext << '\n';
       }

       // 4. Ask for the charset a type uses (falls back to UTF-8 for text/*).
       if (auto cs = charset("text/css")) {
           std::cout << "text/css charset = " << *cs << '\n';
       }

       // 5. Parse a structured-syntax-suffix media type per RFC 6838.
       auto mt = parse("application/vnd.api+json");
       std::cout << "type=" << mt.type
                 << " subtype=" << mt.subtype
                 << " suffix=" << mt.suffix << '\n';

       // 6. And the reverse — format it back out.
       std::cout << format(MediaType{"image", "svg", "xml"}) << '\n';
   }

Compile it with the same CMake wiring from :doc:`installation`:

.. code-block:: bash

   cmake -B build -G Ninja
   cmake --build build
   ./build/my_app

Expected output:

.. code-block:: text

   index.html -> text/html
   Content-Type: application/json; charset=utf-8
   Content-Type: image/png
   image/svg+xml -> svg
   text/css charset = UTF-8
   type=application subtype=vnd.api suffix=json
   image/svg+xml

What just happened
------------------

1. :cpp:func:`polycpp::mime::lookup` accepts full paths, bare extensions,
   or dotted extensions (``"index.html"``, ``"html"``, ``".html"``) and
   returns an ``std::optional<std::string>``. Unknown extensions produce
   ``std::nullopt`` — no exceptions, no silent fallback.

2. :cpp:func:`polycpp::mime::contentType` is the one you want when you're
   writing to a response: it runs ``lookup`` if the argument has no
   slash, then appends ``; charset=<cs>`` when mime-db defines one and
   the input didn't already specify a charset.

3. :cpp:func:`polycpp::mime::extension` strips parameters before lookup,
   so ``"text/html; charset=utf-8"`` still resolves to ``"html"``.

4. :cpp:func:`polycpp::mime::charset` returns a charset if mime-db knows
   one, otherwise ``"UTF-8"`` for any ``text/*`` type, otherwise
   ``std::nullopt``. That fallback matches the upstream npm package
   exactly.

5. :cpp:func:`polycpp::mime::parse` and :cpp:func:`polycpp::mime::format`
   are the RFC 6838 pair — they understand structured-syntax suffixes
   (``+json``, ``+xml``, ``+cbor``) that ``mime-types`` alone does not.

Next steps
----------

- :doc:`../tutorials/index` — step-by-step walkthroughs of common tasks.
- :doc:`../guides/index` — short how-tos for specific problems.
- :doc:`../api/index` — every public type, function, and option.
- :doc:`../examples/index` — runnable programs you can drop into a sandbox.
