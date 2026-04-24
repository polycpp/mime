mime
====

**MIME type lookup and RFC 6838 media type handling**

Look up MIME types by file extension, generate ``Content-Type`` headers
with the right charset, walk in the other direction to a canonical
extension, and parse or validate RFC 6838 media types like
``application/vnd.api+json``. A C++20 port of the npm ``mime-types``,
``mime-db``, and ``media-typer`` packages backed by 2,601 types compiled
in as ``constexpr`` data — zero startup cost, zero I/O at first lookup.

.. code-block:: cpp

   #include <polycpp/mime/detail/aggregator.hpp>
   using namespace polycpp::mime;

   auto type = lookup("index.html");        // "text/html"
   auto ct   = contentType("json");         // "application/json; charset=utf-8"
   auto ext  = extension("image/svg+xml");  // "svg"
   auto cs   = charset("text/css");         // "UTF-8"

   auto parsed = parse("application/vnd.api+json");
   // parsed.type == "application", subtype == "vnd.api", suffix == "json"

.. grid:: 2

   .. grid-item-card:: Drop-in familiarity
      :margin: 1

      Same four verbs as npm ``mime-types`` — ``lookup``, ``contentType``,
      ``extension``, ``charset`` — plus the ``parse`` / ``format`` /
      ``test`` triad from ``media-typer``.

   .. grid-item-card:: C++20 native
      :margin: 1

      Header-only where possible, zero-overhead abstractions, ``constexpr``
      and ``std::string_view`` throughout.

   .. grid-item-card:: Tested
      :margin: 1

      Covers the upstream ``mime-types`` and ``media-typer`` test suites
      plus RFC 6838 edge cases — UTF-8 charset fallback, structured
      suffixes, case-insensitive lookup.

   .. grid-item-card:: Plays well with polycpp
      :margin: 1

      Uses the same JSON value, error, and typed-event types as the rest of
      the polycpp ecosystem — no impedance mismatch.

Getting started
---------------

.. code-block:: bash

   # With FetchContent (recommended)
   FetchContent_Declare(
       polycpp_mime
       GIT_REPOSITORY https://github.com/polycpp/mime.git
       GIT_TAG        master
   )
   FetchContent_MakeAvailable(polycpp_mime)
   target_link_libraries(my_app PRIVATE polycpp::mime)

:doc:`Installation <getting-started/installation>` · :doc:`Quickstart <getting-started/quickstart>` · :doc:`Tutorials <tutorials/index>` · :doc:`API reference <api/index>`

.. toctree::
   :hidden:
   :caption: Getting started

   getting-started/installation
   getting-started/quickstart

.. toctree::
   :hidden:
   :caption: Tutorials

   tutorials/index

.. toctree::
   :hidden:
   :caption: How-to guides

   guides/index

.. toctree::
   :hidden:
   :caption: API reference

   api/index

.. toctree::
   :hidden:
   :caption: Examples

   examples/index
