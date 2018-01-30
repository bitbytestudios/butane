//===-- yeti/application.cc -----------------------------*- mode: C++11 -*-===//
//
//                             __ __     _   _
//                            |  |  |___| |_|_|
//                            |_   _| -_|  _| |
//                              |_| |___|_| |_|
//
//       This file is distributed under the terms described in LICENSE.
//
//===----------------------------------------------------------------------===//

#include "yeti/application.h"

#include "yeti/application/time_step_policy.h"

#include "yeti/world.h"

#include "yeti/window.h"

// We automatically hook up windows to input singletons.
#include "yeti/input.h"

// To do some work while waiting.
#include "yeti/task.h"
#include "yeti/task_scheduler.h"

namespace yeti {

Application::Application()
  : time_step_policy_(NULL)
  , windows_(core::global_heap_allocator(), 0)
  , worlds_(core::global_heap_allocator(), 0)
  , logical_frame_count_(0)
  , visual_frame_count_(0) {
}

Application::~Application() {
  // There's no need to clean up after ourselves as the operating system will
  // do that for us. Ever use software that takes more than a second to close?
  // That's because they follow "best" practices and make sure everything is
  // properly destructed and deallocated before they exit.
}

const char *Application::platform() {
#if YETI_PLATFORM == YETI_PLATFORM_WINDOWS
  return "windows";
#elif YETI_PLATFORM == YETI_PLATFORM_MAC
  return "mac";
#elif YETI_PLATFORM == YETI_PLATFORM_LINUX
  return "linux";
#elif YETI_PLATFORM == YETI_PLATFORM_IOS
  return "ios";
#elif YETI_PLATFORM == YETI_PLATFORM_ANDROID
  return "android";
#endif
}

const char *Application::architecture() {
#if YETI_ARCHITECTURE == YETI_ARCHITECTURE_X86
  return "x86";
#elif YETI_ARCHITECTURE == YETI_ARCHITECTURE_X86_64
  return "x86_64";
#endif
}

const char *Application::build() {
#if YETI_CONFIGURATION == YETI_CONFIGURATION_DEBUG
  return "debug";
#elif YETI_CONFIGURATION == YETI_CONFIGURATION_DEVELOPMENT
  return "development";
#elif YETI_CONFIGURATION == YETI_CONFIGURATION_RELEASE
  return "release";
#endif
}

bool Application::startup() {
  return true;
}

void Application::shutdown() {
}

void Application::update(const f32 delta_time) {
  // No time travel, please.
  yeti_assert_debug(delta_time >= 0.f);
}

void Application::render() {
}

Window *Application::open_a_window(const Window::Description &desc) {
  Window *window = Window::open(desc);

  // Hook up input to input singletons.
  yeti::input::from(window);

  // Store a reference.
  windows_.push(window);

  return window;
}

void Application::close_a_window(Window *window) {
  // Swap and pop.
  const size_t index = windows_.position(window);
  windows_[index] = windows_[windows_.size() - 1];
  windows_.pop();

  // Close.
  window->close();
}

void Application::window_event_handler_(Window *window,
                                        const Window::Event &event,
                                        void *self) {
  Application *app = (Application *)self;

  const size_t id = app->windows_.position(window);

  switch (event.type) {
    case Window::Event::CLOSED: {

      if (id == 0)
        // Main window closed. Quit.
        app->quit();

      // Swap and pop.
      app->windows_[id] = app->windows_[app->windows_.size() - 1];
      app->windows_.pop();
    } break;
  }
}

World *Application::create_a_world() {
  World *world = World::create();

  // Store a reference.
  worlds_.push(world);

  return world;
}

void Application::update_a_world(World *world,
                                 const f32 delta_time) {
  world->update(delta_time);
}

#if 0
void Application::render_a_world(const World *world,
                                 Camera::Handle camera,
                                 Renderer::Viewport *viewport) {
}
#endif

void Application::destroy_a_world(World *world) {
  // Swap and pop.
  const size_t index = worlds_.position(world);
  worlds_[index] = worlds_[worlds_.size() - 1];
  worlds_.pop();

  // Destroy.
  world->destroy();
}

void Application::run() {
  core::Timer frame_timer;
  core::Timer wall_timer;

  this->startup();

  for (;;) {
    for (Window **window = windows_.begin(); window < windows_.end(); ++window)
      (*window)->update(&window_event_handler_, (void *)this);

    yeti_assert_with_reason_development(time_step_policy_ != NULL,
                                        "You must specify a time-step policy!");

    time_step_policy_->update(frame_timer, wall_timer);

    // It's important that these are pulled out prior to updating, just in case
    // the time-step policy is changed during a step.
    const u32 steps = time_step_policy_->steps();
    const f32 delta_time_per_step = time_step_policy_->delta_time_per_step();

    yeti_assert_debug(steps >= 1);

    for (u32 step = 0; step < steps; ++step)
      this->update(delta_time_per_step);

    logical_frame_count_ += 1;

    // TODO(mtwilliams): Limit latency.
    this->render();

    visual_frame_count_ += 1;

    yeti::Keyboard::update();
    yeti::Mouse::update();

    // BUG(mtwilliams): Due to inadequate timing resolution, this may result in
    // all frames taking `zero' microseconds unless enough work is performed
    // every frame.
    frame_timer.reset();
  }

  YETI_UNREACHABLE();
}

void Application::pause() {
  yeti_assert_with_reason(0, "Pausing and unpausing applications has not been implemented yet.");
}

void Application::unpause() {
  yeti_assert_with_reason(0, "Pausing and unpausing applications has not been implemented yet.");
}

void Application::quit() {
  this->shutdown();

  ::exit(EXIT_SUCCESS);

  YETI_UNREACHABLE();
}

TimeStepPolicy *Application::time_step_policy() {
  return time_step_policy_;
}

const TimeStepPolicy *Application::time_step_policy() const {
  return time_step_policy_;
}

void Application::set_time_step_policy(TimeStepPolicy *new_time_step_policy) {
  if (time_step_policy_)
    time_step_policy_->destroy();

  time_step_policy_ = new_time_step_policy;
}

const core::Array<Window *> &Application::windows() const {
  return windows_;
}

const core::Array<World *> &Application::worlds() const {
  return worlds_;
}

} // yeti
