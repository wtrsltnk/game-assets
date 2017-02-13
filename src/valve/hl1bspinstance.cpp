#include "../../include/valve/hl1bspinstance.h"

#include <glm/glm.hpp>
#include <set>

using namespace std;
using namespace valve::hl1;

BspInstance::BspInstance(BspAsset* asset)
    : _asset(asset), _shader(nullptr)
{
    this->_shader = new Shader();
    this->_shader->BuildProgram();
}

BspInstance::~BspInstance()
{
    this->Unload();
}

void BspInstance::Render(const glm::mat4& proj, const glm::mat4& view)
{
    glm::mat3 rotMat(view);
    glm::vec3 pos = -glm::vec3(view[3]) * rotMat;
    std::set<unsigned short> visibleFaces/*;visibleFaces.insert(320);//*/ = this->FindVisibleFaces(pos, this->_asset->_modelData[0].headnode[0]);
    if (visibleFaces.size() > 0)
        this->_visibleFaces = visibleFaces;

    this->_shader->UseProgram();
    this->_shader->SetProjectionMatrix(proj);
    this->_shader->SetViewMatrix(view);

    this->_asset->RenderFaces(this->_visibleFaces);
}

void BspInstance::Unload()
{
    if (this->_shader != nullptr) delete this->_shader;
    this->_shader = nullptr;
}

std::set<unsigned short> BspInstance::FindVisibleFaces(const glm::vec3& pos, int headNode)
{
    std::set<unsigned short> leafFaces;

    int leaf = this->TracePointInLeaf(pos, headNode);
    if (leaf != 0)
    {
        // add all faces of current leaf
        for (int f = 0; f < this->_asset->_leafData[leaf].markSurfacesCount; f++)
        {
            unsigned short findex = this->_asset->_marksurfaceData[this->_asset->_leafData[leaf].firstMarkSurface + f];
            if (this->_asset->FaceFlags(findex) == 0)
                leafFaces.insert(findex);
        }

        hl1::tBSPVisLeaf* visleaf = &this->_asset->_visLeafs[leaf];
        // add all faces of leafs through vis data
        for (int l = 0; l < visleaf->leafCount; l++)
        {
            for (int f = 0; f < this->_asset->_leafData[visleaf->leafs[l]].markSurfacesCount; f++)
            {
                unsigned short findex = this->_asset->_marksurfaceData[this->_asset->_leafData[visleaf->leafs[l]].firstMarkSurface + f];
                if (this->_asset->FaceFlags(findex) == 0)
                    leafFaces.insert(findex);
            }
        }

        // Add all faces of non worldspawn model
        for (int m = 1; m < this->_asset->_modelData.count; m++)
        {
            for (unsigned short f = 0; f < this->_asset->_modelData[m].faceCount; f++)
            {
                if (this->_asset->FaceFlags(f + this->_asset->_modelData[m].firstFace) != 0)
                    continue;

                leafFaces.insert(f + this->_asset->_modelData[m].firstFace);
            }
        }
    }
    else
    {
        for (unsigned short i = 0; i < this->_asset->_faceData.count; i++)
        {
            if (this->_asset->FaceFlags(i) != 0)
                continue;

            this->_visibleFaces.insert(i);
        }
    }

    return leafFaces;
}

int BspInstance::TracePointInLeaf(const glm::vec3& point, int nodenum)
{
    float d;
    hl1::tBSPNode *node;
    hl1::tBSPPlane *plane;

    while (nodenum >= 0)
    {
        node = &this->_asset->_nodeData[nodenum];
        plane = &this->_asset->_planeData[node->planeIndex];
        d = glm::dot(point, plane->normal) - plane->distance;
        if (d > 0)
            nodenum = node->children[0];
        else
            nodenum = node->children[1];
    }

    return -nodenum - 1;
}

