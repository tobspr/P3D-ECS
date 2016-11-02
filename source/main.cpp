
#include "main.h"

#include "entity_manager.h"
#include "entity_system.h"
#include "all_components.h"
#include "entity.h"

#include "movement_system.h"

#include <iostream>
#include <unordered_set>
#include <functional>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;

#define TC_STATUS(content) cout << "\n\nTC> " << content << endl;
#define TC_EXPECT(value, expected) testsuite_expect(value, expected, " " #value " == " #expected "", __FILE__, __LINE__);


void write_tc_log(const std::string& msg) {
    ofstream outfile("test-output.txt", ios_base::app);
    outfile << msg;
}

template < typename T >
void testsuite_expect(T value, T expected, const std::string& comparison, const char* file, size_t line)
{
	if (value != expected) {
        stringstream s;
        s << "\n\n========================================================================" << endl;
		s << "\n\nTESTCASE FAIL!" << endl;
        s << "In file " << file << " Line " << line << endl; 
		s << "Expected: " << expected << endl;
		s << "Actual: " << value << endl;
		s << "Expression: " << comparison << endl;
        s << "\n\n========================================================================" << endl;
		s << "\n\n\n\n\n";
        write_tc_log(s.str());
		cerr << s.str();
	} else {
        cout << "Expression succeeded: " << comparison << " (value: " << value << ")" << endl;
    }
}

template < typename A, typename B >
void testsuite_expect(A value, B expected, const std::string& comparison, const char* file, size_t line)
{
    return testsuite_expect(value, (A)expected, comparison, file, line);
}

void general_testsuite(const std::string& name, std::function<void(EntityManager*)> inner)
{
    write_tc_log("Testcase: " + name + "\n");
    ECS_RESET_LEAKS();
    cout << "\n\n\n=========== Test " << name << " ===========" << endl;

    TC_STATUS("Constructing new entity manager");
    EntityManager* mgr = new EntityManager();

    TC_STATUS("Running testsuite");
    inner(mgr);

    TC_STATUS("Testsuite done, cleaning up ..");
    mgr->print_status();
    mgr->shutdown();

    TC_EXPECT(mgr->get_num_entities(), 0);
    ECS_PRINT_LEAKS();

    write_tc_log("Done.\n");
}


void testcase_1()
{
    general_testsuite("Testcase 1", [](EntityManager* mgr) {

        cout << "\n\nTC> Constructing new entity" << endl;
        Entity* entity = mgr->new_entity();

        cout << "\n\nTC> Adding transform component" << endl;
        entity->add<TransformComponent>(LVecBase3f(1, 2, 3), LVecBase3f(2, 3, 4),
            LVecBase3f(5, 6, 7));

        cout << "\n\nTC> Modifying transform component" << endl;
        entity->get<TransformComponent>().set_pos(LVecBase3f(5, 5, 6));

        cout << "\n\nTC> Constructing another entity" << endl;
        Entity* entity2 = mgr->new_entity();
        entity2->add<TransformComponent>();
        entity2->add<PhysicsComponent>();

        cout << "\n\nTC> Constructing new movement system" << endl;
        MovementSystem* sys = mgr->new_system<MovementSystem>();

        auto update = [&]() {
            cout << "\n\nTC> Single step .." << endl;
            float dt = 0.05;
            mgr->single_step(dt);

            cout << "\n\nTC> Update system.. " << endl;
            sys->process(dt);
        };

        update();

        cout << "\n\nTC> Modifying first entry so it also has a pyhsics component"
             << endl;
        entity->add<PhysicsComponent>(0.0, LVecBase2f());

        update();

        cout << "\n\nTC> Constructing new entity just to delete it " << endl;
        Entity* entity3 = mgr->new_entity();
        entity3->remove();

        update();

        cout << "\n\nTC> Constructing new entity to delete it afterwards" << endl;
        Entity* entity4 = mgr->new_entity();
        entity4->add<TransformComponent>();
        entity4->add<PhysicsComponent>();

        update();
        entity4->remove();
        update();

        cout << "\n\nTC> Constructing new entity just before shutdown" << endl;
        Entity* entity5 = mgr->new_entity();

        cout << "\n\nTC> Deleting system" << endl;
        delete sys;

    });
};

