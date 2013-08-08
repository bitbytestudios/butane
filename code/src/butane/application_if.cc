// This file is part of Butane. See README.md and LICENSE.md for details.
// Copyright (c) 2012 Michael Williams <devbug@bitbyte.ca>

#include <butane/application_if.h>
#include <butane/application.h>

#include <butane/time_step_policy.h>

namespace butane {
  namespace {
    static int lua_application_platform( lua_State* L ) {
      lua_pushstring(L, Application::platform());
      return 1;
    }

    static int lua_application_architecture( lua_State* L ) {
      lua_pushstring(L, Application::architecture());
      return 1;
    }

    static int lua_application_build( lua_State* L ) {
      lua_pushstring(L, Application::build());
      return 1;
    }

    static int lua_application_pause( lua_State* L ) {
      Application::pause();
      return 0;
    }

    static int lua_application_unpause( lua_State* L ) {
      Application::unpause();
      return 0;
    }

    static int lua_application_quit( lua_State* L ) {
      Application::quit();
      return 0;
    }

    static int lua_application_time_step_policy( lua_State* L ) {
      switch (Application::time_step_policy().policy()) {
        case TimeStepPolicy::VARIABLE:
          lua_pushstring(L, "variable"); break;
        default:
          __builtin_unreachable(); }
      return 1;
    }

    static int lua_application_set_time_step_policy( lua_State* L ) {
      const char* policy = luaL_checkstring(L, 1);
      if (strcmp("variable", policy) == 0)
        Application::set_time_step_policy(TimeStepPolicy::variable());
      else
        luaL_argerror(L, 1, "expected 'variable', 'fixed', 'smoothed', or 'smoothed_with_debt_payback'");
      return 0;
    }
  }
} // butane

namespace butane {
  int luaopen_application( lua_State* L )
  {
    lua_createtable(L, 0, 6);
    lua_pushcfunction(L, &lua_application_platform);
    lua_setfield(L, -2, "platform");
    lua_pushcfunction(L, &lua_application_architecture);
    lua_setfield(L, -2, "architecture");
    lua_pushcfunction(L, &lua_application_build);
    lua_setfield(L, -2, "build");
    lua_pushcfunction(L, &lua_application_pause);
    lua_setfield(L, -2, "pause");
    lua_pushcfunction(L, &lua_application_unpause);
    lua_setfield(L, -2, "unpause");
    lua_pushcfunction(L, &lua_application_quit);
    lua_setfield(L, -2, "quit");
    lua_pushcfunction(L, &lua_application_time_step_policy);
    lua_setfield(L, -2, "time_step_policy");
    lua_pushcfunction(L, &lua_application_set_time_step_policy);
    lua_setfield(L, -2, "set_time_step_policy");
    return 1;
  }
} // butane
