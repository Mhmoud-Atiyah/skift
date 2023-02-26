#pragma once

#include <hal/vmm.h>
#include <handover/spec.h>
#include <hjert-api/api.h>
#include <karm-base/box.h>
#include <karm-base/rc.h>
#include <karm-io/traits.h>

namespace Hjert::Core {

struct Cpu;

struct Ctx;

struct Task;

struct Space;

} // namespace Hjert::Core

namespace Hjert::Arch {

Core::Cpu &cpu();

Res<> init(Handover::Payload &);

Hal::Vmm &vmm();

Io::TextWriter<> &loggerOut();

[[noreturn]] void stopAll();

void start(Core::Task &, uintptr_t ip, uintptr_t sp, Hj::Args args);

Res<Box<Core::Ctx>> createCtx(uintptr_t ksp);

Res<Strong<Core::Space>> createSpace();

} // namespace Hjert::Arch
