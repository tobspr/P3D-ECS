#pragma once

#ifndef P3D_ECS_OUTPUT_H
#define P3D_ECS_OUTPUT_H

#include <vector>
#include <string>
#include <sstream>
#include <iostream>

namespace p3d_ecs {
  enum class Warning {

    // System
    SYS_NR = 0000,
    SYS_0001, // Memory Leak detected

    // Serialization
    SER_NR = 1000,
    SER_0001, // Parent specified without transform component, ignoring parent
    SER_0002, // Registered child was reparented to another parent in the meantime
    SER_0003, // Child no longer has a transform component, ignoring it
    SER_0004, // Removed transform component while still having children
    SER_0005, // Invalid UUID for entity in packet
    SER_0006, // Invalid UUID in vector of uuids in packet
    SER_0007, // Tried to fillin ptr but uuid was not found
    SER_0008, // Entity ptr in vector has invalid UUID

    // Entity Manager
    EM_NR = 2000,
    EM_0001, // Failed to find entity with id X
    EM_0002, // Failed to deserializa message, uuid not present or invalid
    EM_0003, // <UNUSED>
    EM_0004, // Referring by uuid to entity flagged for deletion
    EM_0005, // Failed to find entity by uuid

    // Entity
    ENT_NR = 3000,
    ENT_0001, // Remove() called twice
    ENT_0002, // No components definition found in entity message
    ENT_0003, // <UNUSED>
    ENT_0004, // Adding component twice
    ENT_0005, // Adding a component to an entity flagged for deletion

    UNKOWN = 9999, // Unkown warning
    _
  };

  class MessageManager {
  public:
    MessageManager() = delete;
    static void warn(Warning nr, const std::string& title, const std::string& message);

    static const std::vector<Warning>& get_occurred_warnings();
    static std::vector<Warning> take_occurred_warnings();
    static void clear_occured_warnings();

    static bool take_warning(Warning nr);
    static bool take_warnings(const std::vector<Warning>& nrs);

  private:
    static std::vector<Warning> _warnings;
  };

  inline std::ostream& operator<<(std::ostream& stream, Warning w) {
    return stream << static_cast<size_t>(w);
  }
}

#define ECS_EMIT_WARNING(nr, content)                                                              \
  {                                                                                                \
    std::stringstream _warn_stream;                                                                \
    _warn_stream << content;                                                                       \
    ::p3d_ecs::MessageManager::warn(::p3d_ecs::Warning::##nr, "" #nr, _warn_stream.str());         \
  \
}

#endif