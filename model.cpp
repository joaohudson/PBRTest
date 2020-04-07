#include "model.h"
#include <sstream>
#include <fstream>
#include <iostream>

Model::Model(const Material& defaultMaterial, const std::string& fileName)
{
	load(fileName, defaultMaterial);
}

Model::~Model()
{
	for(auto mesh : data)
	{
		for(unsigned int i = 0; i < mesh.second->faces.size(); i++)
			delete mesh.second->faces[i];
		delete mesh.second;
	}
}

void Model::addMaterial(const std::string& meshName, const Material& material)
{
    data[meshName]->material = material;
}

bool Model::intersect(const Ray& ray, IntersectionRecord& intersection_record) const
{
    IntersectionRecord previous_record;//previous intersection record
    bool result = false;
    previous_record.t_ = std::numeric_limits<double>::max();

	for(auto mesh : data)
	{
        if(mesh.second->bvh->intersect(ray, previous_record))
        if((previous_record.t_ < intersection_record.t_) && (previous_record.t_ > 0.0))
        {
            intersection_record = previous_record;
            intersection_record.material_ = mesh.second->material;
            result = true;
        }
        previous_record.t_ = std::numeric_limits<double>::max();
	}

	return result;
}

static void split(const std::string& str, char delimiter, std::vector<std::string>& data)
{
	std::string token;
	int size = 0;
	int pos = 0;

	for(unsigned int i = 0; i < str.length(); i++)
	{
		if(str[i] == delimiter)
		{
			token = str.substr(pos, size);
			
			if(!token.empty())
				data.push_back(token);

			pos = i + 1;
			size = 0;
		}
		else if(i == str.length() - 1)
		{
			token = str.substr(pos, pos - str.length());
			data.push_back(token);

		}
		else
		{
			size++;
		}
	}
}

static void loadVertex(std::string& line, glm::vec3& vec)
{
    std::stringstream str;

    //discard 'v'
    str << line;

    str >> vec.x >> vec.y >> vec.z;
}

static Triangle* loadFace(std::string& line, const std::vector<glm::vec3>& vs, const std::vector<glm::vec3>& nr)
{
    int a, b, c, n0, n1, n2;
    std::stringstream str;
    std::vector<std::string> tokens; 
    std::vector<std::string> t0, t1, t2;
    //discarta o "f "
    line = line.c_str() + 2;

    split(line, ' ', tokens);

    split(tokens[0], '/', t0);
    split(tokens[1], '/', t1);
    split(tokens[2], '/', t2);

    if(t0.size() == 3)//foi incluida coordenada de textura, troca ela com a normal para conservá-la no índice 1
    {
        std::swap(t0[1], t0[2]);
        std::swap(t1[1], t1[2]);
        std::swap(t2[1], t2[2]);
    }

    //sem normais ou coordenadas de textura
    if(t0.size() == 1)
    {
        str << line;

        str >> a >> b >> c;

        return new Triangle(vs[a - 1], vs[b - 1], vs[c - 1]);
    }
    else
    {
        a = std::atoi(t0[0].c_str());
        b = std::atoi(t1[0].c_str());
        c = std::atoi(t2[0].c_str());

        n0 = std::atoi(t0[1].c_str());
        n1 = std::atoi(t1[1].c_str());
        n2 = std::atoi(t2[1].c_str());
        
        return new Triangle(vs[a - 1], vs[b - 1], vs[c - 1], nr[n0 - 1], nr[n1 - 1], nr[n2 - 1]);
    }

    return NULL;
}

void Model::load(const std::string& fileName, const Material& material)
{
    std::fstream in;
    std::vector<glm::vec3> vs;
    std::vector<glm::vec3> normals;
    glm::vec3 vec;
    std::string line;
    Triangle *triangle;
	Mesh *mesh = NULL;

    in.open(fileName, std::ios::in);

    while(!in.eof())
    {
        std::getline(in, line);

        switch(line.c_str()[0])
        {
			case 'o':
                if(mesh != NULL)
                    mesh->bvh = new BVH(mesh);
				mesh = new Mesh();
				mesh->material = material;
				//descarta os 2 primeiros caracteres
				line = line.c_str() + 2;
				this->data[line] = mesh;
			break;
				case 'v':
                //se não iniciar com vn, é um vértice
                if(line.substr(0, 2).compare("vn"))
                {
                    line = line.c_str() + 2;//discard v
                    loadVertex(line, vec);
                    vs.push_back(vec);
                }
                else//se iniciar com vn, é um vetor normal
                {
                    line = line.c_str() + 3;//discard vn
                    loadVertex(line, vec);
                    normals.push_back(vec);
                }
            break;

            case 'f':
				triangle = loadFace(line, vs, normals);
				mesh->faces.push_back(triangle);
			break;
        }
    }

    mesh->bvh = new BVH(mesh);

    in.close();
}