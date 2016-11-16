

#include "testsuite_perftest.h"

#include "unittest.h"
#include "entity_system.h"
#include "transform_component.h"
#include "physics_component.h"

#include <iomanip>

// Scales all iterations to make the perfest last longer, ideal for profiling
#ifdef PROFILING
#define MULTIPLIER 1
#else
#define MULTIPLIER 0.25f

// Required to color the warning below
#include "termcolor.h"
#endif

using namespace p3d_ecs;

// Simple system which modifies transform components
struct TestSystem_1 : public SimpleEntitySystem<TransformComponent> {
  TestSystem_1(EntityManager* mgr)
    : SimpleEntitySystem<TransformComponent>(mgr){};
  virtual void process(float dt) override {
    for (auto entity : get_entities()) {
      entity->get_component<TransformComponent>().set_pos({ 1, 2, 3 });
    };
  }
};

void
testsuite_perftest() {

#ifndef PROFILING
  std::cout
    << std::endl
    << std::endl
    << termcolor::red
    << "WARNING:: Not compiled with the PROFILING flag (see config_module.h). Performance will "
    << "be worse than it could be, about 5 times or even more!" << termcolor::reset << std::endl
    << std::endl;
#endif

  BEGIN_TESTCASE("Testing perfutil performance") {
    size_t iterations = 100000 * MULTIPLIER;
    std::vector<Entity*> entities;
    entities.reserve(iterations);

    BEGIN_MEASURE("Create entities", iterations) {
      for (size_t i = 0; i < iterations; ++i) {
        Entity* entity = mgr->new_entity();
        entities.push_back(entity);
      }
    }
    END_MEASURE;

    std::vector<Entity*> search_order(entities.begin(), entities.end());
    std::random_shuffle(search_order.begin(), search_order.end());
    size_t multiplier = 1;

    for (size_t k = 0; k < 2; ++k) {
      BEGIN_MEASURE("performance::vector_contains", iterations * multiplier) {
        for (size_t m = 0; m < multiplier; ++m) {
          size_t stuff = 0; // just to prevent getting optimized out
          for (Entity* to_search : search_order) {
            if (::p3d_ecs::performance::vector_contains(entities, to_search)) {
              stuff++;
            }
          }
          volatile size_t _prevent_optimized_out = stuff;
          REQUIRE_EQ(stuff, entities.size());
        }
      }
      END_MEASURE;

      BEGIN_MEASURE("manual loop", iterations * multiplier) {

        for (size_t m = 0; m < multiplier; ++m) {
          size_t stuff = 0; // just to prevent getting optimized out
          for (Entity* to_search : search_order) {
            for (Entity* entity : entities) {
              if (entity == to_search) {
                stuff++;
                break;
              }
            }
          }

          volatile size_t _prevent_optimized_out = stuff;
          REQUIRE_EQ(stuff, entities.size());
        }
      }
      END_MEASURE;
    }
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Memory usage") {

    std::cout << "An empty entity manager takes: "
              << (size_t)(mgr->get_memory_usage_bytes() / 1024.0) << " KB." << std::endl;
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Startup time") {

    size_t iterations = 100 * MULTIPLIER;
    std::vector<EntityManager*> mgrs;
    mgrs.reserve(iterations);

    BEGIN_MEASURE("Create entity mgr", iterations) {
      for (size_t i = 0; i < iterations; ++i) {
        EntityManager* mgr = new EntityManager();
        mgrs.push_back(mgr);
      }
    }
    END_MEASURE;

    BEGIN_MEASURE("Destruct entity mgr", iterations) {

      for (EntityManager* ptr : mgrs) {
        delete ptr;
      }
    }
    END_MEASURE;
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Simple entity creation") {

    size_t iterations = 7000000 * MULTIPLIER;
    BEGIN_MEASURE("Create entities", iterations) {
      for (size_t i = 0; i < iterations; ++i) {
        Entity* entity = mgr->new_entity();
      }
    }
    END_MEASURE;

    BEGIN_MEASURE("First manager update", 1) { mgr->process_changes(); }
    END_MEASURE;

    BEGIN_MEASURE("Manager cleanup", 1) { mgr->reset(); }
    END_MEASURE;
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Entity creation with component") {

    size_t iterations = 3000000 * MULTIPLIER;
    BEGIN_MEASURE("Create entities", iterations) {
      for (size_t i = 0; i < iterations; ++i) {
        Entity* entity = mgr->new_entity();
        TransformComponent& transform = entity->new_component<TransformComponent>();
      }
    }
    END_MEASURE;

    BEGIN_MEASURE("First manager update", 1) { mgr->process_changes(); }
    END_MEASURE;

    mgr->reset();
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Entity::get_component benchmark") {
    Entity* entity = mgr->new_entity();
    entity->new_component<TransformComponent>();

    size_t iterations = 20000000 * MULTIPLIER;
    BEGIN_MEASURE("Modify transform", iterations) {
      for (size_t i = 0; i < iterations; ++i) {
        entity->get_component<TransformComponent>().set_pos({ 1, 2, 3 });
      }
    }
    END_MEASURE;

    BEGIN_MEASURE("Access transform", iterations) {
      float x = 0.0;
      for (size_t i = 0; i < iterations; ++i) {
        x += entity->get_component<TransformComponent>().get_pos().get_x();
      }
      if (x == 624345345.30) {
        std::cout << "_";
      } // Avoid getting optimized out
    }
    END_MEASURE;

    entity->remove();
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Simple entity system benchmark") {

    TestSystem_1* sys = mgr->new_system<TestSystem_1>();

    size_t iterations = 3000000 * MULTIPLIER;
    BEGIN_MEASURE("Init entities", iterations) {
      for (size_t i = 0; i < iterations; ++i) {
        Entity* entity = mgr->new_entity();
        TransformComponent& transform = entity->new_component<TransformComponent>();
      }
    }
    END_MEASURE;

    BEGIN_MEASURE("Mgr::process_changes", iterations) { mgr->process_changes(); }
    END_MEASURE;

    BEGIN_MEASURE("System::process", iterations) { sys->process(1.0); }
    END_MEASURE;

    delete sys;
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Testing UUID performance") {

    size_t iterations = 25000000 * MULTIPLIER;
    BEGIN_MEASURE("Generate + Test uuids", iterations) {
      for (size_t i = 0; i < iterations; ++i) {
        UUID uuid = UUID::generate();
        REQUIRE_TRUE(UUID::is_valid_uuid(uuid.c_str()));
      }
    }
    END_MEASURE;
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Testing UUID for collisions") {

    size_t iterations = 5000000 * MULTIPLIER;

    BEGIN_MEASURE("Testing uuids", iterations) {
      std::unordered_set<UUID> uuids;
      uuids.reserve(iterations);
      bool result = false;
      for (size_t i = 0; i < iterations; ++i) {
        UUID uuid = UUID::generate();
        auto result = uuids.emplace(std::move(uuid));
        REQUIRE_TRUE(result.second);

        if (!result.second) {
          TC_STATUS("COLLISION!");
          TC_STATUS("After " << i << " iterations, the first collision occured. Stopping loop."
                             << endl
                             << endl
                             << endl);
          TC_STATUS("Collided element was: " << result.first->c_str());
          break; // Avoid spam
        }
      }
    }
    END_MEASURE;
  }
  END_TESTCASE;

  BEGIN_TESTCASE("Entity::find_by_uuid") {

    size_t iterations = 1000000 * MULTIPLIER;
    std::vector<Entity*> entities;
    entities.reserve(iterations);
    BEGIN_MEASURE("Create entities", iterations) {
      for (size_t i = 0; i < iterations; ++i) {
        Entity* entity = mgr->new_entity();
        entities.push_back(entity);
      }
    }
    END_MEASURE;

    BEGIN_MEASURE("Find by uuid", iterations) {
      for (Entity* entity : entities) {
        if (entity != mgr->find_entity(entity->get_uuid())) {
          std::cout << "FAILED TO FIND ENTITY"; // prevent to get optimized out
        }
      }
    }
    END_MEASURE;

    BEGIN_MEASURE("Shuffle vec", iterations) {
      std::random_shuffle(entities.begin(), entities.end());
    }
    END_MEASURE;

    BEGIN_MEASURE("Find by uuid [shuf]", iterations) {
      for (Entity* entity : entities) {
        if (entity != mgr->find_entity(entity->get_uuid())) {
          std::cout << "FAILED TO FIND ENTITY"; // prevent to get optimized out
        }
      }
    }
    END_MEASURE;
  }
  END_TESTCASE;

  BEGIN_TESTCASE("[De]serializing entities") {

    size_t iterations = 100000 * MULTIPLIER;
    std::vector<Entity*> entities;
    std::vector<std::string> serialized;

    entities.reserve(iterations);
    serialized.reserve(iterations);

    BEGIN_MEASURE("Create entities", iterations) {
      for (size_t i = 0; i < iterations; ++i) {
        Entity* entity = mgr->new_entity();
        auto& transform = entity->new_component<TransformComponent>();
        auto& physics = entity->new_component<PhysicsComponent>();

        transform.set_pos({ (float)i, 2, 3 });
        transform.set_scale({ ((float)i) * 0.005f, 1, 3 });
        transform.set_hpr({ 22, 96, (float)(i % 360) });
        physics.set_mass(i * 0.34f);
        physics.set_is_static(i % 2 == 0);
        entities.push_back(entity);
      }
    }
    END_MEASURE;

    BEGIN_MEASURE("Process changes", iterations) { mgr->process_changes(); }
    END_MEASURE;

    BEGIN_MEASURE("Serialize entities", iterations) {
      for (Entity* entity : entities) {
        auto* message = entity->serialize();
        std::string str;
        REQUIRE_TRUE(message->SerializeToString(&str));
        serialized.push_back(str);
      }
    }
    END_MEASURE;

    BEGIN_MEASURE("Delete entities", iterations) {
      for (Entity* entity : entities) {
        entity->remove();
      }
    }
    END_MEASURE;

    BEGIN_MEASURE("Process changes", iterations) { mgr->process_changes(); }
    END_MEASURE;

    entities.clear();
    BEGIN_MEASURE("Deseraialize entities", iterations) {
      for (const std::string& str : serialized) {
        ::p3d_ecs::proto::Entity* message = new ::p3d_ecs::proto::Entity;
        REQUIRE_TRUE(message->ParseFromString(str));
        Entity* ptr = mgr->deserialize_entity(message);
        REQUIRE_PTR(ptr);
        entities.push_back(ptr);
      }
    }
    END_MEASURE;

    BEGIN_MEASURE("Delete entities [bulk]", iterations) { mgr->reset(); }
    END_MEASURE;
  }
  END_TESTCASE;
}
