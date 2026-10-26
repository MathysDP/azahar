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
constexpr std::uint32_t Version = 5;

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
    BattleAction = 2,
    TurnState = 3,
    PartyChunk = 4,
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

/// An action a battle client committed in the sender's game: its player's menu choice, or a
/// foe's choice when the sender decides the foes. The receiver's game plays it for the same
/// Pokémon: the sender's client 0 is the receiver's ally, client 2, and foes 1 and 3 match.
struct BattleActionMessage {
    MessageType type = MessageType::BattleAction;
    std::uint8_t client = 0; ///< battle client in the sender's game
    std::uint8_t padding[2] = {};
    /// As the game encodes it: (move << 8) | (target position << 4) | command, target seen
    /// from the sender's side.
    std::uint32_t action = 0;
    std::uint32_t decision = 0; ///< 1 for the client's first decision of the battle
};
static_assert(sizeof(BattleActionMessage) <= MaxPayloadSize);

/// Battle state at the start of a turn, to detect the two games drifting apart.
/// HP by battle position as the sender sees it: 0 the sender, 2 its partner, 1 and 3 foes.
struct TurnStateMessage {
    MessageType type = MessageType::TurnState;
    std::uint8_t padding[3] = {};
    std::uint32_t turn = 0; ///< 1 for the first turn of the battle
    std::uint16_t hp[4] = {};
};
static_assert(sizeof(TurnStateMessage) <= MaxPayloadSize);

/// Size of one Pokémon as the game stores it, encrypted: 0xE8 bytes of core data followed by
/// 0x1C bytes of party stats. The encryption key is in the data, so it can be copied as is.
constexpr std::uint32_t PokemonSize = 0x104;
/// Pokémon a player brings to a multi battle.
constexpr std::uint32_t MaxPartySent = 3;
constexpr std::uint32_t PartyChunkBytes = 52;
constexpr std::uint32_t ChunksPerPokemon = (PokemonSize + PartyChunkBytes - 1) / PartyChunkBytes;

/// One piece of the sender's party. A full send is `count` Pokémon of ChunksPerPokemon chunks,
/// all with the same revision.
struct PartyChunkMessage {
    MessageType type = MessageType::PartyChunk;
    std::uint8_t revision = 0; ///< changes with each full send
    std::uint8_t count = 0;    ///< Pokémon in the send, 1..MaxPartySent
    std::uint8_t index = 0;    ///< chunk index: Pokémon * ChunksPerPokemon + piece
    std::uint8_t bytes[PartyChunkBytes] = {};
};
static_assert(sizeof(PartyChunkMessage) <= MaxPayloadSize);

} // namespace CoopProtocol