void testcase_parent_child()
{

    enum class Operation {
        ConstructRoot,
        ConstructChild,
        ReparentChild,
        Update,
        DeleteRoot,
        DeleteChild
    };

    // see unittest/generate_tc1_code.py
    std::vector<std::vector<Operation> > testcases = {
        { Operation::ConstructRoot, Operation::Update, Operation::ConstructChild, Operation::Update, Operation::ReparentChild, Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 1
        { Operation::ConstructRoot, Operation::ConstructChild, Operation::Update, Operation::ReparentChild, Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 2
        { Operation::ConstructRoot, Operation::Update, Operation::ConstructChild, Operation::ReparentChild, Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 3
        { Operation::ConstructRoot, Operation::ConstructChild, Operation::ReparentChild, Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 4
        { Operation::ConstructChild, Operation::Update, Operation::ConstructRoot, Operation::Update, Operation::ReparentChild, Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 5
        { Operation::ConstructChild, Operation::ConstructRoot, Operation::Update, Operation::ReparentChild, Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 6
        { Operation::ConstructChild, Operation::Update, Operation::ConstructRoot, Operation::ReparentChild, Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 7
        { Operation::ConstructChild, Operation::ConstructRoot, Operation::ReparentChild, Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 8
        { Operation::ConstructRoot, Operation::Update, Operation::ConstructChild, Operation::Update, Operation::ReparentChild, Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 9
        { Operation::ConstructRoot, Operation::ConstructChild, Operation::Update, Operation::ReparentChild, Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 10
        { Operation::ConstructRoot, Operation::Update, Operation::ConstructChild, Operation::ReparentChild, Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 11
        { Operation::ConstructRoot, Operation::ConstructChild, Operation::ReparentChild, Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 12
        { Operation::ConstructChild, Operation::Update, Operation::ConstructRoot, Operation::Update, Operation::ReparentChild, Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 13
        { Operation::ConstructChild, Operation::ConstructRoot, Operation::Update, Operation::ReparentChild, Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 14
        { Operation::ConstructChild, Operation::Update, Operation::ConstructRoot, Operation::ReparentChild, Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 15
        { Operation::ConstructChild, Operation::ConstructRoot, Operation::ReparentChild, Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 16
        { Operation::ConstructRoot, Operation::Update, Operation::ConstructChild, Operation::Update, Operation::ReparentChild, Operation::Update, Operation::DeleteChild, Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 17
        { Operation::ConstructRoot, Operation::ConstructChild, Operation::Update, Operation::ReparentChild, Operation::Update, Operation::DeleteChild, Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 18
        { Operation::ConstructRoot, Operation::Update, Operation::ConstructChild, Operation::ReparentChild, Operation::Update, Operation::DeleteChild, Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 19
        { Operation::ConstructRoot, Operation::ConstructChild, Operation::ReparentChild, Operation::Update, Operation::DeleteChild, Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 20
        { Operation::ConstructChild, Operation::Update, Operation::ConstructRoot, Operation::Update, Operation::ReparentChild, Operation::Update, Operation::DeleteChild, Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 21
        { Operation::ConstructChild, Operation::ConstructRoot, Operation::Update, Operation::ReparentChild, Operation::Update, Operation::DeleteChild, Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 22
        { Operation::ConstructChild, Operation::Update, Operation::ConstructRoot, Operation::ReparentChild, Operation::Update, Operation::DeleteChild, Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 23
        { Operation::ConstructChild, Operation::ConstructRoot, Operation::ReparentChild, Operation::Update, Operation::DeleteChild, Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 24
        { Operation::ConstructRoot, Operation::Update, Operation::ConstructChild, Operation::Update, Operation::ReparentChild, Operation::Update, Operation::DeleteChild, Operation::DeleteRoot, Operation::Update }, // Testcase 25
        { Operation::ConstructRoot, Operation::ConstructChild, Operation::Update, Operation::ReparentChild, Operation::Update, Operation::DeleteChild, Operation::DeleteRoot, Operation::Update }, // Testcase 26
        { Operation::ConstructRoot, Operation::Update, Operation::ConstructChild, Operation::ReparentChild, Operation::Update, Operation::DeleteChild, Operation::DeleteRoot, Operation::Update }, // Testcase 27
        { Operation::ConstructRoot, Operation::ConstructChild, Operation::ReparentChild, Operation::Update, Operation::DeleteChild, Operation::DeleteRoot, Operation::Update }, // Testcase 28
        { Operation::ConstructChild, Operation::Update, Operation::ConstructRoot, Operation::Update, Operation::ReparentChild, Operation::Update, Operation::DeleteChild, Operation::DeleteRoot, Operation::Update }, // Testcase 29
        { Operation::ConstructChild, Operation::ConstructRoot, Operation::Update, Operation::ReparentChild, Operation::Update, Operation::DeleteChild, Operation::DeleteRoot, Operation::Update }, // Testcase 30
        { Operation::ConstructChild, Operation::Update, Operation::ConstructRoot, Operation::ReparentChild, Operation::Update, Operation::DeleteChild, Operation::DeleteRoot, Operation::Update }, // Testcase 31
        { Operation::ConstructChild, Operation::ConstructRoot, Operation::ReparentChild, Operation::Update, Operation::DeleteChild, Operation::DeleteRoot, Operation::Update }, // Testcase 32
        { Operation::ConstructRoot, Operation::Update, Operation::ConstructChild, Operation::Update, Operation::ReparentChild, Operation::DeleteRoot, Operation::Update }, // Testcase 33
        { Operation::ConstructRoot, Operation::ConstructChild, Operation::Update, Operation::ReparentChild, Operation::DeleteRoot, Operation::Update }, // Testcase 34
        { Operation::ConstructRoot, Operation::Update, Operation::ConstructChild, Operation::ReparentChild, Operation::DeleteRoot, Operation::Update }, // Testcase 35
        { Operation::ConstructRoot, Operation::ConstructChild, Operation::ReparentChild, Operation::DeleteRoot, Operation::Update }, // Testcase 36
        { Operation::ConstructChild, Operation::Update, Operation::ConstructRoot, Operation::Update, Operation::ReparentChild, Operation::DeleteRoot, Operation::Update }, // Testcase 37
        { Operation::ConstructChild, Operation::ConstructRoot, Operation::Update, Operation::ReparentChild, Operation::DeleteRoot, Operation::Update }, // Testcase 38
        { Operation::ConstructChild, Operation::Update, Operation::ConstructRoot, Operation::ReparentChild, Operation::DeleteRoot, Operation::Update }, // Testcase 39
        { Operation::ConstructChild, Operation::ConstructRoot, Operation::ReparentChild, Operation::DeleteRoot, Operation::Update }, // Testcase 40
        { Operation::ConstructRoot, Operation::Update, Operation::ConstructChild, Operation::Update, Operation::ReparentChild, Operation::DeleteRoot, Operation::Update }, // Testcase 41
        { Operation::ConstructRoot, Operation::ConstructChild, Operation::Update, Operation::ReparentChild, Operation::DeleteRoot, Operation::Update }, // Testcase 42
        { Operation::ConstructRoot, Operation::Update, Operation::ConstructChild, Operation::ReparentChild, Operation::DeleteRoot, Operation::Update }, // Testcase 43
        { Operation::ConstructRoot, Operation::ConstructChild, Operation::ReparentChild, Operation::DeleteRoot, Operation::Update }, // Testcase 44
        { Operation::ConstructChild, Operation::Update, Operation::ConstructRoot, Operation::Update, Operation::ReparentChild, Operation::DeleteRoot, Operation::Update }, // Testcase 45
        { Operation::ConstructChild, Operation::ConstructRoot, Operation::Update, Operation::ReparentChild, Operation::DeleteRoot, Operation::Update }, // Testcase 46
        { Operation::ConstructChild, Operation::Update, Operation::ConstructRoot, Operation::ReparentChild, Operation::DeleteRoot, Operation::Update }, // Testcase 47
        { Operation::ConstructChild, Operation::ConstructRoot, Operation::ReparentChild, Operation::DeleteRoot, Operation::Update }, // Testcase 48
        { Operation::ConstructRoot, Operation::Update, Operation::ConstructChild, Operation::Update, Operation::ReparentChild, Operation::DeleteChild, Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 49
        { Operation::ConstructRoot, Operation::ConstructChild, Operation::Update, Operation::ReparentChild, Operation::DeleteChild, Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 50
        { Operation::ConstructRoot, Operation::Update, Operation::ConstructChild, Operation::ReparentChild, Operation::DeleteChild, Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 51
        { Operation::ConstructRoot, Operation::ConstructChild, Operation::ReparentChild, Operation::DeleteChild, Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 52
        { Operation::ConstructChild, Operation::Update, Operation::ConstructRoot, Operation::Update, Operation::ReparentChild, Operation::DeleteChild, Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 53
        { Operation::ConstructChild, Operation::ConstructRoot, Operation::Update, Operation::ReparentChild, Operation::DeleteChild, Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 54
        { Operation::ConstructChild, Operation::Update, Operation::ConstructRoot, Operation::ReparentChild, Operation::DeleteChild, Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 55
        { Operation::ConstructChild, Operation::ConstructRoot, Operation::ReparentChild, Operation::DeleteChild, Operation::Update, Operation::DeleteRoot, Operation::Update }, // Testcase 56
        { Operation::ConstructRoot, Operation::Update, Operation::ConstructChild, Operation::Update, Operation::ReparentChild, Operation::DeleteChild, Operation::DeleteRoot, Operation::Update }, // Testcase 57
        { Operation::ConstructRoot, Operation::ConstructChild, Operation::Update, Operation::ReparentChild, Operation::DeleteChild, Operation::DeleteRoot, Operation::Update }, // Testcase 58
        { Operation::ConstructRoot, Operation::Update, Operation::ConstructChild, Operation::ReparentChild, Operation::DeleteChild, Operation::DeleteRoot, Operation::Update }, // Testcase 59
        { Operation::ConstructRoot, Operation::ConstructChild, Operation::ReparentChild, Operation::DeleteChild, Operation::DeleteRoot, Operation::Update }, // Testcase 60
        { Operation::ConstructChild, Operation::Update, Operation::ConstructRoot, Operation::Update, Operation::ReparentChild, Operation::DeleteChild, Operation::DeleteRoot, Operation::Update }, // Testcase 61
        { Operation::ConstructChild, Operation::ConstructRoot, Operation::Update, Operation::ReparentChild, Operation::DeleteChild, Operation::DeleteRoot, Operation::Update }, // Testcase 62
        { Operation::ConstructChild, Operation::Update, Operation::ConstructRoot, Operation::ReparentChild, Operation::DeleteChild, Operation::DeleteRoot, Operation::Update }, // Testcase 63
        { Operation::ConstructChild, Operation::ConstructRoot, Operation::ReparentChild, Operation::DeleteChild, Operation::DeleteRoot, Operation::Update }, // Testcase 64

    };

    size_t i = 0;
    for (const auto& order : testcases) {
        ++i;
        general_testsuite("Parent Child Relations - CASE " + std::to_string(i), [&](EntityManager* mgr) {

            auto make_entity = [&]() {
                Entity* tmp = mgr->new_entity();
                tmp->add<TransformComponent>();
                return tmp;
            };
            Entity* root = nullptr;
            Entity* child = nullptr;

            TC_STATUS("Start to execute " << order.size() << " operations in order.");
            for (Operation op : order) {
                switch (op) {
                case Operation::ConstructRoot:
                    root = make_entity();
                    break;
                case Operation::ConstructChild:
                    child = make_entity();
                    break;
                case Operation::ReparentChild:
                    child->get<TransformComponent>().set_parent(root);
                    break;
                case Operation::Update:
                    mgr->single_step(1.0);
                    break;
                case Operation::DeleteRoot:
                    root->remove();
                    break;
                case Operation::DeleteChild:
                    child->remove();
                    break;
                }
            }

        });
    }
};

void testcase_collectors()
{

    struct TestSystem : public SimpleEntitySystem<PhysicsComponent> {
        TestSystem(EntityManager* mgr)
            : SimpleEntitySystem<PhysicsComponent>(mgr){};
        virtual void process(float dt) override
        {
			processed_entities = 0;
            for (auto entity : get_entities()) {
                ++processed_entities;
            };
        }

        size_t processed_entities = 0;
    };

    general_testsuite("Collectors - Initial test", [](EntityManager* mgr) {

        TestSystem* sys = mgr->new_system<TestSystem>();

        Entity* entity = mgr->new_entity();
        entity->add<TransformComponent>();
        entity->add<PhysicsComponent>();

        mgr->single_step(1.0);
        sys->process(1.0);

		TC_EXPECT(sys->get_entities().size(), 1u);

		entity->remove();

		mgr->single_step(1.0);

		TC_EXPECT(sys->get_entities().size(), 0u);

        delete sys;
    });

    general_testsuite("Collectors - Add component after creation", [](EntityManager* mgr) {

        TestSystem* sys = mgr->new_system<TestSystem>();

        Entity* entity = mgr->new_entity();
        entity->add<TransformComponent>();
    
        mgr->single_step(1.0);
        sys->process(1.0);

		TC_EXPECT(sys->get_entities().size(), 0u);

        entity->add<PhysicsComponent>();
        mgr->single_step(1.0);

		TC_EXPECT(sys->get_entities().size(), 1u);

        mgr->single_step(1.0);

		entity->remove();

		mgr->single_step(1.0);

		TC_EXPECT(sys->get_entities().size(), 0u);

        delete sys;
    });


    general_testsuite("Collectors - Add component and remove entity", [](EntityManager* mgr) {

        TestSystem* sys = mgr->new_system<TestSystem>();

        Entity* entity = mgr->new_entity();

		TC_EXPECT(sys->get_entities().size(), 0u);
        mgr->single_step(1.0);

        entity->add<TransformComponent>();
    
        sys->process(1.0);

        TC_EXPECT(sys->processed_entities, 0);

        entity->remove();

        mgr->print_status();
		mgr->single_step(0.0);

        TC_EXPECT(sys->get_entities().size(), 0u);

        delete sys;    
    });


}