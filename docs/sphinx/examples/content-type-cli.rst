Content-Type CLI
================

A one-shot command-line tool that prints the ``Content-Type`` header
for each path or extension passed on the command line. Useful for
spot-checking what a static-file middleware would emit before you put
it behind a real server.

.. literalinclude:: ../../../examples/content_type_cli.cpp
   :language: cpp
   :caption: examples/content_type_cli.cpp

Run it with a handful of paths:

.. code-block:: bash

   ./build/examples/content_type_cli index.html style.css logo.svg README

Expected output:

.. code-block:: text

   index.html      text/html; charset=utf-8
   style.css       text/css; charset=utf-8
   logo.svg        image/svg+xml
   README          application/octet-stream

The last line shows the fallback path from
:doc:`../guides/lookup-unknown-extension`.
