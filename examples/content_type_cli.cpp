// content_type_cli — print the Content-Type header for every argument.
//
// Usage:  content_type_cli file1.html path/to/img.png .json bogus
//
// Demonstrates the combined contentType + fallback pattern from
// docs/sphinx/guides/lookup-unknown-extension.rst.
#include <iostream>
#include <string>

#include <polycpp/mime/mime.hpp>

int main(int argc, char** argv) {
    using namespace polycpp::mime;

    if (argc < 2) {
        std::cerr << "usage: " << argv[0] << " <path-or-ext>...\n";
        return 64;  // EX_USAGE
    }

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        std::string ct;
        if (auto found = contentType(arg)) {
            ct = *found;
        } else {
            ct = "application/octet-stream";
        }
        std::cout << arg << "\t" << ct << '\n';
    }
    return 0;
}
