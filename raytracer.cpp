#include "raytracer.h"
#include <omp.h>
#include <random>
#include <time.h>

static float light = 2.0f;
static std::uniform_real_distribution<float> dist(0.0f, 1.0f);

RayTracer::RayTracer( Camera &camera,
                      const Scene &scene,
                      const glm::vec3 background_color,
                      Buffer &buffer ) :
        camera_( camera ),
        scene_( scene ),
        background_color_{ background_color },
        buffer_( buffer )
{}

void RayTracer::integrate()
{
    #pragma omp parallel for schedule(dynamic, 1)

    // Loops over image rows
    for ( std::size_t y = 0; y < buffer_.v_resolution_; y++ )
    {
        Seed seed(clock());
        std::stringstream progress_stream;
        progress_stream << "\r  progress .........................: "
                        << std::fixed << std::setw( 6 )
                        << std::setprecision( 2 )
                        << 100.0 * y / ( buffer_.v_resolution_ - 1 )
                        << "%";

        std::clog << progress_stream.str();

        // Loops over image columns
        for ( std::size_t x = 0; x < buffer_.h_resolution_; x++ )
        {
            buffer_.buffer_data_[x][y] = glm::vec3();

            Ray ray{ camera_.getWorldSpaceRay( glm::vec2{ x + .5f, y + .5f } ) };

            IntersectionRecord intersection_record;
            intersection_record.t_ = std::numeric_limits<double>::max();
            if (scene_.intersect( ray, intersection_record ) )
                buffer_.buffer_data_[x][y] +=  light * -glm::dot(intersection_record.normal_, camera_.direction_)
                * intersection_record.material_.color / (intersection_record.t_ >= 1.0f ? intersection_record.t_ : 1.0f);
        }
    }

    std::clog << std::endl;
}