void BspInstance::BotmanTraceLine(glm::vec3 start, glm::vec3 end, botman_trace_t *tr)
{
    tBSPLeaf  *startleaf, *endleaf;
    int      numsteps, totalsteps;
    glm::vec3   move, step, position;
    float    dist, trace_dist;

   memset(tr, 0, sizeof(botman_trace_t));

   if ((start.x < -4095) || (start.x > 4095) ||
       (start.y < -4095) || (start.y > 4095) ||
       (start.z < -4095) || (start.z > 4095))
   {
      // start beyond edge of world is INVALID!!!
      fprintf(stderr,"TraceLine: start point beyond edge of world!\n");
   }

   if (end.x > 4095.0f)
   {
      float percent = 4095.0f / end.x;
      end.y = end.y * percent;
      end.z = end.z * percent;
      end.x = 4095.0f;
   }

   if (end.y > 4095.0f)
   {
      float percent = 4095.0f / end.y;
      end.x = end.x * percent;
      end.z = end.z * percent;
      end.y = 4095.0f;
   }

   if (end.z > 4095.0f)
   {
      float percent = 4095.0f / end.z;
      end.x = end.x * percent;
      end.y = end.y * percent;
      end.z = 4095.0f;
   }

   if (end.x < -4095.0f)
   {
      float percent = 4095.0f / end.x;
      end.y = end.y * percent;
      end.z = end.z * percent;
      end.x = -4095.0f;
   }

   if (end.y < -4095.0f)
   {
      float percent = 4095.0f / end.y;
      end.x = end.x * percent;
      end.z = end.z * percent;
      end.y = -4095.0f;
   }

   if (end.z < -4095.0f)
   {
      float percent = 4095.0f / end.z;
      end.x = end.x * percent;
      end.y = end.y * percent;
      end.z = -4095.0f;
   }

   // find the starting and ending leafs...
   startleaf = &this->_asset->_leafData[TracePointInLeaf(start, 0)];
   endleaf = &this->_asset->_leafData[TracePointInLeaf(end, 0)];

   // set endpos, fraction and contents to the default (trace completed)
   tr->endpos = end;
   tr->fraction = 1.0f;
   tr->contents = endleaf->contents;

   if (startleaf->contents == CONTENTS_SOLID)
      tr->startsolid = TRUE;

   // is start and end leaf the same (couldn't possibly hit the world)...
   if (startleaf == endleaf) {
      if (startleaf->contents == CONTENTS_SOLID)
         tr->allsolid = TRUE;
      return;
   }

   // get the length of each interation of the loop...
   move = end - start;
   dist = glm::length(move);

   // determine the number of steps from start to end...
   if (dist > 1.0f)
      numsteps = totalsteps = (int)dist + 1;
   else
      numsteps = totalsteps = 1;

   // calculate the length of the step vector...
   step = move * float(2.0f / numsteps);

   position = start;

   while (numsteps)
   {
      position = position + step;

      endleaf = &this->_asset->_leafData[TracePointInLeaf(position, 0)];

      if ((endleaf->contents == CONTENTS_SOLID) ||  // we hit something solid...
          (endleaf->contents == CONTENTS_SKY))      // we hit the sky
      {
         glm::vec3 hitpos;

         hitpos = position;

         // store the hit position
         tr->hitpos = position;

         // back off one step before solid
         position = position - step;

         // store the end position and end position contents
         tr->endpos = position;
         tr->contents = endleaf->contents;

         move = position - start;
         trace_dist = glm::length(move);
         tr->fraction = trace_dist / dist;

         break;  // break out of while loop
      }

      numsteps--;
   }
}

// WARNING!!! the "vector" parameter should be NORMALIZED!!!
float DistanceToIntersection(const glm::vec3 origin, const glm::vec3 vector,
                             const glm::vec3 plane_origin, const glm::vec3 plane_normal)
{
   float d = -(glm::dot(plane_normal, plane_origin));

   float numerator = glm::dot(plane_normal, origin) + d;
   float denominator = glm::dot(plane_normal, vector);

   if (fabs(denominator) < 0.00001)
      return (-1.0f);  // normal is orthogonal to vector, no intersection

   return -(numerator/denominator);
}


