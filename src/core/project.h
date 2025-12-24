#pragma once 

#include <vector>
#include <memory>
#include <mutex>

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

    private:
    Project() {};
  };

} // namespace core
