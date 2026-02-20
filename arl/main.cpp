#include <pybind11/pybind11.h>

namespace py = pybind11;

#include "audioengine.h"

struct _new_node {
  Node::node_init init;
  Node::node_close close;
};

namespace output {
  void init(Node&, uint32_t, uint32_t);
  void close();
}

namespace oscillator {
  void init(Node&, uint32_t, uint32_t);
  void close();
}

const _new_node nodes[] = {
  {output::init, output::close},
  {oscillator::init, oscillator::close}
};

PYBIND11_MODULE(arl, m, py::mod_gil_not_used())
{
  m.def("Init", audio::Init, "Initialise the audio engine");
  m.def("Close", audio::Close, "Close the audio engine");
  m.def("RemoveNode", &audio::RemoveNode, "Remove node");

  m.def("AddNode",
      [](int16_t key) {
      return audio::AddNode(nodes[key].init, nodes[key].close);
      }, "Add new node");

  m.def("SetConnection",
      [](int16_t na, int16_t ia, int16_t nb, int16_t ib) {
      audio::SendMessage(_set_connection{na, ia, nb, ib});
      }, "Set the connection of input [node a, index a] to output [node b, index b]");
}
