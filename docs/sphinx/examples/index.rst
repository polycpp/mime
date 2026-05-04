Examples
========

Self-contained programs exercising the main features of mime. Each
example compiles against the public API only — no private headers, no
non-exported targets.

.. toctree::
   :maxdepth: 1

   content-type-cli
   media-type-router

Running an example
------------------

From the repository root:

.. code-block:: bash

   cmake -B build -G Ninja
   cmake --build build --target polycpp_mime_example_content_type_cli
   ./build/examples/content_type_cli index.html .json

Standalone builds enable examples by default. When mime is consumed as a
subproject, pass ``-DPOLYCPP_MIME_BUILD_EXAMPLES=ON`` to build them.
