// Copyright 2026 Pokemon ROSA co-op contributors
// Licensed under GPLv2 or any later version
// Refer to the misc/licenses/gplv2.txt file included.

#pragma once

#include <deque>
#include <mutex>
#include <vector>
#include "core/hle/service/service.h"

namespace Core {
class System;
}

namespace Service::COOP {

/// Bridge between the co-op 3GX plugin running in the game and the emulator.
/// Payloads travel through the Azahar room the emulator has joined.
class COOP_U final : public ServiceFramework<COOP_U> {
public:
    COOP_U();
    ~COOP_U() override;

private:
    void Ping(Kernel::HLERequestContext& ctx);
    void Send(Kernel::HLERequestContext& ctx);
    void Receive(Kernel::HLERequestContext& ctx);

    /// Payloads received from the room, filled on the network thread.
    std::mutex inbox_mutex;
    std::deque<std::vector<u8>> inbox;

    SERVICE_SERIALIZATION_SIMPLE
};

void InstallInterfaces(Core::System& system);

} // namespace Service::COOP

BOOST_CLASS_EXPORT_KEY(Service::COOP::COOP_U)
