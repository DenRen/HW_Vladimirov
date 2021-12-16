#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "cppl.hpp"

namespace clbrod {

class ClAccelerator {
protected:
    cl::Device device_;
    cl::Context context_;
    cl::CommandQueue cmd_queue_;
    cl::Program program_;

public:
    ClAccelerator (const cl::Device& device,
                   const std::string& source,
                   cl::QueueProperties cmd_queue_prop = cl::QueueProperties::None);
};

template <typename KernelFunctorT>
class FractalWrapper {
    std::string name_;
    KernelFunctorT kernel_;

public:

    template <typename... KernelArgTs>
    FractalWrapper (std::string name, KernelArgTs&&... kernelArgs) :
        name_ (name),
        kernel_ (std::forward <KernelArgTs> (kernelArgs)...)
    {}

    template <typename... KernelArgTs>
    auto operator () (KernelArgTs&&... kernelArgs) {
        return kernel_ (std::forward <KernelArgTs> (kernelArgs)...);
    }

    const std::string& getName () const {
        return name_;
    }
}; // tt <KernelFunctorT> class FractalWrapper

class FractalDrawer : protected ClAccelerator {
    using ClAccelerator::device_;
    using ClAccelerator::context_;
    using ClAccelerator::cmd_queue_;
    using ClAccelerator::program_;

    using kernelFunctor_t = cl::KernelFunctor <cl::Buffer,      // Global buf vertexs
                                               float,           // Scale factor
                                               float,           // Shift x
                                               float,           // Shift y
                                               sf::Vector2f,    // Complex number C
                                               int>;            // Count iterations

    std::vector <FractalWrapper <kernelFunctor_t>> kernels;

    std::vector <sf::Vertex> verts_;
    sf::Vector2u size_;

    unsigned fractalNumber_ = 0;

    void processFractalNumber ();
public:

    FractalDrawer (const cl::Device& device,
                   sf::Vector2u size);

    void draw (sf::RenderTarget& target,
               const sf::Vector3f& pos,
               const sf::Vector2f& C,
               int count_iters = 1000);

    const std::string& getFractalName () const;

    void nextFractal ();
    void previousFractal ();
    void setFractalNumber (unsigned fractalNumber);
};

enum class QUALITY : unsigned {
    HD = 720,
    FHD = 1080
};

int drawFractal (QUALITY quality, int windowStyle = sf::Style::Fullscreen);

} // namespace clbrod