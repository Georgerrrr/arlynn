#pragma once 

#include <memory>
#include <variant>
#include <vector>

#include "signal.h"

namespace core {

  typedef std::variant<int*, float*, double*> attribute_t;

  /**
   * Node is the base class for any node such as Oscillator or Amplifier 
   * 
   * Each node takes in input data and outputs more data. Nodes can generate their own
   * data or use their inputs to create new data. For instance, Oscillator will generate
   * an audio signal based on it's 2 inputs: frequency and modulation. Amplifier takes in
   * an audio signal and an amplification signal, it will then multiply the audio signal 
   * by the amplification signal at each frame in the buffer.
   */
  class Node {
    public:
    Node() = default; /**< Default constructor for Node */

    void setHasRendered(bool hs); /**< @param hs sets value of m_hasRendered */

    /** 
     * Set an input of the node
     * @param inputIndex the input on this Node which the connection is being made to
     * @param node the other node which is being connected to 
     * @param outputIndex the specific output of the other node being connected to
     * @return 
     *  - 0 if input has been successfully set.
     *  - 1 if connection is made between two different formats. 
     *  - 2 if node finds itself in it's own input chain.
     */
    uint8_t setInput(size_t inputIndex, std::shared_ptr<Node> node=nullptr, size_t outputIndex=0);

    const bool getHasRendered() const { return m_hasRendered; } /**< @return m_hasRendered */
    /**
     * @param the index of Node's inputs that is being returned
     * @return node input
     */
    const NodeInput& getInput(size_t index) const { return m_inputs.at(index); } 
    /**
     * @param the index of Node's output that is being returned
     * @return node output 
     */
    const NodeOutput& getOutput(size_t index) const { return m_outputs.at(index); }
    /**
     * @param the index of Node's attribute that is being returned
     * @return node attribute
     */
    attribute_t getAttribute(size_t index) { return m_attributes.at(index); }
    
    /**
     * Renders this nodes audio buffer 
     *
     * Checks if buffer has already been rendered, if not 
     * it calls Node::renderBuffer on it's inputs that are set.
     * It renders all of the nodes outputs.
     * Set's m_hasRendered to true
     */
    void renderBuffer();

    virtual constexpr std::string name() = 0; /**< The name of the node */
    virtual constexpr std::string author() = 0; /**< The author of the node */
    virtual constexpr std::string version() = 0; /**< The version of the node */

    protected:
    void registerInput(NodeInput&& input); /**< Register a new input (Only accepts R-values) */
    void registerOutput(NodeOutput&& output); /**< Register a new output (Only accepts R-values) */
    void registerAttribute(attribute_t attribute); /**< Register a new attribute */

    private:
    std::vector<NodeOutput> m_outputs;
    std::vector<NodeInput> m_inputs;
    std::vector<attribute_t> m_attributes;

    bool m_hasRendered;
  };

} // namespace core 