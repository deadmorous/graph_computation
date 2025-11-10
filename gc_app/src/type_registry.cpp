/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2024 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#include "gc_app/type_registry.hpp"

#include "gc_app/types/cell2d_rules.hpp"
#include "gc_app/types/image.hpp"
#include "gc_app/types/palette.hpp"

namespace gc_app {

auto populate_type_registry(gc::TypeRegistry& result)
    -> void
{
    result.register_value("Cell2dRules", gc::type_of<Cell2dRules>());
    result.register_value("Color", gc::type_of<Color>());
    result.register_value("IndexedPalette", gc::type_of<IndexedPalette>());
    result.register_value("UintSize", gc::type_of<UintSize>());
}

} // namespace gc_app
