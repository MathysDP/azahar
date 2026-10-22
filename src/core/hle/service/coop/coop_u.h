// Copyright 2026 Pokemon ROSA co-op contributors
// Licensed under GPLv2 or any later version
// Refer to the misc/licenses/gplv2.txt file included.

#pragma once

#include "core/hle/service/service.h"

namespace Core {
class System;
}

namespace Service::COOP {

/// Bridge between the co-op 3GX plugin running in the game and the emulator.
class COOP_U final : public ServiceFramework<COOP_U> {
public:
    COOP_U();

private:
    void Ping(Kernel::HLERequestContext& ctx);

    SERVICE_SERIALIZATION_SIMPLE
};

void InstallInterfaces(Core::System& system);

} // namespace Service::COOP

BOOST_CLASS_EXPORT_KEY(Service::COOP::COOP_U)
