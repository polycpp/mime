media-typer
===========

RFC 6838 media type parsing, formatting, and validation — the
``parse`` / ``format`` / ``test`` triad from npm ``media-typer`` 1.1.0.

These helpers work with bare media types such as
``application/vnd.api+json``. They are not full ``Content-Type`` header
parsers, so callers should strip parameters such as ``; charset=UTF-8``
before calling :cpp:func:`polycpp::mime::parse` or
:cpp:func:`polycpp::mime::test`.

:cpp:func:`polycpp::mime::parse` lowercases valid input and throws
``polycpp::TypeError`` for empty or invalid media type strings.
Leading and trailing C whitespace is trimmed before validation.
:cpp:func:`polycpp::mime::format` validates the ``type``, ``subtype``, and
non-empty ``suffix`` fields in :cpp:struct:`polycpp::mime::MediaType`; it
throws ``polycpp::TypeError`` when any field is invalid. An empty
``MediaType::suffix`` means no structured syntax suffix.

MediaType
---------

.. doxygenstruct:: polycpp::mime::MediaType
   :members:
   :undoc-members:

Functions
---------

.. doxygenfunction:: polycpp::mime::parse
.. doxygenfunction:: polycpp::mime::format
.. doxygenfunction:: polycpp::mime::test
