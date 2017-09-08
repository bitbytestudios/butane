//===-- yeti/resources/script_resource.cc ---------------*- mode: C++11 -*-===//
//
//                             __ __     _   _
//                            |  |  |___| |_|_|
//                            |_   _| -_|  _| |
//                              |_| |___|_| |_|
//
//       This file is distributed under the terms described in LICENSE.
//
//===----------------------------------------------------------------------===//

#include "yeti/resources/script_resource.h"

#include "yeti/resource.h"
#include "yeti/resource_compiler.h"

// Compilation logic is factored into a seperate class.
#include "script_compiler.h"

extern "C" {
  #include <lua.h>
  #include <lauxlib.h>
  #include <lualib.h>
}

namespace yeti {

ScriptResource::ScriptResource(Resource::Id id)
  : Resource(id) {
}

ScriptResource::~ScriptResource() {
}

const Resource::Type *ScriptResource::type() {
  static const char *extensions[] = {"lua", (const char *)NULL};
  static const Resource::Type type = {
    /* .name = */ "script",
    /* .extensions = */ extensions,
    /* .prepare = */ &ScriptResource::prepare,
    /* .load = */ &ScriptResource::load,
    /* .unload = */ &ScriptResource::unload,
    /* .online = */ NULL,
    /* .offline = */ NULL,
    /* .compile = */ &ScriptResource::compile,
    /* .lifecycle_preference = */ resource::LifecyclePreferences::INDIFFERENT
  };

  return &type;
}

Resource *ScriptResource::prepare(Resource::Id id) {
  // TODO(mtwilliams): Use an arena allocator.
  return (Resource *)(new (foundation::heap()) ScriptResource(id));
}

void ScriptResource::load(Resource *resource, const Resource::Data &data) {
  ScriptResource *script_resource = (ScriptResource *)resource;

  foundation::fs::Info memory_resident_data_info;
  foundation::fs::info(data.memory_resident_data, &memory_resident_data_info);

  foundation::fs::read(data.memory_resident_data, (uintptr_t)&script_resource->path_[0], 256);

  script_resource->bytecode_len_ = memory_resident_data_info.size - 256;
  script_resource->bytecode_ = (const char *)foundation::heap().allocate(script_resource->bytecode_len_);
  foundation::fs::read(data.memory_resident_data, (uintptr_t)script_resource->bytecode_, script_resource->bytecode_len_);

  foundation::fs::close(data.memory_resident_data);
  foundation::fs::close(data.streaming_data);
}

void ScriptResource::unload(Resource *resource) {
  ScriptResource *script_resource = (ScriptResource *)resource;

  foundation::heap().deallocate((uintptr_t)script_resource->bytecode_);

  // BUG(mtwilliams): Potential memory leak?
  delete script_resource;
}

bool ScriptResource::compile(const resource_compiler::Environment *env,
                             const resource_compiler::Input *input,
                             const resource_compiler::Output *output) {
  return ScriptCompiler(env, input, output).run();
}

} // yeti
