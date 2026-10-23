// Copyright 2026 Pokemon ROSA co-op contributors
// Licensed under GPLv2 or any later version
// Refer to the misc/licenses/gplv2.txt file included.

#include <atomic>
#include "core/coop/test_input.h"

namespace Coop::TestInput {

namespace {
// Packed as buttons | x << 32 | y << 48, so the HID thread reads it atomically.
std::atomic<u64> packed{0};
} // namespace

void Set(const State& state) {
    packed.store(u64{state.buttons} | u64{static_cast<u16>(state.circle_x)} << 32 |
                 u64{static_cast<u16>(state.circle_y)} << 48);
}

std::optional<State> Get() {
    const u64 value = packed.load();
    if (value == 0) {
        return std::nullopt;
    }
    return State{static_cast<u32>(value), static_cast<s16>(value >> 32),
                 static_cast<s16>(value >> 48)};
}

} // namespace Coop::TestInput
