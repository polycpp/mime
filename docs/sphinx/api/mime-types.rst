mime-types
==========

Lookup helpers and read-only data accessors ported from npm
``mime-types`` 3.0.2 with data generated from ``mime-db`` 1.54.0.
Every function is stateless — the 2,601-entry database is compiled in as
``constexpr`` data, so there is no startup cost and no concurrency
concern.

Lookup misses use C++ absence instead of JavaScript ``false``:
:cpp:func:`polycpp::mime::lookup`,
:cpp:func:`polycpp::mime::contentType`,
:cpp:func:`polycpp::mime::extension`, and
:cpp:func:`polycpp::mime::charset` return
``std::optional<std::string>``. The generated map surfaces are exposed as
read-only accessors rather than mutable JavaScript objects.

.. doxygenstruct:: polycpp::mime::TypeEntry
   :members:

.. doxygenstruct:: polycpp::mime::ExtensionConflict
   :members:

.. doxygenfunction:: polycpp::mime::lookup
.. doxygenfunction:: polycpp::mime::contentType
.. doxygenfunction:: polycpp::mime::extension
.. doxygenfunction:: polycpp::mime::charset
.. doxygenfunction:: polycpp::mime::types
.. doxygenfunction:: polycpp::mime::extensions
.. doxygenfunction:: polycpp::mime::extensionConflicts
