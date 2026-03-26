#pragma once

/**
 * @file aggregator.hpp
 * @brief Aggregator header that includes declarations and all detail
 *        implementation headers.
 *
 * This is the single header that users include via the top-level mime.hpp.
 * It ensures all inline definitions are available in every translation unit.
 */

#include <polycpp/mime/mime.hpp>
#include <polycpp/mime/detail/mime_db_data.hpp>
#include <polycpp/mime/detail/mime_types_impl.hpp>
#include <polycpp/mime/detail/media_typer_impl.hpp>