#define TWO_PI 6.2831853f
#define DELTA 0.001f

// find the face where the traceline hit...
tBSPFace *BspInstance::TraceLineFindFace(glm::vec3 start, botman_trace_t *tr)
{
   glm::vec3 v_intersect, v_normalized, v_temp;
   tBSPFace *return_face = NULL;
   float min_diff = 9999.9f;

   v_normalized = glm::normalize(tr->endpos - start);

   tBSPLeaf *endleaf = &this->_asset->_leafData[TracePointInLeaf(tr->endpos, 0)];

   unsigned short *p = this->_asset->_marksurfaceData + endleaf->firstMarkSurface;

   // find a plane with endpos on one side and hitpos on the other side...
   for (int i = 0; i < endleaf->markSurfacesCount; i++)
   {
      int face_idx = *p++;

      tBSPFace *face = &this->_asset->_faceData[face_idx];

      tBSPPlane *plane = &this->_asset->_planeData[face->planeIndex];

      float d1 = glm::dot(tr->endpos, plane->normal) - plane->distance;
      float d2 = glm::dot(tr->hitpos, plane->normal) - plane->distance;

      if ((d1 > 0 && d2 <= 0)||(d1 <= 0 && d2 > 0))
      {
         // found a plane, find the intersection point in the plane...

         glm::vec3 plane_origin, v_angle1, v_angle2;

         plane_origin = (plane->normal * plane->distance);

         float dist = DistanceToIntersection(start, v_normalized, plane_origin, plane->normal);

         if (dist < 0)
            return NULL;  // can't find intersection

         v_temp = (v_normalized * dist);
         v_intersect = (start + v_temp);

         // loop through all of the vertexes of all the edges of this face and
         // find the angle between vertex-n, v_intersect and vertex-n+1, then add
         // all these angles together.  if the sum of these angles is 360 degrees
         // (or 2 PI radians), then the intersect point lies within that polygon.

         float angle_sum = 0.0f;

         // loop though all of the edges, getting the vertexes...
         for (int edge_index = 0; edge_index < face->edgeCount; edge_index++)
         {
             glm::vec3 vertex1, vertex2;

            // get the coordinates of the vertex of this edge...
            int edge = this->_asset->_surfedgeData[face->firstEdge + edge_index];

            if (edge < 0)
            {
               edge = -edge;
               tBSPEdge* e = &this->_asset->_edgeData[edge];
               vertex1 = this->_asset->_verticesData[e->vertex[1]].point;
               vertex2 = this->_asset->_verticesData[e->vertex[0]].point;
            }
            else
            {
               tBSPEdge* e = &this->_asset->_edgeData[edge];
               vertex1 = this->_asset->_verticesData[e->vertex[0]].point;
               vertex2 = this->_asset->_verticesData[e->vertex[1]].point;
            }

            // now create vectors from the vertexes to the plane intersect point...
            v_angle1 = glm::normalize(vertex1 - v_intersect);
            v_angle2 = glm::normalize(vertex2 - v_intersect);

            // find the angle between these vectors...
            float angle = glm::dot(v_angle1, v_angle2);

            angle = (float)acos(angle);

            angle_sum += angle;

            edge++;
         }

         // is the sum of the angles 360 degrees (2 PI)?...
         if ((angle_sum >= (TWO_PI - DELTA)) && (angle_sum <= (TWO_PI + DELTA)))
         {
            // find the difference between the sum and 2 PI...
            float diff = (float)fabs(angle_sum - TWO_PI);

            if (diff < min_diff)  // is this the BEST so far?...
            {
               min_diff = diff;
               return_face = face;
            }
         }
      }
   }

   return return_face;
}
