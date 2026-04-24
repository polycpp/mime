mime-types
==========

The four lookup helpers ported from npm ``mime-types``. Every function
is stateless — the 2,601-entry database is compiled in as
``constexpr`` data, so there is no startup cost and no concurrency
concern.

.. doxygenfunction:: polycpp::mime::lookup
.. doxygenfunction:: polycpp::mime::contentType
.. doxygenfunction:: polycpp::mime::extension
.. doxygenfunction:: polycpp::mime::charset
