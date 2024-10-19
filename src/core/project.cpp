#include <algorithm>

#include "project.h"

#include "nodes/output.h"

namespace core {

  void Project::initialize() {
    /* 
     * I intend to reimplement this for a no gui mode later on...
    auto outputNode = std::make_shared<nodes::Output>();
    m_outputNode = std::weak_ptr<nodes::Output>(outputNode);
    m_nodes.push_back(outputNode);
    */
  }

  Project::~Project() {
    std::unique_lock<std::mutex> lock(dataMutex);
    m_nodes.clear();
  }

  void Project::setOutputNode(std::shared_ptr<Node> node) {
    std::unique_lock<std::mutex> lock(dataMutex);
    m_outputNode = node;
  }

  void Project::addNode(std::shared_ptr<Node> node) {
    std::unique_lock<std::mutex> lock(dataMutex);
    m_nodes.push_back(node);
  }

  void Project::removeNode(std::shared_ptr<Node> node) {
    std::unique_lock<std::mutex> lock(dataMutex);

    if (m_outputNode.expired()) return; // something has gone horribly wrong but yeh

    if (node == m_outputNode.lock()) return;

    m_nodes.erase(
        std::remove_if(
          m_nodes.begin(), m_nodes.end(),
          [node](const auto& n) { return n == node; }),
        m_nodes.end());
  }

} // namespace core
