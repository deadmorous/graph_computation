/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc_app/nodes/visual/image_loader.hpp"

#include "gc_app/types/image.hpp"
#include "gc_app/types/palette.hpp"
#include "gc_app/types/uint_vec.hpp"

#include "gc/computation_node.hpp"
#include "gc/expect_n_node_args.hpp"
#include "gc/node_port_names.hpp"

#include "common/defer.hpp"

#include <png.h>

#include <cmath>
#include <format>
#include <numbers>


using namespace std::literals;
using namespace gc::literals;

namespace gc_app::visual {

namespace {

using common::Defer;

// Simple struct to hold the resulting image data
struct IndexedImage final
{
    I8Image image;
    IndexedColorMap palette;
};

// Custom Error Handler (must throw to integrate with C++)
auto custom_error_fn(png_structp png_ptr, png_const_charp msg) -> void
{
    // Simply throw a C++ exception, triggering stack unwinding
    // (and Defer destructors)
    throw std::runtime_error(msg);
}

auto load_indexed_png(const std::string& path) -> IndexedImage
{
    IndexedImage result = {};
    FILE *fp = nullptr;
    png_structp png_ptr = nullptr;
    png_infop info_ptr = nullptr;
    png_infop* info_ptr_ptr = nullptr;
    png_bytep* row_pointers = nullptr;

    // --- 1. Open File ---
    fp = fopen(path.c_str(), "rb");
    if (!fp)
        throw std::runtime_error(
            std::format("Failed to open file '{}' for reading.", path));

    // Defer 1: Ensure file is closed on exit/exception
    auto defer_close_file = Defer{ [&]{ fclose(fp); } };

    // --- 2. Check Signature ---
    png_byte sig[8];
    if (fread(sig, 1, 8, fp) != 8 || png_sig_cmp(sig, 0, 8) != 0)
        throw std::runtime_error(
            std::format("File '{}' is not a valid PNG.", path));

    // --- 3. Initialize libpng Structures ---
    // Note: The third argument (error_fn) is our custom function that throws.
    // The second argument (error_ptr) can be NULL here, as we don't need
    // context since the exception will handle the required cleanup via Defer.
    png_ptr = png_create_read_struct(
        PNG_LIBPNG_VER_STRING, NULL, custom_error_fn, NULL);
    if (!png_ptr)
        throw std::runtime_error("Failed to create png_struct.");
    // Defer 2: Ensure libpng memory is freed on exit/exception.
    // We pass NULL for the info pointers because they will be cleaned up
    // either separately (Defer 3) or before this Defer executes if
    // initialization failed.
    auto defer_destroy_read = Defer{
        [&]{ png_destroy_read_struct(&png_ptr, info_ptr_ptr, NULL); }
    };

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
        throw std::runtime_error("Failed to create png_info struct.");

    info_ptr_ptr = &info_ptr;

    // Defer 3: If an error occurs between here and Defer 2's execution,
    // this ensures info_ptr is cleaned up. We only need to destroy the info
    // struct, as the read struct cleanup is handled by Defer 2.
    // However, since png_destroy_read_struct also cleans up info_ptr,
    // it's cleaner to handle both together (see next comment block).

    // TRICKY PART: The safest and most common practice is to let the main
    // cleanup (Defer 2) handle the info_ptr as well, but this requires placing
    // the defer after the info_ptr is created and modifying its cleanup lambda.

    // --- 4. Setup I/O and Read Info ---
    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8); // We already read 8 bytes
    png_read_info(png_ptr, info_ptr);

    // --- 5. Image & Palette Info Check ---
    int color_type, bit_depth;
    auto width = png_uint_32{};
    auto height = png_uint_32{};
    png_get_IHDR(png_ptr, info_ptr, &width, &height,
                 &bit_depth, &color_type, NULL, NULL, NULL);
    result.image.size.width = width;
    result.image.size.height = height;

