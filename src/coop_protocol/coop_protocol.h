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
constexpr std::uint32_t Version = 3;

/// Name the plugin passes to srvGetServiceHandle.
constexpr char ServiceName[] = "coop:u";

/// Largest payload of Send and Receive, carried inline as 32-bit words.
constexpr std::uint32_t MaxPayloadWords = 16;
constexpr std::uint32_t MaxPayloadSize = MaxPayloadWords * 4;

/// IPC command IDs of coop:u.
enum class Command : std::uint16_t {
    /// In: u32 nonce. Out: result, u32 Version, u32 nonce + 1.
    Ping = 0x0001,
    /// In: u32 size, MaxPayloadWords words. Out: result, u32 sent (0 when not in a room).
    /// The payload goes to every other member of the room.
    Send = 0x0002,
    /// In: nothing. Out: result, u32 size (0 when nothing is queued), MaxPayloadWords words.
    /// Returns the oldest payload received from another member.
    Receive = 0x0003,
};

/// First byte of every payload sent with Command::Send.
enum class MessageType : std::uint8_t {
    PlayerPosition = 1,
    AllyAction = 2,
};

/// The sender's player position in world units (one tile is 18 units).
struct PlayerPositionMessage {
    MessageType type = MessageType::PlayerPosition;
    std::uint8_t padding[3] = {};
    float x = 0;
    float y = 0;
    float z = 0;
};
static_assert(sizeof(PlayerPositionMessage) <= MaxPayloadSize);

/// The sender's choice for its Pokémon, which the receiver's game plays as the battle ally.
struct AllyActionMessage {
    MessageType type = MessageType::AllyAction;
    std::uint8_t move_slot = 0; ///< 0..3, index in the Pokémon's move list
    std::uint8_t target = 0;    ///< battle position targeted, as the game's AI encodes it
    std::uint8_t padding = 0;
    std::uint32_t sequence = 0; ///< increases with each choice, so a repeat is not replayed
};
static_assert(sizeof(AllyActionMessage) <= MaxPayloadSize);

} // namespace CoopProtocol
