mime-types
==========

Lookup helpers and read-only data accessors ported from npm
``mime-types``. Every function is stateless — the 2,601-entry database
is compiled in as ``constexpr`` data, so there is no startup cost and no
concurrency concern.

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
