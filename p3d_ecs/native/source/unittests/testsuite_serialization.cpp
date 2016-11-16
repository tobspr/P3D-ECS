
#include "testsuite_serialization.h"

#include "unittest.h"
#include "transform_component.h"
#include "physics_component.h"

#include "google/protobuf/text_format.h"

using namespace p3d_ecs;

void
testsuite_serialization() {

  auto print_message = [](::google::protobuf::Message* msg) {
    REQUIRE_TRUE(msg != nullptr);
#if 0
    cout << "Serialized message:" << endl;
    cout << "Binary content: '";
    msg->SerializeToOstream(&cout);
    cout << "'" << endl;
    std::string json;
    ::google::protobuf::TextFormat::PrintToString(*msg, &json);
    cout << "JSON: " << json << endl;
    cout << endl;
#endif
    return true;
  };

  BEGIN_TESTCASE("Serializing a component") {

    Entity* entity = mgr->new_entity();

    TransformComponent& transform = entity->new_component<TransformComponent>();

    proto::Components* components = new proto::Components;
    transform.serialize(components);

    print_message(components);
    delete components;

    entity->remove();
    mgr->process_changes();
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Serializing a mod. component") {

    Entity* entity = mgr->new_entity();

    TransformComponent& transform = entity->new_component<TransformComponent>();
    transform.set_pos({ 1, 2, 3 });
    transform.set_scale({ 4, 2, 1 });
    transform.set_hpr({ 30, 60, 120 });

    proto::Components* components = new proto::Components;
    transform.serialize(components);
    print_message(components);

    delete components;
    entity->remove();

    mgr->process_changes();
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Serializing a comp w/ parent") {

    Entity* entity1 = mgr->new_entity();
    Entity* entity2 = mgr->new_entity();
    TransformComponent& transform1 = entity1->new_component<TransformComponent>();
    TransformComponent& transform2 = entity2->new_component<TransformComponent>();

    Entity* root = mgr->new_entity();
    TransformComponent& root_transform = root->new_component<TransformComponent>();

    transform1.set_parent(root);
    transform2.set_parent(root);

    proto::Components* components = new proto::Components;
    transform1.serialize(components);
    print_message(components);

    delete components;

    proto::Components* components_2 = new proto::Components;
    root_transform.serialize(components_2);
    print_message(components_2);

    delete components_2;

    entity1->remove();
    entity2->remove();
    root->remove();

    mgr->process_changes();
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Serializing a simple entity") {

    Entity* entity = mgr->new_entity();

    proto::Entity* serialized = entity->serialize();
    print_message(serialized);
    delete serialized;

    entity->remove();
    mgr->process_changes();
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Serializing a multi-comp entity") {

    Entity* entity = mgr->new_entity();
    TransformComponent& transform = entity->new_component<TransformComponent>();
    PhysicsComponent& physics = entity->new_component<PhysicsComponent>();

    physics.set_mass(15.0);
    physics.set_is_static(true);
    transform.set_pos({ 1, 2, 3 });
    transform.set_scale({ 4, 2, 1 });
    transform.set_hpr({ 30, 60, 120 });

    auto serialized = entity->serialize();

    print_message(serialized);

    delete serialized;

    entity->remove();
    mgr->process_changes();
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Simple [de]serializing") {
    Entity* entity = mgr->new_entity();

    auto serialized = entity->serialize();
    print_message(serialized);

    Entity* deserialized = mgr->deserialize_entity(serialized);
    mgr->process_changes();

    REQUIRE_PTR(deserialized);
    REQUIRE_TRUE(entity->get_uuid() == deserialized->get_uuid());
    REQUIRE_TRUE(entity->data_equals(deserialized));
    delete serialized;
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Simple [de]serializing without components") {
    Entity* entity = mgr->new_entity();

    auto serialized = new proto::Entity;
    serialized->set_uuid(entity->get_uuid().c_str(), UUID::uuid_length);
    print_message(serialized);

    entity->remove();
    mgr->process_changes();
    Entity* deserialized;
    {
      REQUIRE_WARNING_SCOPED(ENT_0002);
      deserialized = mgr->deserialize_entity(serialized);
    }
    mgr->process_changes();

    REQUIRE_PTR(deserialized);
    delete serialized;
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Simple [de]serializing with components") {

    Entity* entity = mgr->new_entity();
    TransformComponent& transform = entity->new_component<TransformComponent>();
    PhysicsComponent& physics = entity->new_component<PhysicsComponent>();

    physics.set_mass(15.0);
    physics.set_is_static(true);
    transform.set_pos({ 1, 2, 3 });
    transform.set_scale({ 4, 2, 1 });
    transform.set_hpr({ 30, 60, 120 });

    auto serialized = entity->serialize();

    print_message(serialized);

    istringstream input_str(serialized->SerializeAsString());

    Entity* deserialized = mgr->deserialize_entity(serialized);
    mgr->process_changes();

    REQUIRE_PTR(deserialized);
    REQUIRE_TRUE(entity->get_uuid() == deserialized->get_uuid());
    REQUIRE_TRUE(entity->data_equals(deserialized));
    delete serialized;
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Deserialization after deletion") {

    Entity* entity = mgr->new_entity();
    REQUIRE_FALSE(entity->get_needs_fillin());

    std::string uuid_copy(entity->get_uuid().c_str());

    auto serialized = entity->serialize();
    REQUIRE_PTR(serialized);

    entity->remove();
    mgr->process_changes();
    REQUIRE_EQ(mgr->get_num_entities(), 0u);

    Entity* deserialized = mgr->deserialize_entity(serialized);

    REQUIRE_TRUE(deserialized->get_needs_fillin());
    mgr->process_changes();
    REQUIRE_FALSE(deserialized->get_needs_fillin());

    REQUIRE_EQ(deserialized->get_num_components(), 0u);
    REQUIRE_EQ(deserialized->get_component_mask(), 0u);

    REQUIRE_EQ(std::string(deserialized->get_uuid().c_str()), uuid_copy);
    deserialized->remove();
    mgr->process_changes();

    delete serialized;
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Delete deserialized entity before process_changes") {
    Entity* entity = mgr->new_entity();
    TransformComponent& transform = entity->new_component<TransformComponent>();
    auto serialized = entity->serialize();
    entity->remove();
    mgr->process_changes();

    Entity* deserialized = mgr->deserialize_entity(serialized);
    deserialized->remove();
    REQUIRE_EQ(deserialized->get_needs_fillin(), false);
    mgr->process_changes();

    REQUIRE_EQ(mgr->get_num_entities(), 0u);
    delete serialized;
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Parent/Child: Serialize and deserialize child") {

    Entity* parent = mgr->new_entity();
    TransformComponent& parent_tr = parent->new_component<TransformComponent>();

    Entity* child = mgr->new_entity();
    TransformComponent& child_tr = child->new_component<TransformComponent>();

    child_tr.set_parent(parent);

    REQUIRE_EQ(child_tr.get_parent(), parent);
    REQUIRE_EQ(parent_tr.get_children(), { child });
    auto serialized = child->serialize();
    child->remove();

    mgr->process_changes();
    REQUIRE_EQ(parent_tr.get_children(), {});

    Entity* child_again = mgr->deserialize_entity(serialized);
    mgr->process_changes();
    TransformComponent& child_again_tr = child_again->get_component<TransformComponent>();

    REQUIRE_EQ(parent_tr.get_children(), { child_again });
    REQUIRE_EQ(child_again_tr.get_parent(), parent);

    delete serialized;
    child_again->remove();

    parent->remove();
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Parent/Child: [De]serialize child - Remove Transform inbetween") {

    Entity* parent = mgr->new_entity();
    TransformComponent& parent_tr = parent->new_component<TransformComponent>();

    Entity* child = mgr->new_entity();
    TransformComponent& child_tr = child->new_component<TransformComponent>();

    child_tr.set_parent(parent);

    REQUIRE_EQ(child_tr.get_parent(), parent);
    REQUIRE_EQ(parent_tr.get_children(), { child });
    auto serialized = child->serialize();
    child->remove();

    mgr->process_changes();
    REQUIRE_EQ(parent_tr.get_children(), {});

    parent->remove_component<TransformComponent>();
    mgr->process_changes();

    Entity* child_again = mgr->deserialize_entity(serialized);
    {
      REQUIRE_WARNING_SCOPED(SER_0001);
      mgr->process_changes();
    }

    TransformComponent& child_again_tr = child_again->get_component<TransformComponent>();

    REQUIRE_EQ(child_again_tr.get_parent(), nullptr);

    delete serialized;
    child_again->remove();

    parent->remove();
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Serializing parent, remvoing inbetween") {

    Entity* parent = mgr->new_entity();
    Entity* child = mgr->new_entity();
    TransformComponent& parent_tr = parent->new_component<TransformComponent>();
    TransformComponent& child_tr = child->new_component<TransformComponent>();

    child_tr.set_parent(parent);

    REQUIRE_EQ(parent_tr.get_children(), { child });
    REQUIRE_EQ(child_tr.get_parent(), parent);

    auto serialized = parent->serialize();
    {
      REQUIRE_WARNING_SCOPED(SER_0004);
      parent->remove();
    }
    mgr->process_changes();

    REQUIRE_EQ(child_tr.get_parent(), nullptr);

    Entity* parent_again = mgr->deserialize_entity(serialized);
    mgr->process_changes();

    REQUIRE_EQ(parent_again->get_component<TransformComponent>().get_children(), { child });
    REQUIRE_EQ(child_tr.get_parent(), parent_again);

    delete serialized;
    child->remove();
    parent_again->remove();
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Serializing parent, remvoing transform component inbetween") {

    Entity* parent = mgr->new_entity();
    Entity* child = mgr->new_entity();
    TransformComponent& parent_tr = parent->new_component<TransformComponent>();
    TransformComponent& child_tr = child->new_component<TransformComponent>();

    child_tr.set_parent(parent);

    REQUIRE_EQ(parent_tr.get_children(), { child });
    REQUIRE_EQ(child_tr.get_parent(), parent);

    auto serialized = parent->serialize();
    child->remove_component<TransformComponent>();
    parent->remove();
    mgr->process_changes();

    Entity* parent_again = mgr->deserialize_entity(serialized);
    {
      REQUIRE_WARNING_SCOPED(SER_0003);
      mgr->process_changes();
    }

    REQUIRE_EQ(parent_again->get_component<TransformComponent>().get_children(), {});
    delete serialized;
    child->remove();
    parent_again->remove();
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Parent/Child: [De]serialize child - reparent inbetween") {

    Entity* parent = mgr->new_entity();
    TransformComponent& parent_tr = parent->new_component<TransformComponent>();

    Entity* parent2 = mgr->new_entity();
    TransformComponent& parent2_tr = parent2->new_component<TransformComponent>();

    Entity* child = mgr->new_entity();
    TransformComponent& child_tr = child->new_component<TransformComponent>();

    child_tr.set_parent(parent);

    REQUIRE_EQ(child_tr.get_parent(), parent);
    REQUIRE_EQ(parent_tr.get_children(), { child });
    auto serialized = parent->serialize();

    {
      REQUIRE_WARNING_SCOPED(SER_0004);
      parent->remove();
    }

    mgr->process_changes();

    // Set new parent inbetween
    child_tr.set_parent(parent2);
    mgr->process_changes();
    REQUIRE_EQ(parent2_tr.get_children(), { child });
    REQUIRE_EQ(child_tr.get_parent(), parent2);

    Entity* parent_again = mgr->deserialize_entity(serialized);

    {
      REQUIRE_WARNING_SCOPED(SER_0002);
      mgr->process_changes();
    }

    TransformComponent& parent_again_tr = parent_again->get_component<TransformComponent>();
    REQUIRE_EQ(parent_again_tr.get_children(), {});

    // Require that the actual parent stayed
    REQUIRE_EQ(parent2_tr.get_children(), { child });
    REQUIRE_EQ(child_tr.get_parent(), parent2);

    delete serialized;
    parent_again->remove();
    child->remove();
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Parent/Child: [De]serialize child, remove parent inbetween") {

    Entity* parent = mgr->new_entity();
    TransformComponent& parent_tr = parent->new_component<TransformComponent>();

    Entity* child = mgr->new_entity();
    TransformComponent& child_tr = child->new_component<TransformComponent>();

    child_tr.set_parent(parent);

    auto serialized = child->serialize();
    child->remove();
    parent->remove();
    mgr->process_changes();

    Entity* child_again = mgr->deserialize_entity(serialized);
    {
      REQUIRE_WARNING_SCOPED2(EM_0005, SER_0007);
      mgr->process_changes();
    }

    REQUIRE_EQ(child_again->get_component<TransformComponent>().get_children(), {});
    REQUIRE_EQ(child_again->get_component<TransformComponent>().get_parent(), nullptr);

    child_again->remove();
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Parent/Child: [De]serialize parent, remove child inbetween") {

    Entity* parent = mgr->new_entity();
    TransformComponent& parent_tr = parent->new_component<TransformComponent>();

    Entity* child = mgr->new_entity();
    TransformComponent& child_tr = child->new_component<TransformComponent>();

    child_tr.set_parent(parent);

    auto serialized = parent->serialize();
    child->remove();
    parent->remove();
    mgr->process_changes();

    Entity* parent_again = mgr->deserialize_entity(serialized);
    {
      REQUIRE_WARNING_SCOPED2(EM_0005, SER_0008);
      mgr->process_changes();
    }

    REQUIRE_EQ(parent_again->get_component<TransformComponent>().get_children(), {});
    REQUIRE_EQ(parent_again->get_component<TransformComponent>().get_parent(), nullptr);

    parent_again->remove();
  }
  END_TESTCASE;

  auto generate_bad_entity = [&](EntityManager* mgr, bool invalid_size = false) {

    Entity* entity = mgr->new_entity();
    const UUID& uuid = entity->get_uuid();

    // Hack to get access
    UUID& nonconst_uuid = const_cast<UUID&>(uuid);

    if (invalid_size) {
      nonconst_uuid = UUID::generate_faulty_for_testcases("ABC");
    } else {
      nonconst_uuid = UUID::generate_faulty_for_testcases("ABCABCABCAB^");
    }

    return entity;
  };

  BEGIN_TESTCASE("Provoke warning EM_0002 - Invalid entity UUID") {

    Entity* entity = generate_bad_entity(mgr, false);

    auto serialized = entity->serialize();
    entity->remove();
    mgr->process_changes();

    {
      REQUIRE_WARNING_SCOPED(EM_0002);
      REQUIRE_EQ(mgr->deserialize_entity(serialized), nullptr);
    }
  }
  END_TESTCASE;
}