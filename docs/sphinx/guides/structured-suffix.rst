Read the structured syntax suffix
=================================

**When to reach for this:** you need to know whether an incoming
``Content-Type`` is a JSON-shaped variant (``application/vnd.api+json``,
``application/activity+json``) without hard-coding a list of subtypes.

.. code-block:: cpp

   #include <polycpp/mime/detail/aggregator.hpp>

   bool isJsonShaped(const std::string& header) {
       using namespace polycpp::mime;
       if (!test(header.substr(0, header.find(';')))) return false;
       auto mt = parse(header);
       if (mt.suffix == "json")               return true;
       if (mt.type == "application"
           && mt.subtype == "json")            return true;
       return false;
   }

   // isJsonShaped("application/json")                 == true
   // isJsonShaped("application/vnd.api+json")         == true
   // isJsonShaped("application/vnd.api+json; q=0.9")  == true
   // isJsonShaped("text/html")                        == false

The :cpp:member:`polycpp::mime::MediaType::suffix` field holds the
structured-syntax suffix as defined by RFC 6838 §4.2.8 — that's
``json``, ``xml``, ``cbor``, ``ber``, and a handful of others.
:cpp:func:`polycpp::mime::parse` extracts it for you; you never need
to crack the subtype on ``+`` by hand.
