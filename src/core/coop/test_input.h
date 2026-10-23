// Copyright 2026 Pokemon ROSA co-op contributors
// Licensed under GPLv2 or any later version
// Refer to the misc/licenses/gplv2.txt file included.

// Input injected by test scripts through the RPC server, so automated tests
// can drive the game without a human. Inactive unless a script sets it.

#pragma once

#include <optional>
#include "common/common_types.h"

namespace Coop::TestInput {

struct State {
    u32 buttons; ///< HID PadState bits, OR-ed with the real input
    s16 circle_x; ///< Circle pad position, -0x9A..0x9A, replaces the real one
    s16 circle_y;
};

/// Sets the injected input. All zeros releases it.
void Set(const State& state);

/// Returns the injected input, or nothing when released.
std::optional<State> Get();

} // namespace Coop::TestInput
