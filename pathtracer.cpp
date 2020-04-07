#include "pathtracer.h"
#include <omp.h>
#include <random>
#include <time.h>

static std::uniform_real_distribution<float> dist(0.0f, 1.0f);

PathTracer::PathTracer( Camera &camera,
                      const Scene &scene,
                      const glm::vec3 background_color,
                      Buffer &buffer ) :
        camera_( camera ),
        scene_( scene ),
        background_color_{ background_color },
        buffer_( buffer )
{}

void PathTracer::integrate()
{
    integrate(4);
}

void PathTracer::integrate( std::size_t samples )
{
    const int wh = std::sqrt(samples);//width and height of the subpixel

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
            
            for(std::size_t s = 0; s < samples; s++)
            {
                float rand = dist(seed);//random betewn [0,1]
                float sx = s / wh;
                float sy = s % wh;
                float qx = (sx + rand) / float(wh);
                float qy = (sy + rand) / float(wh);

                Ray ray{ camera_.getWorldSpaceRay( glm::vec2{ x + qx, y + qy } ) };

               buffer_.buffer_data_[x][y] += radiance(ray, 0, seed);
            }
            buffer_.buffer_data_[x][y] /= samples;
        }
    }

    std::clog << std::endl;
}

static glm::vec3 vecrand(const glm::vec3& normal, Seed& seed);

static glm::vec3 vecspe(const glm::vec3& norm, const glm::vec3& vec);

static glm::vec3 vecrefrated(const glm::vec3& normal, const glm::vec3& vec, float ni, float nt);

static float fresnel(float ni, float nt, float costheta);

glm::vec3 PathTracer::radiance(const Ray& ray, int depth, Seed& seed)
{
    Ray newRay;
    IntersectionRecord ir;
    glm::vec3 rad = glm::vec3();
    glm::vec3 em, col;
    float dot;
    //dieletric
    float cosi, cos2_th, costheta;
    float ni, nt;

    ir.t_ = std::numeric_limits<double>::max();//reset t

    if(depth < 7 && scene_.intersect(ray, ir))
    {
        newRay.origin_ = ir.position_;
        em = ir.material_.emission;
        col = ir.material_.color;

        switch(ir.material_.type)
        {
            case MaterialType::DIFFUSE:
            newRay.direction_ = vecrand(ir.normal_, seed);
            dot = glm::dot(newRay.direction_, ir.normal_);
            rad = em + 2.0f * col * radiance(newRay, ++depth, seed) * dot;
            break;

            case MaterialType::SPECULAR:
            newRay.direction_ = vecspe(ir.normal_, ray.direction_);
            rad = radiance(newRay, ++depth, seed);
            break;

            case MaterialType::DIELECTRIC:
            cosi = -glm::dot(ir.normal_, ray.direction_);

            if(cosi < 0.0f)
            {
                ni = 1.5f;
                nt = 1.0f;
                ir.normal_ = -ir.normal_;
            }
            else
            {
                ni = 1.0f;
                nt = 1.5f;
            }

            cos2_th = 1.0f - (ni / nt) * (ni / nt) * (1.0f - cosi * cosi);

            if(ni > nt)
            {
                costheta = sqrt(cos2_th);
            }
            else
            {
                costheta = cosi;
            }
            

            if(cos2_th < 0.0f)
                newRay.direction_ = vecspe(ir.normal_, ray.direction_);
            else if(dist(seed) < fresnel(ni, nt, costheta))
                newRay.direction_ = vecspe(ir.normal_, ray.direction_);
            else
                newRay.direction_ = vecrefrated(ray.direction_, ir.normal_, ni, nt);
            
            newRay.origin_ = ir.position_ + newRay.direction_ * 0.001f;

            rad = radiance(newRay, ++depth, seed);
            break;

            case MaterialType::LIGHT:
            rad = em;//refletance (0, 0, 0)
            break;
        }
    }

    return rad;
}

static glm::vec3 vecspe(const glm::vec3& norm, const glm::vec3& vec)
{
    return vec - glm::dot(norm, vec) * norm * 2.0f;
}

static glm::vec3 vecrand(const glm::vec3& normal, Seed& seed)
{
    const float pi = 3.1415926536f;
    //generate random number betwen [0, 1)
    float r0 = dist(seed);
    float r1 = dist(seed);
    while(r0 == 1.0f)
        r0 = dist(seed);
    while(r1 == 1.0f)
        r1 =dist(seed);

    float theta = std::acos(1.0f - r0);
    float phi = r1 * pi * 2.0f;
    glm::vec3 vrand;
    ONB onb;
    onb.setFromV(normal);

    vrand.x = std::sin(theta) * std::cos(phi);
    vrand.z = std::sin(theta) * std::sin(phi);
    vrand.y = std::cos(theta);
    vrand = glm::normalize(onb.getBasisMatrix() * vrand);

    return vrand;
}

static glm::vec3 vecrefrated(const glm::vec3& vec, const glm::vec3& normal, float ni, float nt)
{
    float cosi = -glm::dot(vec, normal);
    float n = ni / nt;
    float cost = sqrt(1.0f - n * n * (1.0f - cosi * cosi));

    return n * (vec + cosi * normal) - normal * cost;
}

static float fresnel(float ni, float nt, float costheta)
{
    float r0 = (ni - nt) / (ni + nt);
    r0 = r0 * r0;

    costheta = 1.0f - costheta;

    return r0  + (1.0f - r0) * costheta * costheta * costheta * costheta * costheta;
}