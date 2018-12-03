#pragma once

#include "primitives/operation.h"
#include "world.h"
#include <exception>
#include <optional>
#include <string>
#include <unordered_set>

struct unit_carrier_t; // Forward

std::string to_string(EResourceType type) noexcept;

class ex_warehouse_unknown_resource : public std::runtime_error {
public:
  explicit ex_warehouse_unknown_resource(EResourceType type)
      : std::runtime_error("Unknown resource type {" + to_string(type) + "}") {}
};

class ex_warehouse_resource_full : public std::runtime_error {
public:
  explicit ex_warehouse_resource_full(EResourceType type)
      : std::runtime_error("Storage for resource type {" + to_string(type) + "} is full") {}
};

class module_warehouse_t {
public:
  using module_id_t = size_t;

  const module_id_t id;
  const World::position_t position;

  explicit module_warehouse_t(module_id_t id, World::position_t position) noexcept;

  operation_t unload(unit_carrier_t& miner);

  std::optional<size_t> get_resource_amount(EResourceType type) const noexcept;

private:
  struct resource_storage_t {
    size_t current{0u};
    const size_t capacity{0u};
  };

  using storage_t = std::unordered_map<EResourceType, resource_storage_t>;
  storage_t _storage;
};
