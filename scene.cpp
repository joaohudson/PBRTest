#include "scene.h"
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>

Scene::Scene( void )
{}

Scene::~Scene( void )
{}

bool Scene::intersect( const Ray &ray,
                       IntersectionRecord &intersection_record ) const
{
    bool intersection_result = false;
    IntersectionRecord tmp_intersection_record = intersection_record;
    std::size_t num_primitives = primitives_.size();

    // Loops over the list of primitives, testing the intersection of each primitive against the given ray 
    for ( std::size_t primitive_id = 0; primitive_id < num_primitives; primitive_id++ )
        if ( primitives_[primitive_id]->intersect( ray, tmp_intersection_record ) )
            if ( ( tmp_intersection_record.t_ < intersection_record.t_ ) && ( tmp_intersection_record.t_ > 0.0 ) )
            {
                intersection_record = tmp_intersection_record;
                intersection_result = true; // the ray intersects a primitive!
            }

    return intersection_result;
}

void Scene::load( void ) 
{
    const std::string fileName = "scpbr.obj";
    Material specularMaterial, wall0, wall1, wm, wd, sw, sm, base, glass;
    Material lowerLightMaterial, lightMaterial; 
    Model *model;

    base.color = glm::vec3(0.54f, 0.4f, 0.29f);

    glass.type = MaterialType::DIELECTRIC;

    wm.color = glm::vec3(0.6f, 0.6f, 0.6f);
    wm.type = MaterialType::DIFFUSE;
    wd.color = glm::vec3(0.9f, 0.9f, 0.9f);
    wd.type = MaterialType::SPECULAR;

    sw.color = glm::vec3(0.84f, 0.84f, 0.74f);
    sw.type = MaterialType::DIFFUSE;
    sm.type = MaterialType::SPECULAR;

    wall0.color = glm::vec3(0.5f, 0.5f, 0.8f);
    wall1.color = glm::vec3(0.9f, 0.9f, 0.9f);

    lightMaterial.emission = glm::vec3(12.0f, 12.0f, 12.0f);
    lightMaterial.type = MaterialType::LIGHT;

    lowerLightMaterial.emission = glm::vec3(4.0f, 4.0f, 4.0f);
    lowerLightMaterial.type = MaterialType::LIGHT;
    
    specularMaterial.type = MaterialType::SPECULAR;

    // primitives_.push_back( Primitive::PrimitiveUniquePtr( new Sphere{ glm::vec3{  2.0f, 1.f, -1.2f }, 0.5f, glass } ) );
    // primitives_.push_back( Primitive::PrimitiveUniquePtr( new Sphere{ glm::vec3{  -2.0f, 1.f, -1.2f }, 0.5f, glass } ) );
    primitives_.push_back( Primitive::PrimitiveUniquePtr( new Sphere{ glm::vec3{  1.2f, -0.37f, 0.9f }, 0.25f, glass } ) );
    primitives_.push_back( Primitive::PrimitiveUniquePtr( new Sphere{ glm::vec3{  0.0f, 1.f, -1.0f }, 0.5f, specularMaterial } ) );
    primitives_.push_back( Primitive::PrimitiveUniquePtr( new Sphere{ glm::vec3{  28.0f, 0.0f, -4.0f }, 18.0f, wall0 } ) );
    primitives_.push_back( Primitive::PrimitiveUniquePtr( new Sphere{ glm::vec3{  -28.0f, 0.0f, -4.0f }, 18.0f, wall0 } ) );
    primitives_.push_back( Primitive::PrimitiveUniquePtr( new Sphere{ glm::vec3{  0.0f, 22.0f, -4.0f }, 18.0f, wall1 } ) );
    primitives_.push_back( Primitive::PrimitiveUniquePtr( new Sphere{ glm::vec3{  0.0f, -22.0f, -4.0f }, 18.0f, wall1 } ) );
    primitives_.push_back( Primitive::PrimitiveUniquePtr( new Sphere{ glm::vec3{  0.0f, 0.0f, -15.0f }, 14.0f, wall1 } ) );
    primitives_.push_back( Primitive::PrimitiveUniquePtr( new Sphere{ glm::vec3{  0.0f, 5.0f, 0.0f }, 2.0f, lowerLightMaterial } ) );
    primitives_.push_back( Primitive::PrimitiveUniquePtr( new Sphere{ glm::vec3{  0.0f, 6.0f, 4.0f }, 2.0f, lightMaterial } ) );
    
    model = new Model(base, fileName);
    model->addMaterial("wm2_Gun_trigger_Cube.002", wm);
    model->addMaterial("wm_Gun_Cube.001", wm);
    model->addMaterial("wd2_Cube.002_Cube.003", wd);
    model->addMaterial("wd_Cube.005_Cube.000", wd);
    model->addMaterial("sm2_Cylinder", sm);
    model->addMaterial("sm_Cylinder.001", sm);
    model->addMaterial("sw_Cube.006_Cube.010", sw);
    model->addMaterial("sw2_Cube.002_Cube.009", sw);
    model->addMaterial("sw3_Cube.005_Cube.011", sw);
    primitives_.push_back( Primitive::PrimitiveUniquePtr(model));
}
