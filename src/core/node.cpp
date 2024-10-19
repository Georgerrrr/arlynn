#include <variant>
#include <mutex>

#include "core/project.h"

#include "node.h"

namespace core {

  void Node::setHasRendered(bool hs) {
    m_hasRendered = hs;
  }

  uint8_t Node::setInput(size_t inputIndex, std::shared_ptr<Node> node, size_t outputIndex) {
    std::scoped_lock<std::mutex> lock(Project::get().dataMutex);

    if (node != nullptr) {
      if (m_inputs.at(inputIndex).inputType != getOutputFormat(node->getOutput(outputIndex))) return 1;
    }

    m_inputs.at(inputIndex).node = node;
    m_inputs.at(inputIndex).index = outputIndex;
    return 0;
  }

  template<class... Ts>
  struct overloaded : Ts... { using Ts::operator()...; };

  void Node::renderBuffer() {
    if (m_hasRendered) return;
    for (auto& input : m_inputs) {
      if (input.node == nullptr) continue;
      input.node->renderBuffer();
    }

    for (auto& output : m_outputs) {
      std::visit(overloaded{
          [](AudioSignal& signal) { signal.render(signal.data, signal.data.size(), signal.userData); },
          [](AmplificationSignal& signal) { signal.render(signal.data, signal.data.size(), signal.userData); },
          [](FrequencySignal& signal) { signal.render(signal.data, signal.data.size(), signal.userData); }
          }, output);
      }
    m_hasRendered = true;
  }

  void Node::registerInput(NodeInput&& input) {
    m_inputs.push_back(input);
  }

  void Node::registerOutput(NodeOutput&& output) {
    m_outputs.push_back(output);
  }

  void Node::registerAttribute(attribute_t attribute) {
    m_attributes.push_back(attribute);
  }

} // namespace core
