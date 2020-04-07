#include "main.h"
#include <time.h>
#include <chrono>
#include <sstream>

int main( int argc, char **argv )
{
    if(argc < 2)
    {
        std::cerr << "Error: invalid paramaters! Sampler number undefined.\nExample: ./swpathtracer 10000" << std::endl;
        return 1;
    }

    unsigned int x_resolution = 1024;
    unsigned int y_resolution = 720;
    std::stringstream input;
    unsigned int sampler = 30;

    input << argv[1];
    input >> sampler;

    PerspectiveCamera camera{ 3.141592f * 0.25f,//45 deg
                                glm::ivec2{ x_resolution, y_resolution }, 
                                glm::vec3{ 0.0f, 2.0f,  4.5f },     // position
                                glm::vec3{ 0.0f, -1.0f,  0.0f },     // up
                                glm::vec3{ 0.0f, 0.0f, -1.0f } };   // look at
    Scene scene{};
    
    scene.load();

    Buffer rendering_buffer{ x_resolution, y_resolution };
    glm::vec3 background_color{ 0.0f, 0.0f, 0.0f };

    // Set up the renderer.
    auto render = PathTracer( camera,
                             scene,
                             background_color,
                             rendering_buffer );

    auto initTime = std::chrono::high_resolution_clock::now();
    render.integrate(sampler); // Renders the final image.
    auto duration = std::chrono::high_resolution_clock::now() - initTime;
    time_t result = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    std::cout << result << " seconds" << std::endl;

    // Save the rendered image to a .ppm file.
    rendering_buffer.save( "output_image.ppm" );

    return EXIT_SUCCESS;
}

