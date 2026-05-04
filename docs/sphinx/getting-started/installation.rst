Installation
============

mime targets C++20. The documented support baseline is CMake 3.20+
with GCC 13+ or Clang 16+. Older compilers may work, but they are not
the tested support target. The library depends only on the base
`polycpp <https://github.com/enricohuang/polycpp>`_ library, which
CMake fetches transitively.

CMake FetchContent (supported)
------------------------------

Add the library to your ``CMakeLists.txt``:

.. code-block:: cmake

   include(FetchContent)

   FetchContent_Declare(
       polycpp_mime
       GIT_REPOSITORY https://github.com/polycpp/mime.git
       GIT_TAG        44646ebf70ec63bbd4a84fe391035cc6b9be2e56
   )
   FetchContent_MakeAvailable(polycpp_mime)

   add_executable(my_app main.cpp)
   target_link_libraries(my_app PRIVATE polycpp::mime)

FetchContent is the documented consumption path today. There is no
documented install package, ``find_package``, vcpkg, or Conan flow yet.
Until release tags exist, use a commit SHA for ``GIT_TAG`` rather than
a floating branch. The SHA above is a concrete example for mime; when
you upgrade, choose the commit you have tested.

The first configure pulls ``polycpp`` transitively, so the build tree
may be large. The default transitive fetch currently follows the
``polycpp`` ``master`` branch. If you need a fully reproducible build,
check out ``polycpp`` at a commit you have tested and pass that checkout
to CMake with ``FETCHCONTENT_SOURCE_DIR_POLYCPP`` or
``POLYCPP_SOURCE_DIR``.

Using a local clone
-------------------

If you already have mime and polycpp checked out side by side, tell
CMake to use them instead of fetching from GitHub. This is also the
way to make the transitive ``polycpp`` input fully local:

.. code-block:: bash

   cmake -B build -G Ninja \
       -DFETCHCONTENT_SOURCE_DIR_POLYCPP=/path/to/polycpp \
       -DFETCHCONTENT_SOURCE_DIR_POLYCPP_MIME=/path/to/mime

This is the path CI uses for the test suite — see ``tests/`` in the repo.
You can also use ``-DPOLYCPP_SOURCE_DIR=/path/to/polycpp`` when you only
need to override the transitive base library.

Build options
-------------

mime options
~~~~~~~~~~~~

``POLYCPP_MIME_BUILD_TESTS``
    Build the GoogleTest suite. Defaults to ``ON`` for standalone builds and
    ``OFF`` when consumed via FetchContent.

``POLYCPP_MIME_BUILD_EXAMPLES``
    Build the runnable programs under ``examples/``. Defaults to ``ON`` for
    standalone builds and ``OFF`` when consumed via FetchContent.

Transitive polycpp options
~~~~~~~~~~~~~~~~~~~~~~~~~~

These options belong to the transitive ``polycpp`` dependency. They may
appear in your CMake cache when FetchContent pulls ``polycpp``, but mime
itself does not perform I/O, TLS, or ICU work.

``POLYCPP_IO``
    ``asio`` (default) or ``libuv`` — inherited from polycpp.

``POLYCPP_SSL_BACKEND``
    ``boringssl`` (default) or ``openssl``.

``POLYCPP_UNICODE``
    ``icu`` (recommended) or ``builtin``. ICU enables the Intl surface that
    several polycpp headers pull into their public signatures.

Verifying the install
---------------------

.. code-block:: bash

   cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
   cmake --build build
   ctest --test-dir build --output-on-failure

All tests should pass on a supported toolchain — if they do not, open an
issue on the `repository <https://github.com/polycpp/mime/issues>`_
with the compiler version and the failing test name.
