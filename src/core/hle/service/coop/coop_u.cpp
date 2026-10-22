// Copyright 2026 Pokemon ROSA co-op contributors
// Licensed under GPLv2 or any later version
// Refer to the misc/licenses/gplv2.txt file included.

#include "common/archives.h"
#include "coop_protocol/coop_protocol.h"
#include "core/core.h"
#include "core/hle/ipc_helpers.h"
#include "core/hle/service/coop/coop_u.h"

SERIALIZE_EXPORT_IMPL(Service::COOP::COOP_U)

namespace Service::COOP {

COOP_U::COOP_U() : ServiceFramework(CoopProtocol::ServiceName, 1) {
    static const FunctionInfo functions[] = {
        {static_cast<u32>(CoopProtocol::Command::Ping), &COOP_U::Ping, "Ping"},
    };
    RegisterHandlers(functions);
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

void InstallInterfaces(Core::System& system) {
    std::make_shared<COOP_U>()->InstallAsService(system.ServiceManager());
}

} // namespace Service::COOP