    if (color_type != PNG_COLOR_TYPE_PALETTE)
        throw std::runtime_error(
            "PNG is not in indexed (palette) color format.");

    // Force to 8-bit packing for easy index array access (1 byte per index)
    if (bit_depth < 8)
        png_set_packing(png_ptr);

    png_read_update_info(png_ptr, info_ptr);

    // --- 6. Extract Palette ---
    png_colorp palette_ptr;
    int num_palette;
    png_get_PLTE(png_ptr, info_ptr, &palette_ptr, &num_palette);

    // Copy the palette data into our result struct
    result.palette.reserve(num_palette);
    for (auto i: common::index_range<size_t>(num_palette))
    {
        auto& c = palette_ptr[i];
        result.palette.push_back(rgba(
            ColorComponent{c.red},
            ColorComponent{c.green},
            ColorComponent{c.blue}));
    }

    // --- 7. Read Pixel Indices (The 2D Array) ---
    png_uint_32 row_bytes = png_get_rowbytes(png_ptr, info_ptr);

    // Allocate memory for the row pointers and the actual pixel data
    row_pointers = (png_bytep*)png_malloc(png_ptr, height * sizeof(png_bytep));

    // Defer 4: Ensure the row pointer memory is freed.
    // This MUST be done before Defer 2 executes, as it relies on png_ptr.
    auto defer_free_rows = Defer{
        [&] {
            if (row_pointers) {
                for (png_uint_32 y = 0; y < height; y++) {
                    png_free(png_ptr, row_pointers[y]);
                }
                png_free(png_ptr, row_pointers);
            }
        }
    };

    // Allocate memory for each row and set up the row_pointers array
    for (png_uint_32 y = 0; y < height; y++)
        row_pointers[y] = (png_bytep)png_malloc(png_ptr, row_bytes);

    // Read the image data into our 2D array
    png_read_image(png_ptr, row_pointers);

    // --- 8. Flatten Data into a 1D Index Array (for C++ vector) ---
    result.image.data.reserve(width * height);
    for (png_uint_32 y = 0; y < height; y++) {
        png_bytep row = row_pointers[y];
        for (png_uint_32 x = 0; x < width; x++) {
            // Since we set packing, each byte is a single index
            result.image.data.push_back(row[x]);
        }
    }

    // After this point, the Defer destructors will run in reverse order of creation:
    // defer_free_rows -> defer_destroy_read -> defer_close_file.
    // This cleans up everything automatically and safely!

    return result;
}

} // anonymous namespace

class ImageLoader final :
    public gc::ComputationNode
{
public:
    auto input_names() const
        -> gc::InputNames override
    {
        return gc::node_input_names<ImageLoader>("file"sv);
    }

    auto output_names() const
        -> gc::OutputNames override
    { return gc::node_output_names<ImageLoader>("image"sv, "palette"sv); }

    auto default_inputs(gc::InputValues result) const
        -> void override
    {
        assert(result.size() == 1_gc_ic);
        result[0_gc_i] = "image.png"s;
    }

    auto compute_outputs(
            gc::OutputValues result,
            gc::ConstInputValues inputs,
            const std::stop_token& stoken,
            const gc::NodeProgress& progress) const
        -> bool override
    {
        assert(inputs.size() == 1_gc_ic);
        assert(result.size() == 2_gc_oc);
        const auto& path = inputs[0_gc_i].as<std::string>();
        auto [image, palette] = load_indexed_png(path);
        result[0_gc_o] = std::move(image);
        result[1_gc_o] = std::move(palette);
        return true;
    }

private:

    struct LoadContext final
    {
        const std::string& path;
        static auto error_fn(png_structp, png_const_charp);
    };
};

auto make_image_loader(gc::ConstValueSpan args)
    -> std::shared_ptr<gc::ComputationNode>
{
    gc::expect_no_node_args("ImageLoader", args);
    return std::make_shared<ImageLoader>();
}

} // namespace gc_app::visual
