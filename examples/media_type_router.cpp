// media_type_router — parse a Content-Type and route on its parts.
//
// Shows how to combine parse() + format() to make a small dispatch
// table keyed by (type, subtype, suffix). Reads one header line per
// stdin line; writes the routing decision to stdout.
//
// Example:
//   echo 'application/vnd.api+json' | media_type_router
//   -> route: json-api
#include <iostream>
#include <string>
#include <string_view>

#include <polycpp/core/error.hpp>
#include <polycpp/mime/mime.hpp>

namespace {

std::string_view route(const polycpp::mime::MediaType& mt) {
    // Structured-syntax suffix wins: if it's +json, treat it as JSON.
    if (mt.suffix == "json") {
        if (mt.subtype.starts_with("vnd.")) return "json-api";
        return "json";
    }
    if (mt.suffix == "xml") return "xml";

    if (mt.type == "application" && mt.subtype == "json")   return "json";
    if (mt.type == "application" && mt.subtype == "x-www-form-urlencoded")
        return "form";
    if (mt.type == "multipart" && mt.subtype == "form-data") return "multipart";
    if (mt.type == "text")                                   return "text";
    return "opaque";
}

}  // namespace

int main() {
    using namespace polycpp::mime;

    for (std::string line; std::getline(std::cin, line);) {
        if (line.empty()) continue;

        // Drop parameters before parse().
        auto semi = line.find(';');
        auto core = (semi == std::string::npos) ? line : line.substr(0, semi);

        if (!test(core)) {
            std::cout << line << "\tinvalid\n";
            continue;
        }

        try {
            auto mt = parse(core);
            std::cout << line << "\troute: " << route(mt)
                      << "\t(" << format(mt) << ")\n";
        } catch (const polycpp::TypeError& e) {
            std::cout << line << "\terror: " << e.what() << '\n';
        }
    }
    return 0;
}
