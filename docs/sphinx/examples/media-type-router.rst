Media type router
=================

A filter program that reads one ``Content-Type`` header per line from
stdin and writes a routing decision for each. Demonstrates the full
``test`` / ``parse`` / ``format`` pipeline and the discriminating power
of the structured-syntax suffix field.

.. literalinclude:: ../../../examples/media_type_router.cpp
   :language: cpp
   :caption: examples/media_type_router.cpp

Run it through a few headers:

.. code-block:: bash

   printf '%s\n' \
       'application/json' \
       'application/vnd.api+json' \
       'text/html; charset=utf-8' \
       'multipart/form-data; boundary=---' \
       'application/x-www-form-urlencoded' \
       'not a type' \
     | ./build/examples/media_type_router

Expected output:

.. code-block:: text

   application/json                                           route: json            (application/json)
   application/vnd.api+json                                   route: json-api        (application/vnd.api+json)
   text/html; charset=utf-8                                   route: text            (text/html)
   multipart/form-data; boundary=---                          route: multipart       (multipart/form-data)
   application/x-www-form-urlencoded                          route: form            (application/x-www-form-urlencoded)
   not a type                                                 invalid

The ``route()`` helper inside the program branches on
:cpp:member:`polycpp::mime::MediaType::suffix` first — that's what
makes ``application/vnd.api+json`` land in the same bucket as plain
JSON without a hand-maintained allowlist.
