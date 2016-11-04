#pragma once
#ifndef P3D_ECS_SERIALIZATION_H
#define P3D_ECS_SERIALIZATION_H

#include "luse.h"

#include <string>
#include <sstream>

class Entity;
#include "entity.h"

class BaseSerializer {};

class YAMLSerializer : public BaseSerializer {
public:
  YAMLSerializer() : BaseSerializer() {}

  template <typename T>
  inline void serialize_prop(const std::string &name, const T &prop) {
    _out << _current_indent << name << ": ";
    write_value(prop);
    _out << "\n";
  }

  std::string get_output() const { return _out.str(); };

private:
  template <typename T> inline void write_value(const T &value) {
    _out << "<Unkown prop: " << typeid(T).name() << ">";
  }

  template <> inline void write_value<float>(const float &value) {
    _out << value;
  }

  template <> inline void write_value<bool>(const bool &value) {
    _out << (value ? "true" : "false");
  }

  template <> inline void write_value<LVecBase3f>(const LVecBase3f &value) {
    _out << value.get_x() << ", " << value.get_y() << ", " << value.get_z();
  }

  template <> inline void write_value<LVecBase2f>(const LVecBase2f &value) {
    _out << value.get_x() << ", " << value.get_y();
  }

  template <> inline void write_value<EntityRef>(const EntityRef &value) {
      _out << "entity::" << value.get_id();
  }

private:
  std::stringstream _out;
  std::string _current_indent;
};

#endif