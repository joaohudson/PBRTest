#ifndef _PATHTRACER_H_
#define _PATHTRACER_H_

#include <sstream>
#include <iomanip>
#include <cmath>
#include <random>

#include "camera.h"
#include "scene.h"
#include "buffer.h"

typedef std::mt19937 Seed;

class PathTracer
{
public:

    PathTracer( Camera &camera,
               const Scene &scene,
               const glm::vec3 background_color,
               Buffer &buffer );

    void integrate( void );

    void integrate( std::size_t samples );

private:

    glm::vec3 radiance(const Ray& ray, int depth, Seed& seed);

    const Camera &camera_;

    const Scene &scene_;

    glm::dvec3 background_color_;

    Buffer &buffer_;

};

#endif /* RAYTRACER_H_ */

