#ifndef MESH_HH
#define MESH_HH

#include <vector>

#include "triangle.hh"

struct mesh
{
    std::vector<triangle> triangles;
};

#endif /* !MESH_HH */
