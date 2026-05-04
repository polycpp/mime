Compatibility with npm packages
===============================

This library preserves the public intent of npm ``mime-types`` 3.0.2,
``mime-db`` 1.54.0, and ``media-typer`` 1.1.0, but exposes that behavior
through typed C++ APIs under ``polycpp::mime`` instead of CommonJS
package exports.

Surface mapping
---------------

.. list-table::
   :header-rows: 1
   :widths: 20 30 50

   * - npm surface
     - C++ surface
     - Compatibility note
   * - ``mime.lookup(path)``
     - :cpp:func:`polycpp::mime::lookup`
     - Uses ``polycpp::path::extname`` for path and extension handling and
       returns ``std::optional<std::string>`` instead of JavaScript ``false``.
   * - ``mime.contentType(str)``
     - :cpp:func:`polycpp::mime::contentType`
     - Accepts extension-like values or MIME type strings. Charset insertion
       is compatible with npm behavior, with stricter detection of an actual
       ``charset`` parameter.
   * - ``mime.extension(type)``
     - :cpp:func:`polycpp::mime::extension`
     - Strips parameters and returns ``std::optional<std::string>`` instead
       of JavaScript ``false``.
   * - ``mime.charset(type)`` and ``mime.charsets.lookup(type)``
     - :cpp:func:`polycpp::mime::charset`
     - The separate ``charsets.lookup`` alias is omitted; the typed C++ API
       exposes the canonical function directly.
   * - ``mime.types`` and ``mime.extensions``
     - :cpp:func:`polycpp::mime::types` and
       :cpp:func:`polycpp::mime::extensions`
     - Public JavaScript maps become read-only generated data accessors.
   * - ``mime._extensionConflicts``
     - :cpp:func:`polycpp::mime::extensionConflicts`
     - Exposes read-only conflict diagnostics generated from upstream
       ``mimeScore`` resolution.
   * - ``mediaTyper.parse(string)``
     - :cpp:func:`polycpp::mime::parse`
     - Returns a typed :cpp:struct:`polycpp::mime::MediaType` and throws
       ``polycpp::TypeError`` for invalid input.
   * - ``mediaTyper.format(obj)``
     - :cpp:func:`polycpp::mime::format`
     - Accepts a typed :cpp:struct:`polycpp::mime::MediaType`, validates its
       components, and throws ``polycpp::TypeError`` for invalid fields.
   * - ``mediaTyper.test(string)``
     - :cpp:func:`polycpp::mime::test`
     - Returns ``bool`` for typed string input. In this C++ API,
       ``test("")`` returns ``false``.

Sentinel values
---------------

The npm ``mime-types`` package uses JavaScript ``false`` for lookup-style
misses. C++ callers get ``std::optional<std::string>`` from
:cpp:func:`polycpp::mime::lookup`, :cpp:func:`polycpp::mime::contentType`,
:cpp:func:`polycpp::mime::extension`, and
:cpp:func:`polycpp::mime::charset`; absence is ``std::nullopt``.

:cpp:func:`polycpp::mime::extensions` returns an empty vector when a type is
unknown or has no extensions. :cpp:func:`polycpp::mime::types` and
:cpp:func:`polycpp::mime::extensionConflicts` always return spans over static
generated data.

MediaType suffix
----------------

JavaScript ``media-typer`` returns an object where ``suffix`` is undefined
when the subtype has no structured syntax suffix. C++ uses
``MediaType::suffix`` as a ``std::string``; the empty string means "no
suffix". :cpp:func:`polycpp::mime::format` omits ``+suffix`` when the field
is empty.

Read-only data
--------------

The npm packages expose mutable JavaScript objects for ``mime.types`` and
``mime.extensions``. This port does not expose mutable global maps. Use
:cpp:func:`polycpp::mime::types` to inspect extension-to-type entries and
:cpp:func:`polycpp::mime::extensions` to get a copy of the extension list for
one MIME type. Conflict diagnostics are similarly read-only through
:cpp:func:`polycpp::mime::extensionConflicts`.

Media type validation
---------------------

:cpp:func:`polycpp::mime::parse` and :cpp:func:`polycpp::mime::test` operate
on bare media types, not full parameterized ``Content-Type`` headers. Strip
parameters such as ``; charset=UTF-8`` before calling them.

Leading and trailing C whitespace is trimmed before media type validation.
Upstream ``media-typer`` 1.1.0 accepts literal spaces around the media type
through its regular expressions.

:cpp:func:`polycpp::mime::parse` throws ``polycpp::TypeError`` for empty or
invalid media type strings. :cpp:func:`polycpp::mime::format` validates the
``type``, ``subtype``, and non-empty ``suffix`` fields and throws
``polycpp::TypeError`` for invalid components. :cpp:func:`polycpp::mime::test`
returns ``false`` for invalid input, including ``""``; upstream
``media-typer`` throws for missing or falsy arguments in that case.

Package and build model
-----------------------

The upstream packages are separate CommonJS modules. This port combines the
``mime-types``, ``mime-db``, and ``media-typer`` surfaces in one
``polycpp::mime`` namespace, one public header
``<polycpp/mime/mime.hpp>``, and the CMake target ``polycpp::mime``.

``mime-db`` is generated into compile-time data from version 1.54.0 rather
than loaded as a runtime JSON object. There is no ``require`` boundary, no
dynamic non-string argument behavior, and no mutable export object to patch
at runtime.
