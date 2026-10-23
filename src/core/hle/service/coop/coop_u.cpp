// Copyright 2026 Pokemon ROSA co-op contributors
// Licensed under GPLv2 or any later version
// Refer to the misc/licenses/gplv2.txt file included.

#include <algorithm>
#include <cstring>
#include "common/archives.h"
#include "coop_protocol/coop_protocol.h"
#include "core/core.h"
#include "core/hle/ipc_helpers.h"
#include "core/hle/service/coop/coop_u.h"
#include "network/network.h"

SERIALIZE_EXPORT_IMPL(Service::COOP::COOP_U)

namespace Service::COOP {

namespace {
/// Oldest payloads are dropped beyond this, so a stalled game cannot grow memory.
constexpr std::size_t MaxInbox = 64;
} // namespace

COOP_U::COOP_U() : ServiceFramework(CoopProtocol::ServiceName, 1) {
    static const FunctionInfo functions[] = {
        {static_cast<u32>(CoopProtocol::Command::Ping), &COOP_U::Ping, "Ping"},
        {static_cast<u32>(CoopProtocol::Command::Send), &COOP_U::Send, "Send"},
        {static_cast<u32>(CoopProtocol::Command::Receive), &COOP_U::Receive, "Receive"},
    };
    RegisterHandlers(functions);

    if (auto member = Network::GetRoomMember().lock()) {
        member->SetCoopDataHandler([this](const std::string& nickname, const std::vector<u8>& data) {
            if (data.size() > CoopProtocol::MaxPayloadSize) {
                return;
            }
            LOG_INFO(Service, "coop:u received {} bytes from {}", data.size(), nickname);
            std::lock_guard lock(inbox_mutex);
            if (inbox.size() == MaxInbox) {
                inbox.pop_front();
            }
            inbox.push_back(data);
        });
    }
}

COOP_U::~COOP_U() {
    if (auto member = Network::GetRoomMember().lock()) {
        member->SetCoopDataHandler({});
    }
}

void COOP_U::Ping(Kernel::HLERequestContext& ctx) {
    IPC::RequestParser rp(ctx);
    const u32 nonce = rp.Pop<u32>();

    IPC::RequestBuilder rb = rp.MakeBuilder(3, 0);
    rb.Push(ResultSuccess);
    rb.Push(CoopProtocol::Version);
    rb.Push(nonce + 1);

    LOG_INFO(Service, "coop:u Ping nonce={:#x}", nonce);
}

void COOP_U::Send(Kernel::HLERequestContext& ctx) {
    IPC::RequestParser rp(ctx);
    const u32 size = std::min(rp.Pop<u32>(), CoopProtocol::MaxPayloadSize);
    std::array<u32, CoopProtocol::MaxPayloadWords> words;
    for (auto& word : words) {
        word = rp.Pop<u32>();
    }
    std::vector<u8> data(size);
    std::memcpy(data.data(), words.data(), size);

    u32 sent = 0;
    if (auto member = Network::GetRoomMember().lock(); member && member->IsConnected()) {
        member->SendCoopData(data);
        sent = 1;
    }
    LOG_INFO(Service, "coop:u Send {} bytes, sent={}", size, sent);

    IPC::RequestBuilder rb = rp.MakeBuilder(2, 0);
    rb.Push(ResultSuccess);
    rb.Push(sent);
}

void COOP_U::Receive(Kernel::HLERequestContext& ctx) {
    IPC::RequestParser rp(ctx);
    std::vector<u8> data;
    {
        std::lock_guard lock(inbox_mutex);
        if (!inbox.empty()) {
            data = std::move(inbox.front());
            inbox.pop_front();
        }
    }
    std::array<u32, CoopProtocol::MaxPayloadWords> words{};
    std::memcpy(words.data(), data.data(), data.size());

    IPC::RequestBuilder rb = rp.MakeBuilder(2 + CoopProtocol::MaxPayloadWords, 0);
    rb.Push(ResultSuccess);
    rb.Push(static_cast<u32>(data.size()));
    for (const u32 word : words) {
        rb.Push(word);
    }
}

void InstallInterfaces(Core::System& system) {
    std::make_shared<COOP_U>()->InstallAsService(system.ServiceManager());
}

} // namespace Service::COOP
