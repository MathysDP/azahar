// Copyright 2026 Pokemon ROSA co-op contributors
// Licensed under GPLv2 or any later version
// Refer to the misc/licenses/gplv2.txt file included.

// Messages exchanged between the 3GX plugin and the coop:u service.
// Shared by Azahar (host compiler) and the plugin (devkitARM), so it must
// only depend on the C++ standard library.

#pragma once

#include <cstdint>

namespace CoopProtocol {

/// Bumped on every incompatible change. The plugin refuses to run when the
/// service reports a different version.
constexpr std::uint32_t Version = 1;

/// Name the plugin passes to srvGetServiceHandle.
constexpr char ServiceName[] = "coop:u";

/// IPC command IDs of coop:u.
enum class Command : std::uint16_t {
    /// In: u32 nonce. Out: result, u32 Version, u32 nonce + 1.
    Ping = 0x0001,
};

} // namespace CoopProtocol
