#pragma once
#ifndef P3D_ECS_PLAINTEXTSERIALIZER_H
#define P3D_ECS_PLAINTEXTSERIALIZER_H

#include "serialization.h"
#include "luse.h"
#include "uuid.h"

class PlainTextSerializer : public BaseSerializer {
public:
  PlainTextSerializer() : BaseSerializer() {}

  inline void begin_entity(const UUID &uuid) {
    _out << "entity" << endl;
    _out << "  uuid " << uuid.c_str() << endl;
    _out << "  components" << endl;
    _current_indent = "    ";
  }

  inline void end_entity() { _current_indent = ""; }

  inline void begin_component(const char *class_name) {
    _out << _current_indent << class_name << endl;
    _current_indent = "      ";
  }

  inline void end_component() { _current_indent = "    "; }

  template <typename T>
  inline void serialize_prop(const std::string &name, const T &prop) {
    _out << _current_indent << name << " ";
    write_value(prop);
    _out << "\n";
  }

  template <typename T>
  inline void serialize_prop_vec(const std::string &name,
                                 const std::vector<T> &vec) {
    _out << _current_indent << " " << name << " " << vec.size();
    for (const T &v : vec) {
      write_value(v);
      _out << " ";
    }
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
    _out << value.get_x() << " " << value.get_y() << " " << value.get_z();
  }

  template <> inline void write_value<LVecBase2f>(const LVecBase2f &value) {
    _out << value.get_x() << " " << value.get_y();
  }

  template <> inline void write_value<LMatrix4f>(const LMatrix4f &value) {
    _out << value;
  }

  template <> inline void write_value<EntityRef>(const EntityRef &value) {
    _out << value.get_id();
  }

  using entity_ptr_t = Entity
      *; // Compiler bug, does not detect this as specialization otherwise
  template <> inline void write_value<entity_ptr_t>(const entity_ptr_t &value) {
    write_entity(value);
  }

  void write_entity(const entity_ptr_t entity);

private:
  std::stringstream _out;
  std::string _current_indent;
};

#endif