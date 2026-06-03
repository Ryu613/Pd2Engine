#include "catch2/catch_test_macros.hpp"

#include "pd/core/entity_manager.hpp"

struct Parent {
  pd::Entity parent;
};

TEST_CASE("test_ecs", "ecs") {
  auto entityManager = std::make_unique<pd::EntityManager>();

  pd::Entity e = entityManager->createEntity();
  entityManager->addComponent<Parent>(e);
}