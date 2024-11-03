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
      if (node->isNodeInInputChain(this)) return 2;
    }


    m_inputs.at(inputIndex).node = node;
    m_inputs.at(inputIndex).index = outputIndex;
    return 0;
  }

  bool Node::isNodeInInputChain(Node* node) {
    for (auto& input : m_inputs) {
      if (input.node == nullptr) continue;
      if (input.node.get() == node) return true;
      if (input.node->isNodeInInputChain(node)) return true;
    }
    return false;
  }

  void Node::setAttribute(size_t attributeIndex, attribute_t attribute) {
    std::scoped_lock<std::mutex> lock(Project::get().dataMutex);
    m_attributes.at(attributeIndex) = attribute;
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

  const size_t Node::registerInput(NodeInput&& input) {
    m_inputs.push_back(input);
    return m_inputs.size()-1;
  }

  const size_t Node::registerOutput(NodeOutput&& output) {
    m_outputs.push_back(output);
    return m_outputs.size()-1;
  }

} // namespace core
