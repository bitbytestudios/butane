//===-- yeti/runtime/standard_application.cc ------------*- mode: C++11 -*-===//
//
//                             __ __     _   _
//                            |  |  |___| |_|_|
//                            |_   _| -_|  _| |
//                              |_| |___|_| |_|
//
//       This file is distributed under the terms described in LICENSE.
//
//===----------------------------------------------------------------------===//

#include "yeti/runtime/standard_application.h"

#include "yeti/runtime/manifest.h"

#include <stdlib.h>
#include <stdio.h>

// TODO(mtwilliams): Respect user settings.

namespace yeti {
namespace runtime {

StandardApplication::StandardApplication(const Manifest *manifest)
  : manifest_(manifest) {
}

StandardApplication::~StandardApplication() {
}

bool StandardApplication::startup() {
  // Start logging to the console and to a file.
  this->start_logging_to_console();
  this->start_logging_to_file();

  // Set the appropriate logging level based on build configuration.
  this->set_appropriate_logging_level();

  // Can't say we didn't tell ya'.
  this->log_copyright_notices();

  // Engine and content revisions are logged to aid when debugging and filing
  // crash reports.
  this->log_pertinent_information_about_build();

  // Hardware information is logged too, for the same reasons.
  this->log_pertinent_information_about_system();

  // TODO(mtwilliams): Load the renderer configuration specified by manfiest.
  // TODO(mtwilliams): Configure rendering.

  // Create the main window and default viewport.
  this->create_main_window_and_default_viewport();

  // Default to a variable time-step policy.
  TimeStepPolicy::Description time_step_policy_desc;
  time_step_policy_desc.type = yeti::TimeStepPolicy::VARIABLE;
  this->time_step_policy_ = yeti::TimeStepPolicy::create(time_step_policy_desc);

  // Expose ourself to Lua. Hehe.
  this->expose_to_lua();

  // Load boot package and script.
  this->load_boot_package_and_script();

  // Boot!
  if (!this->script_.call("startup", 0))
    return false;

  // Conceal boot time by only showing window after booting.
  windows_[0]->show();

  return true;
}

void StandardApplication::update(const f32 delta_time) {
  this->script_.environment()->reset();

  if (!this->script_.call("update", 1, Script::T_FLOAT, delta_time))
    this->pause();
}

void StandardApplication::render() {
  this->script_.environment()->reset();

  if (!this->script_.call("render", 0))
    this->pause();
}

void StandardApplication::shutdown() {
  core::logf(core::log::APP, core::log::INFO, "Shutting down...");

  this->script_.call("shutdown", 0);

  yeti::shutdown();
}

void StandardApplication::start_logging_to_console() const {
  YETI_NEW(core::log::ConsoleBackend, core::global_heap_allocator())();
}

void StandardApplication::start_logging_to_file() const {
  // TODO(mtwilliams): Log to `YYYY-MM-DD-HH-MM-SS.log`.
}

void StandardApplication::set_appropriate_logging_level() const {
  // TODO(mtwilliams): Set logging level.
}

void StandardApplication::log_copyright_notices() const {
  core::logf(core::log::APP, core::log::INFO,
    "                            __ __     _   _                            \n"
    "                           |  |  |___| |_|_|                           \n"
    "                           |_   _| -_|  _| |                           \n"
    "                             |_| |___|_| |_|                           \n\n"
  );

  core::logf(core::log::APP, core::log::INFO,
    "             %s             \n\n",
    __yeti_copyright__()
  );
}

void StandardApplication::log_pertinent_information_about_build() const {
  core::logf(core::log::APP, core::log::INFO,
    "Runtime\n"
    " platform = %s\n"
    " architecture = %s\n"
    " build = %s\n\n",
    platform(),
    architecture(),
    build()
  );

  // TODO(mtwilliams): Determine content version.
  core::logf(core::log::APP, core::log::INFO,
    "Version\n"
    " engine = %s\n"
    " content = %s\n\n",
    __yeti_version__(),
    "unknown"
  );
}

void StandardApplication::log_pertinent_information_about_system() const {
  // TODO(mtwilliams): Log CPU and GPU information.
}

void StandardApplication::create_main_window_and_default_viewport() {
  Window::Description wd; {
    wd.title = manifest_->app.name;

    wd.dimensions.width = manifest_->graphics.width;
    wd.dimensions.height = manifest_->graphics.height;

    wd.resizeable = false;

    // Quits application. See `Application::window_event_handler_`.
    wd.closable = true;
  }

  // Create main window.
  Window *window = this->open_a_window(wd);
}

void StandardApplication::expose_to_lua() {
  math_if::expose(&this->script_);
  application_if::expose(&this->script_, this);
  window_if::expose(&this->script_);
  viewport_if::expose(&this->script_);
  keyboard_if::expose(&this->script_);
  mouse_if::expose(&this->script_);
  world_if::expose(&this->script_);
  entity_if::expose(&this->script_);
  component_if::expose(&this->script_);
  transform_if::expose(&this->script_);
  camera_if::expose(&this->script_);
  light_if::expose(&this->script_);
}

void StandardApplication::load_boot_package_and_script() {
  // TODO(mtwilliams): Load the boot package specified by manfiest.

  const Resource::Type *script_resource_type =
    resource::type_from_name("script");

  const Resource::Type::Id script_resource_type_id =
    resource::id_from_type(script_resource_type);

  const Resource::Id boot_script_id =
    resource::id_from_name(script_resource_type_id,
                           manifest_->boot.script);

  ScriptResource *boot_script_resource =
    (ScriptResource *)resource_manager::lookup(boot_script_id);

  if (resource_manager::autoloads()) {
    while (resource_manager::state(boot_script_id) != Resource::LOADED)
      core::Thread::yield();
  } else {
    yeti_assert_with_reason(resource_manager::state(boot_script_id) == Resource::LOADED,
                            "Boot script is not part of boot package.");
  }

  this->script_.inject(boot_script_resource);

  boot_script_resource->deref();
}

} // runtime
} // yeti
