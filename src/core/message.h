#pragma once 

#include <cstdint>
#include <variant>

namespace core {

  struct _remove_node {
    int16_t nodeId;
  };

  /*
   * | NodeB |                | NodeA |
   * |     - | <- IndexB      | -     |
   * |     - |      IndexA -> | -     |
   *
   */
  struct _set_connection {
    int16_t NodeA;
    int16_t IndexA;
    int16_t NodeB;
    int16_t IndexB;
  };

  typedef std::variant<_remove_node, _set_connection> message_t;

} // namespace core
