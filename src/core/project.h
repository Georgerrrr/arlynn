#pragma once 

#include <vector>
#include <memory>
#include <mutex>

#include "node.h"

namespace core {

  class AudioEngine;

  /** 
   * The Project class holds the important details of a project.
   *
   * Project uses the singleton model. It stores all of the nodes and is interfaced with to add and remove any nodes
   */
  class Project {
    public:
    Project(const Project&) = delete;
    void operator=(const Project&) = delete;

    void initialize();
    ~Project();

    static Project& get() {
      static Project instance;
      return instance;
    }

    std::weak_ptr<Node> getOutputNode() { return m_outputNode; }
    const std::vector<std::shared_ptr<Node>>& getNodes() { return m_nodes; }
    /*
     * Set output node 
     */
    void setOutputNode(std::shared_ptr<Node> node);

    /* 
     * Add node to project 
     */
    void addNode(std::shared_ptr<Node> node);
    /* 
     * Remove node from project
     * @note if node is the same as m_outputNode, it will not be removed 
     */
    void removeNode(std::shared_ptr<Node> node);

    std::mutex dataMutex;

    friend class AudioEngine;

    private:
    Project() {};

    std::vector<std::shared_ptr<Node>> m_nodes;
    std::weak_ptr<Node> m_outputNode;
  };

} // namespace core
