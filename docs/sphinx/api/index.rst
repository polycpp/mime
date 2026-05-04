API reference
=============

The complete public surface of mime, generated from the Doxygen
comments in the headers. If a symbol is missing here, either it's internal
(under a ``detail`` namespace) or its header comment needs more love —
please open an issue.

This port is based on npm ``mime-types`` 3.0.2, ``mime-db`` 1.54.0,
and ``media-typer`` 1.1.0.

Module index
------------

.. toctree::
   :maxdepth: 1

   mime-types
   media-typer
   compatibility

Behavior matrix
---------------

.. list-table::
   :header-rows: 1
   :widths: 15 24 24 20 18 26

   * - Function
     - Accepted input
     - Parameter handling
     - Case normalization
     - Return type
     - Failure behavior
   * - :cpp:func:`polycpp::mime::lookup`
     - File path, ``.ext``, or bare extension.
     - No ``Content-Type`` parameter parsing; the input is interpreted as a
       path or extension string.
     - Lowercases the extracted extension before lookup.
     - ``std::optional<std::string>``.
     - Returns ``std::nullopt`` for empty input, no extension, or unknown
       extension; does not throw.
   * - :cpp:func:`polycpp::mime::contentType`
     - MIME type or extension. Path-like extension inputs without ``/`` also
       work through :cpp:func:`polycpp::mime::lookup`.
     - Existing parameters are preserved. If no real ``charset`` parameter is
       present and the type has a known charset, appends ``; charset=...``.
     - Extension lookup is case-insensitive. MIME type text is otherwise
       preserved; appended charset values are lowercase.
     - ``std::optional<std::string>``.
     - Returns ``std::nullopt`` for empty or unknown extension input; does
       not throw.
   * - :cpp:func:`polycpp::mime::extension`
     - MIME type string, optionally with parameters.
     - Strips leading whitespace and everything after the bare type, including
       parameters after ``;``.
     - Lowercases the extracted media type before lookup.
     - ``std::optional<std::string>``.
     - Returns ``std::nullopt`` for empty, unknown, or extensionless types;
       does not throw.
   * - :cpp:func:`polycpp::mime::charset`
     - MIME type string, optionally with parameters.
     - Strips leading whitespace and everything after the bare type, including
       parameters after ``;``.
     - Lowercases the extracted media type before lookup.
     - ``std::optional<std::string>``.
     - Returns ``std::nullopt`` when mime-db has no charset and the type is
       not ``text/*``; does not throw.
   * - :cpp:func:`polycpp::mime::types`
     - No input.
     - Not applicable.
     - Entries are generated in canonical lowercase extension order.
     - ``std::span<const TypeEntry>``.
     - Always returns the static generated span; no failure path.
   * - :cpp:func:`polycpp::mime::extensions`
     - MIME type string, optionally with parameters.
     - Strips leading whitespace and everything after the bare type, including
       parameters after ``;``.
     - Lowercases the extracted media type before lookup.
     - ``std::vector<std::string>``.
     - Returns an empty vector for empty, unknown, or extensionless types;
       does not throw.
   * - :cpp:func:`polycpp::mime::extensionConflicts`
     - No input.
     - Not applicable.
     - Diagnostic entries use generated data casing.
     - ``std::span<const ExtensionConflict>``.
     - Always returns the static generated span; no failure path.
   * - :cpp:func:`polycpp::mime::parse`
     - Bare RFC 6838 media type, such as ``application/vnd.api+json``, with
       optional leading and trailing C whitespace.
     - Does not accept full parameterized ``Content-Type`` headers; strip
       parameters before calling.
     - Lowercases the input and returned ``MediaType`` components.
     - :cpp:struct:`polycpp::mime::MediaType`; absent suffix is ``""``.
     - Throws ``polycpp::TypeError`` for empty or invalid input.
   * - :cpp:func:`polycpp::mime::format`
     - :cpp:struct:`polycpp::mime::MediaType` with ``type``, ``subtype``, and
       optional ``suffix`` components.
     - Formats only the bare media type. An empty suffix omits ``+suffix``.
     - Emits the component casing supplied by the caller.
     - ``std::string``.
     - Validates each component and throws ``polycpp::TypeError`` for invalid
       fields.
   * - :cpp:func:`polycpp::mime::test`
     - Bare RFC 6838 media type string, with optional leading and trailing C
       whitespace.
     - Does not accept full parameterized ``Content-Type`` headers; strip
       parameters before calling.
     - Lowercases before validation.
     - ``bool``.
     - Returns ``false`` for empty or invalid input; does not throw.

Namespace overview
------------------

.. doxygennamespace:: polycpp::mime
   :desc-only:
