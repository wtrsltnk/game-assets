#include "../../include/valve/hl1mapinstance.h"
#include "../../include/valve/hl1worldsize.h"

#include <GL/glextl.h>

using namespace valve::hl1;

MapInstance::MapInstance(MapAsset* asset)
	: _asset(asset)
{
    this->_shader = new MapShader();
    this->_shader->BuildProgram();

    std::vector<tVertex> vertices;

    std::vector<MapAsset::tEntity>::iterator i = this->_asset->_entities.begin();
    for (; i != this->_asset->_entities.end(); ++i)
    {
        MapAsset::tEntity& entity = *i;

        std::vector<MapAsset::tBrush>::iterator j = entity._brushes.begin();
        for (; j != entity._brushes.end(); ++j)
        {
            MapAsset::tBrush& brush = *j;

            std::vector<MapAsset::tBrushFace>::iterator k = brush._brushFaces.begin();
            for (; k != brush._brushFaces.end(); ++k)
            {
                MapAsset::tBrushFace& face = *k;
                std::vector<glm::vec3> verts = MapInstance::CreateWindingFromPlane(face.normal, face.distance);

                std::vector<MapAsset::tBrushFace>::iterator l = brush._brushFaces.begin();
                for (; l != brush._brushFaces.end(); ++l)
                {
                    MapAsset::tBrushFace& f = *l;
                    verts = MapInstance::ClipWinding(verts, f.normal, f.distance);
                }

                tFace o;
                o.firstVertex = vertices.size();
                o.vertexCount = verts.size();
                o.texture = face.texture;
                o.normal = face.normal;
                this->_faces.push_back(o);

                Texture* texture = this->_asset->_textures[face.texture];
                float W, H, SX, SY;
                W = 1.0f / (float)texture->Width();
                H = 1.0f / (float)texture->Height();
                SX = 1.0f / face.material_v220.xscale;
                SY = 1.0f / face.material_v220.yscale;

                for (unsigned int i = 0; i < verts.size(); i++)
                {
                    tVertex vert;
                    vert.pos = verts[i];
                    vert.uv[0] = (glm::dot(vert.pos, face.material_v220.uaxis) * W * SX) + (face.material_v220.ushift * W);
                    vert.uv[1] = (glm::dot(vert.pos, face.material_v220.vaxis) * H * SY) + (face.material_v220.vshift * H);
                    vertices.push_back(vert);
                }

            }
        }
    }

    glGenVertexArrays(1, &this->_vao);
    glBindVertexArray(this->_vao);

    glGenBuffers(1, &this->_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, this->_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(tVertex), (const GLvoid *)&vertices[0], GL_STATIC_DRAW);

    // Vertices
    glVertexAttribPointer((GLuint)ShaderAttributeLocations::Vertex, 3, GL_FLOAT, GL_FALSE, sizeof(tVertex), 0);
    glEnableVertexAttribArray((GLuint)ShaderAttributeLocations::Vertex);
    // Texcoords
    glVertexAttribPointer((GLuint)ShaderAttributeLocations::TexCoord, 2, GL_FLOAT, GL_FALSE, sizeof(tVertex), (const GLvoid *)(sizeof(GLfloat)*3));
    glEnableVertexAttribArray((GLuint)ShaderAttributeLocations::TexCoord);

    glBindVertexArray(0);                           // Unbind vertex array
}

MapInstance::~MapInstance()
{ }

void MapInstance::Update(float dt)
{ }

void MapInstance::Render(const glm::mat4& proj, const glm::mat4& view)
{
    this->_shader->UseProgram();
    this->_shader->SetProjectionMatrix(proj);
    this->_shader->SetViewMatrix(view);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    glBindVertexArray(this->_vao);

    for (int i = 0; i < this->_faces.size(); i++)
    {
        glBindTexture(GL_TEXTURE_2D, this->_asset->_textures[this->_faces[i].texture]->GlIndex());
        glDrawArrays(GL_POLYGON, this->_faces[i].firstVertex, this->_faces[i].vertexCount);
    }

    glBindVertexArray(0);
}

#define	SIDE_FRONT		0
#define	SIDE_BACK		1
#define	SIDE_ON			2

#define	BOGUS_RANGE	( MAX_COORD_INTEGER * 4 )

#define	SPLIT_EPSILON	0.01

const int MAX_POINTS_ON_WINDING	= 128;

glm::vec3 SplitEdge(const glm::vec3& a, float distA, const glm::vec3& b, float distB, const glm::vec3& planeNormal, float planeDistance)
{
    glm::vec3 rest;
    float dot = distA / (distA - distB);
    for (int j = 0; j < 3; j++)
    {
        if (planeNormal[j] == 1)
            rest[j] = planeDistance;
        else if (planeNormal[j] == -1)
            rest[j] = -planeDistance;
        else
            rest[j] = a[j] + dot * (b[j] - a[j]);
    }
    return rest;
}

std::vector<glm::vec3> MapInstance::ClipWinding(const std::vector<glm::vec3>& in, const glm::vec3& planeNormal, float planeDistance)
{
    std::vector<glm::vec3> result;

    float	dists[MAX_POINTS_ON_WINDING];
    int		sides[MAX_POINTS_ON_WINDING];
    int		counts[3];
    float   dot;

    // determine sides for each point
    for (unsigned int i = 0 ; i < in.size(); i++)
    {
        dot = glm::dot(in[i], planeNormal);
        dot -= planeDistance;
        dists[i] = dot;
        if (dot < SPLIT_EPSILON)
            sides[i] = SIDE_FRONT;
        else if (dot > -SPLIT_EPSILON)
            sides[i] = SIDE_BACK;
        else
            sides[i] = SIDE_ON;

        counts[sides[i]]++;
    }

    // All vertices are on the plane
    if (counts[SIDE_FRONT] == 0 && counts[SIDE_BACK] == 0)
        return in;

    // There are no vertices in front of the plane so everything is clipped
    else if (counts[SIDE_FRONT] == 0)
        return result;

    // There are no vertices behind the plane, so nothing to clip
    else if (counts[SIDE_BACK] == 0)
        return in;

    for (unsigned int i = 0 ; i < in.size(); i++)
    {
        int inext = (i+1) % in.size();
        if (sides[i] == SIDE_ON)
        {
            result.push_back(in[i]);
        }
        else if (sides[i] == SIDE_FRONT)
        {
            result.push_back(in[i]);
            if (sides[(i+1) % in.size()] == SIDE_BACK)
                result.push_back(SplitEdge(in[i], dists[i], in[(i+1) % in.size()], dists[(i+1) % in.size()], planeNormal, planeDistance));
         }
        else if (sides[i] == SIDE_BACK)
        {
            if (sides[inext] == SIDE_FRONT)
                result.push_back(SplitEdge(in[i], dists[i], in[inext], dists[inext], planeNormal, planeDistance));
        }
    }
    return result;
}

void VectorMA (const glm::vec3& va, float scale, const glm::vec3& vb, glm::vec3& vc)
{
    vc.x = va.x + (scale * vb.x);
    vc.y = va.y + (scale * vb.y);
    vc.z = va.z + (scale * vb.z);
}

//-----------------------------------------------------------------------------
// Purpose: Creates a huge quadrilateral winding given a plane.
// Input  : pPlane - Plane normal and distance to use when creating the winding.
// Output : Returns a winding with 4 points.
//-----------------------------------------------------------------------------
// dvs: read through this and clean it up
std::vector<glm::vec3> MapInstance::CreateWindingFromPlane(const glm::vec3& planeNormal, float planeDistance)
{
    float max, v;
    glm::vec3 org, vright, vup;

    // find the major axis
    max = -BOGUS_RANGE;
    int x = -1;
    for (int i = 0; i < 3; i++)
    {
        v = fabs(planeNormal[i]);
        if (v > max)
        {
            x = i;
            max = v;
        }
    }

    if (x == -1)
        throw ("BasePolyForPlane: no axis found");

    if (x == 0 || x == 1)
        vup[2] = 1;
    else if (x == 2)
        vup[0] = 1;

    v = glm::dot(vup, planeNormal);
    VectorMA (vup, -v, planeNormal, vup);
    vup = glm::normalize(vup);
    org = planeNormal * planeDistance;
    vright = glm::cross(vup, planeNormal);

    vup = glm::vec3(vup.x * MAX_TRACE_LENGTH, vup.y * MAX_TRACE_LENGTH, vup.z * MAX_TRACE_LENGTH);
    vright = glm::vec3(vright.x * MAX_TRACE_LENGTH, vright.y * MAX_TRACE_LENGTH, vright.z * MAX_TRACE_LENGTH);

    std::vector<glm::vec3> result;

    result.push_back(glm::vec3((org - vright) + vup));
    result.push_back(glm::vec3((org + vright) + vup));
    result.push_back(glm::vec3((org + vright) - vup));
    result.push_back(glm::vec3((org - vright) - vup));

    return result;
}
