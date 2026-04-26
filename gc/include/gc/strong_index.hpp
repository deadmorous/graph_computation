/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#include "mpk/mix/strong/strong.hpp"

#include <cstdint>


namespace gc {

MPKMIX_STRONG_TYPE(Count, uint32_t, mpk::mix::StrongCountFeatures);
MPKMIX_STRONG_TYPE(Index, uint32_t, mpk::mix::StrongIndexFeatures<Count>);

} // namespace gc
