#include <Neon/Component/SpatialPartitioning/NeonRegularGrid.h>
#include <Neon/Component/NeonMesh.h>
#include <Neon/NeonVertexBufferObject.hpp>

namespace Neon
{
	typedef struct {
		glm::vec3 p[3];
	} TRIANGLE;

	typedef struct {
		glm::vec3 p[8];
		float val[8];
		
	} GRIDCELL;


	glm::vec3 VertexInterp(float isolevel, const glm::vec3& p1, const glm::vec3& p2, float valp1, float valp2);

	/*
	   Given a grid cell and an isolevel, calculate the triangular
	   facets required to represent the isosurface through the cell.
	   Return the number of triangular facets, the array "triangles"
	   will be loaded up with the vertices at most 5 triangular facets.
		0 will be returned if the grid cell is either totally above
	   of totally below the isolevel.
	*/
	int Polygonise(GRIDCELL grid, float isolevel, TRIANGLE* triangles)
	{
		int i, ntriang;
		int cubeindex;
		glm::vec3 vertlist[12];

		int edgeTable[256] = {
		0x0  , 0x109, 0x203, 0x30a, 0x406, 0x50f, 0x605, 0x70c,
		0x80c, 0x905, 0xa0f, 0xb06, 0xc0a, 0xd03, 0xe09, 0xf00,
		0x190, 0x99 , 0x393, 0x29a, 0x596, 0x49f, 0x795, 0x69c,
		0x99c, 0x895, 0xb9f, 0xa96, 0xd9a, 0xc93, 0xf99, 0xe90,
		0x230, 0x339, 0x33 , 0x13a, 0x636, 0x73f, 0x435, 0x53c,
		0xa3c, 0xb35, 0x83f, 0x936, 0xe3a, 0xf33, 0xc39, 0xd30,
		0x3a0, 0x2a9, 0x1a3, 0xaa , 0x7a6, 0x6af, 0x5a5, 0x4ac,
		0xbac, 0xaa5, 0x9af, 0x8a6, 0xfaa, 0xea3, 0xda9, 0xca0,
		0x460, 0x569, 0x663, 0x76a, 0x66 , 0x16f, 0x265, 0x36c,
		0xc6c, 0xd65, 0xe6f, 0xf66, 0x86a, 0x963, 0xa69, 0xb60,
		0x5f0, 0x4f9, 0x7f3, 0x6fa, 0x1f6, 0xff , 0x3f5, 0x2fc,
		0xdfc, 0xcf5, 0xfff, 0xef6, 0x9fa, 0x8f3, 0xbf9, 0xaf0,
		0x650, 0x759, 0x453, 0x55a, 0x256, 0x35f, 0x55 , 0x15c,
		0xe5c, 0xf55, 0xc5f, 0xd56, 0xa5a, 0xb53, 0x859, 0x950,
		0x7c0, 0x6c9, 0x5c3, 0x4ca, 0x3c6, 0x2cf, 0x1c5, 0xcc ,
		0xfcc, 0xec5, 0xdcf, 0xcc6, 0xbca, 0xac3, 0x9c9, 0x8c0,
		0x8c0, 0x9c9, 0xac3, 0xbca, 0xcc6, 0xdcf, 0xec5, 0xfcc,
		0xcc , 0x1c5, 0x2cf, 0x3c6, 0x4ca, 0x5c3, 0x6c9, 0x7c0,
		0x950, 0x859, 0xb53, 0xa5a, 0xd56, 0xc5f, 0xf55, 0xe5c,
		0x15c, 0x55 , 0x35f, 0x256, 0x55a, 0x453, 0x759, 0x650,
		0xaf0, 0xbf9, 0x8f3, 0x9fa, 0xef6, 0xfff, 0xcf5, 0xdfc,
		0x2fc, 0x3f5, 0xff , 0x1f6, 0x6fa, 0x7f3, 0x4f9, 0x5f0,
		0xb60, 0xa69, 0x963, 0x86a, 0xf66, 0xe6f, 0xd65, 0xc6c,
		0x36c, 0x265, 0x16f, 0x66 , 0x76a, 0x663, 0x569, 0x460,
		0xca0, 0xda9, 0xea3, 0xfaa, 0x8a6, 0x9af, 0xaa5, 0xbac,
		0x4ac, 0x5a5, 0x6af, 0x7a6, 0xaa , 0x1a3, 0x2a9, 0x3a0,
		0xd30, 0xc39, 0xf33, 0xe3a, 0x936, 0x83f, 0xb35, 0xa3c,
		0x53c, 0x435, 0x73f, 0x636, 0x13a, 0x33 , 0x339, 0x230,
		0xe90, 0xf99, 0xc93, 0xd9a, 0xa96, 0xb9f, 0x895, 0x99c,
		0x69c, 0x795, 0x49f, 0x596, 0x29a, 0x393, 0x99 , 0x190,
		0xf00, 0xe09, 0xd03, 0xc0a, 0xb06, 0xa0f, 0x905, 0x80c,
		0x70c, 0x605, 0x50f, 0x406, 0x30a, 0x203, 0x109, 0x0 };
		int triTable[256][16] =
		{ {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1},
		{3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1},
		{3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1},
		{3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1},
		{9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1},
		{1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1},
		{9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
		{2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1},
		{8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1},
		{9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
		{4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1, -1},
		{3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1, -1},
		{1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1, -1},
		{4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1, -1},
		{4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1},
		{9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1},
		{1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
		{5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1},
		{2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1},
		{9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
		{0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
		{2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1, -1},
		{10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1},
		{4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1, -1},
		{5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1, -1},
		{5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1},
		{9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1},
		{0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1},
		{1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1, -1},
		{10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1, -1},
		{8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1, -1},
		{2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1},
		{7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1},
		{9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1, -1},
		{2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1},
		{11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1},
		{9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1, -1},
		{5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1},
		{11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, -1},
		{11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
		{1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1, -1},
		{9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1},
		{5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1},
		{2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
		{0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
		{5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1, -1},
		{6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1},
		{0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1},
		{3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1},
		{6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1},
		{5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1},
		{1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
		{10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1, -1},
		{6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1},
		{1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1, -1},
		{8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1, -1},
		{7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1},
		{3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
		{5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1, -1},
		{0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1},
		{9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, -1},
		{8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1, -1},
		{5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, -1},
		{0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, -1},
		{6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1},
		{10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1},
		{10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1},
		{8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1},
		{1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1},
		{3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1, -1},
		{0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1},
		{10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1},
		{0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1, -1},
		{3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1, -1},
		{6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, -1},
		{9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1, -1},
		{8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, -1},
		{3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1},
		{6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1},
		{0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1, -1},
		{10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1},
		{10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1},
		{1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1},
		{2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, -1},
		{7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1},
		{7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1, -1},
		{2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1},
		{1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, -1},
		{11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1, -1},
		{8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, -1},
		{0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1, -1},
		{7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
		{10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
		{2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
		{6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1, -1},
		{7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1},
		{2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1},
		{1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1},
		{10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1},
		{10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1},
		{0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1, -1},
		{7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1, -1},
		{6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1},
		{8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1, -1},
		{9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1, -1},
		{6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1, -1},
		{1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1, -1},
		{4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1, -1},
		{10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, -1},
		{8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1},
		{0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1, -1},
		{1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1},
		{8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1, -1},
		{10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1},
		{4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, -1},
		{10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
		{5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
		{11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1, -1},
		{9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
		{6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1, -1},
		{7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1, -1},
		{3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, -1},
		{7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1},
		{9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1, -1},
		{3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1, -1},
		{6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, -1},
		{9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1, -1},
		{1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, -1},
		{4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, -1},
		{7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1, -1},
		{6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1},
		{3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1},
		{0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1, -1},
		{6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1},
		{1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1, -1},
		{0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, -1},
		{11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, -1},
		{6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1, -1},
		{5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1},
		{9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1},
		{1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, -1},
		{1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, -1},
		{10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1, -1},
		{0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1, -1},
		{5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1, -1},
		{10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1, -1},
		{11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1},
		{0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1, -1},
		{9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1, -1},
		{7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, -1},
		{2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1},
		{8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1, -1},
		{9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1, -1},
		{9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, -1},
		{1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1},
		{9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1},
		{9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1},
		{5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1, -1},
		{0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1, -1},
		{10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, -1},
		{2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1, -1},
		{0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, -1},
		{0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, -1},
		{9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1},
		{5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1},
		{3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, -1},
		{5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1, -1},
		{8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1},
		{0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1, -1},
		{9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1},
		{0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1, -1},
		{1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1, -1},
		{3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, -1},
		{4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1, -1},
		{9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, -1},
		{11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1},
		{11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1, -1},
		{2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1},
		{9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, -1},
		{3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, -1},
		{1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1},
		{4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1, -1},
		{4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1, -1},
		{0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1},
		{3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1, -1},
		{3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1, -1},
		{0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1},
		{9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1, -1},
		{1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
		{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1} };

		/*
		   Determine the index into the edge table which
		   tells us which vertices are inside of the surface
		*/
		cubeindex = 0;
		if (grid.val[0] < isolevel) cubeindex |= 1;
		if (grid.val[1] < isolevel) cubeindex |= 2;
		if (grid.val[2] < isolevel) cubeindex |= 4;
		if (grid.val[3] < isolevel) cubeindex |= 8;
		if (grid.val[4] < isolevel) cubeindex |= 16;
		if (grid.val[5] < isolevel) cubeindex |= 32;
		if (grid.val[6] < isolevel) cubeindex |= 64;
		if (grid.val[7] < isolevel) cubeindex |= 128;

		/* Cube is entirely in/out of the surface */
		if (edgeTable[cubeindex] == 0)
			return(0);

		/* Find the vertices where the surface intersects the cube */
		if (edgeTable[cubeindex] & 1)
			vertlist[0] =
			VertexInterp(isolevel, grid.p[0], grid.p[1], grid.val[0], grid.val[1]);
		if (edgeTable[cubeindex] & 2)
			vertlist[1] =
			VertexInterp(isolevel, grid.p[1], grid.p[2], grid.val[1], grid.val[2]);
		if (edgeTable[cubeindex] & 4)
			vertlist[2] =
			VertexInterp(isolevel, grid.p[2], grid.p[3], grid.val[2], grid.val[3]);
		if (edgeTable[cubeindex] & 8)
			vertlist[3] =
			VertexInterp(isolevel, grid.p[3], grid.p[0], grid.val[3], grid.val[0]);
		if (edgeTable[cubeindex] & 16)
			vertlist[4] =
			VertexInterp(isolevel, grid.p[4], grid.p[5], grid.val[4], grid.val[5]);
		if (edgeTable[cubeindex] & 32)
			vertlist[5] =
			VertexInterp(isolevel, grid.p[5], grid.p[6], grid.val[5], grid.val[6]);
		if (edgeTable[cubeindex] & 64)
			vertlist[6] =
			VertexInterp(isolevel, grid.p[6], grid.p[7], grid.val[6], grid.val[7]);
		if (edgeTable[cubeindex] & 128)
			vertlist[7] =
			VertexInterp(isolevel, grid.p[7], grid.p[4], grid.val[7], grid.val[4]);
		if (edgeTable[cubeindex] & 256)
			vertlist[8] =
			VertexInterp(isolevel, grid.p[0], grid.p[4], grid.val[0], grid.val[4]);
		if (edgeTable[cubeindex] & 512)
			vertlist[9] =
			VertexInterp(isolevel, grid.p[1], grid.p[5], grid.val[1], grid.val[5]);
		if (edgeTable[cubeindex] & 1024)
			vertlist[10] =
			VertexInterp(isolevel, grid.p[2], grid.p[6], grid.val[2], grid.val[6]);
		if (edgeTable[cubeindex] & 2048)
			vertlist[11] =
			VertexInterp(isolevel, grid.p[3], grid.p[7], grid.val[3], grid.val[7]);

		/* Create the triangle */
		ntriang = 0;
		for (i = 0; triTable[cubeindex][i] != -1; i += 3) {
			triangles[ntriang].p[0] = vertlist[triTable[cubeindex][i]];
			triangles[ntriang].p[1] = vertlist[triTable[cubeindex][i + 1]];
			triangles[ntriang].p[2] = vertlist[triTable[cubeindex][i + 2]];
			ntriang++;
		}

		return(ntriang);
	}

	/*
	   Linearly interpolate the position where an isosurface cuts
	   an edge between two vertices, each with their own scalar value
	*/

	glm::vec3 VertexInterp(float isolevel, const glm::vec3& p1, const glm::vec3& p2, float valp1, float valp2)
	{
		float mu;
		glm::vec3 p;

		if (fabsf(isolevel - valp1) < 0.00001)
			return(p1);
		if (fabsf(isolevel - valp2) < 0.00001)
			return(p2);
		if (fabsf(valp1 - valp2) < 0.00001)
			return(p1);
		mu = (isolevel - valp1) / (valp2 - valp1);
		p.x = p1.x + mu * (p2.x - p1.x);
		p.y = p1.y + mu * (p2.y - p1.y);
		p.z = p1.z + mu * (p2.z - p1.z);

		return(p);
	}

	RegularGrid::RegularGrid(const string& name, float cellSize)
		: ComponentBase(name), mesh(nullptr), cellSize(cellSize), cellHalfSize(cellSize * 0.5f)
	{
	}

	RegularGrid::RegularGrid(const string& name, Mesh* mesh, float cellSize)
		: ComponentBase(name), mesh(mesh), cellSize(cellSize), cellHalfSize(cellSize * 0.5f)
	{
	}

	RegularGrid::~RegularGrid()
	{
		for (size_t z = 0; z < cellCountZ; z++)
		{
			for (size_t y = 0; y < cellCountY; y++)
			{
				for (size_t x = 0; x < cellCountX; x++)
				{
					SAFE_DELETE(cells[z][y][x]);
				}
			}
		}
		cells.clear();

		for (auto& v : vertices)
		{
			SAFE_DELETE(v);
		}
		vertices.clear();

		for (auto& t : triangles)
		{
			SAFE_DELETE(t);
		}
		triangles.clear();
	}

	void RegularGrid::Build()
	{
		if (nullptr == mesh)
			return;

		auto meshAABB = mesh->GetAABB();
		cellCountX = (size_t)ceilf(meshAABB.GetXLength() / cellSize) + 1;
		cellCountY = (size_t)ceilf(meshAABB.GetYLength() / cellSize) + 1;
		cellCountZ = (size_t)ceilf(meshAABB.GetZLength() / cellSize) + 1;

		float nx = -((float)cellCountX * cellSize) * 0.5f;
		float px = ((float)cellCountX * cellSize) * 0.5f;
		float ny = -((float)cellCountY * cellSize) * 0.5f;
		float py = ((float)cellCountY * cellSize) * 0.5f;
		float nz = -((float)cellCountZ * cellSize) * 0.5f;
		float pz = ((float)cellCountZ * cellSize) * 0.5f;
		float cx = (px + nx) * 0.5f;
		float cy = (py + ny) * 0.5f;
		float cz = (pz + nz) * 0.5f;
		this->Expand(glm::vec3(nx, ny, nz) + meshAABB.GetCenter());
		this->Expand(glm::vec3(px, py, pz) + meshAABB.GetCenter());

		cells.resize(cellCountZ);
		for (size_t z = 0; z < cellCountZ; z++)
		{
			cells[z].resize(cellCountY);
			for (size_t y = 0; y < cellCountY; y++)
			{
				cells[z][y].resize(cellCountX);
				for (size_t x = 0; x < cellCountX; x++)
				{
					auto minPoint = glm::vec3(xyz.x + (float)x * cellSize, xyz.y + (float)y * cellSize, xyz.z + (float)z * cellSize);
					auto maxPoint = glm::vec3(xyz.x + (float)(x + 1) * cellSize, xyz.y + (float)(y + 1) * cellSize, xyz.z + (float)(z + 1) * cellSize);
					auto cell = new RegularGridCell<Vertex, Triangle>(minPoint, maxPoint);
					cells[z][y][x] = cell;
				}
			}
		}

		auto vb = mesh->GetVertexBuffer();
		auto nov = vb->Size();
		map<GLuint, Vertex*> indexVertexMapping;
		for (size_t i = 0; i < nov; i++)
		{
			auto v = new Vertex{ i };
			InsertVertex(v);
			indexVertexMapping[(GLuint)i] = v;
		}

		auto ib = mesh->GetIndexBuffer();
		auto noi = ib->Size();
		for (size_t i = 0; i < noi / 3; i++)
		{
			GLuint i0, i1, i2;
			mesh->GetTriangleVertexIndices(i, i0, i1, i2);
			auto t = new Triangle{ indexVertexMapping[i0], indexVertexMapping[i1], indexVertexMapping[i2] };
			InsertTriangle(t);
		}
	}

	tuple<size_t, size_t, size_t> RegularGrid::InsertVertex(Vertex* vertex)
	{
		auto vertexPosition = mesh->GetVertex(vertex->index);

		auto index = GetIndex(vertexPosition);
		auto cell = GetCell(index);
		if (nullptr != cell) {
			cell->GetVertices().insert(vertex);
		}
		return index;
	}

	void RegularGrid::InsertTriangle(Triangle* t)
	{
		auto& p0 = mesh->GetVertex(t->v0->index);
		auto& p1 = mesh->GetVertex(t->v1->index);
		auto& p2 = mesh->GetVertex(t->v2->index);

		AABB taabb;
		taabb.Expand(p0);
		taabb.Expand(p1);
		taabb.Expand(p2);
		auto minIndex = GetIndex(taabb.GetMinPoint());
		auto maxIndex = GetIndex(taabb.GetMaxPoint());
		for (size_t z = get<2>(minIndex); z <= get<2>(maxIndex); z++) {
			for (size_t y = get<1>(minIndex); y <= get<1>(maxIndex); y++) {
				for (size_t x = get<0>(minIndex); x <= get<0>(maxIndex); x++) {
					auto cell = cells[(int)z][(int)y][(int)x];
					if (cell->IntersectsTriangle(p0, p1, p2))
					{
						cell->GetTriangles().insert(t);
					}
				}
			}
		}
	}

	vector<vector<glm::vec3>> RegularGrid::ExtractSurface(float isolevel)
	{
		auto t = Neon::Time("ExtractSurface");

		auto cellHalfSize = cellSize * 0.5f;

		vector<vector<glm::vec3>> result;

		for (size_t z = 0; z < cellCountZ; z++)
		{
			for (size_t y = 0; y < cellCountY; y++)
			{
				for (size_t x = 0; x < cellCountX; x++)
				{
					auto cell = GetCell(x, y, z);
					if (nullptr == cell)
						continue;

					{
						GRIDCELL gridCell;
						gridCell.p[0] = cell->xyz - cellHalfSize;
						gridCell.p[1] = cell->Xyz - cellHalfSize;
						gridCell.p[2] = cell->XyZ - cellHalfSize;
						gridCell.p[3] = cell->xyZ - cellHalfSize;
						gridCell.p[4] = cell->xYz - cellHalfSize;
						gridCell.p[5] = cell->XYz - cellHalfSize;
						gridCell.p[6] = cell->XYZ - cellHalfSize;
						gridCell.p[7] = cell->xYZ - cellHalfSize;
						gridCell.val[0] = 1.0f;
						gridCell.val[1] = 1.0f;
						gridCell.val[2] = 1.0f;
						gridCell.val[3] = 1.0f;
						gridCell.val[4] = 1.0f;
						gridCell.val[5] = 1.0f;
						gridCell.val[6] = 1.0f;
						gridCell.val[7] = 1.0f;

						for (size_t i = 0; i < 8; i++)
						{
							auto pcell = GetCell(GetIndex(gridCell.p[i]));
							if (nullptr != pcell)
							{
								if (pcell->selected)
								{
									gridCell.val[i] = -1.0f;
								}
							}
						}

						TRIANGLE triangles[16];
						int not = Polygonise(gridCell, isolevel, (TRIANGLE*)triangles);
						for (size_t i = 0; i < not; i++)
						{
							auto v0 = triangles[i].p[0];
							auto v1 = triangles[i].p[1];
							auto v2 = triangles[i].p[2];

							result.push_back({ triangles[i].p[0], triangles[i].p[1], triangles[i].p[2] });
						}
					}
					if (x == cellCountX - 1 || y == cellCountY - 1 || z == cellCountZ - 1)
					{
						GRIDCELL gridCell;
						gridCell.p[0] = cell->xyz + cellHalfSize;
						gridCell.p[1] = cell->Xyz + cellHalfSize;
						gridCell.p[2] = cell->XyZ + cellHalfSize;
						gridCell.p[3] = cell->xyZ + cellHalfSize;
						gridCell.p[4] = cell->xYz + cellHalfSize;
						gridCell.p[5] = cell->XYz + cellHalfSize;
						gridCell.p[6] = cell->XYZ + cellHalfSize;
						gridCell.p[7] = cell->xYZ + cellHalfSize;
						gridCell.val[0] = 1.0f;
						gridCell.val[1] = 1.0f;
						gridCell.val[2] = 1.0f;
						gridCell.val[3] = 1.0f;
						gridCell.val[4] = 1.0f;
						gridCell.val[5] = 1.0f;
						gridCell.val[6] = 1.0f;
						gridCell.val[7] = 1.0f;

						for (size_t i = 0; i < 8; i++)
						{
							auto pcell = GetCell(GetIndex(gridCell.p[i]));
							if (nullptr != pcell)
							{
								if (pcell->selected)
								{
									gridCell.val[i] = -1.0f;
								}
							}
						}

						TRIANGLE triangles[16];
						int not = Polygonise(gridCell, isolevel, (TRIANGLE*)triangles);
						for (size_t i = 0; i < not; i++)
						{
							auto v0 = triangles[i].p[0];
							auto v1 = triangles[i].p[1];
							auto v2 = triangles[i].p[2];

							result.push_back({ triangles[i].p[0], triangles[i].p[1], triangles[i].p[2] });
						}
					}
				}
			}
		}

		return result;
	}

	void RegularGrid::ForEachCell(function<void(RegularGridCell<Vertex, Triangle>*, size_t, size_t, size_t)> callback)
	{
		auto t = Time("ForEachCell");

		for (size_t z = 0; z < cellCountZ; z++)
		{
			for (size_t y = 0; y < cellCountY; y++)
			{
				for (size_t x = 0; x < cellCountX; x++)
				{
					auto cell = GetCell(x, y, z);
					callback(cell, x, y, z);
				}
			}
		}
	}

	void RegularGrid::SelectOutsideCells()
	{
		auto t = Time("SelectOutsideCells()");

		stack<tuple<size_t, size_t, size_t>> toCheck;
		toCheck.push(make_tuple(0, 0, 0));
		while (false == toCheck.empty())
		{
			auto currentIndex = toCheck.top();
			toCheck.pop();

			auto cell = GetCell(currentIndex);
			if (nullptr == cell) continue;

			cell->tempFlag = 1024;

			if (0 < cell->GetTriangles().size())
				continue;

			if (nullptr != cell)
			{
				if(false == cell->selected)
				{
					cell->selected = true;

					auto x = get<0>(currentIndex);
					auto y = get<1>(currentIndex);
					auto z = get<2>(currentIndex);

					auto mx = x - 1; auto px = x + 1;
					auto my = y - 1; auto py = y + 1;
					auto mz = z - 1; auto pz = z + 1;

					Clamp(mx, 0, cellCountX - 1);
					Clamp(px, 0, cellCountX - 1);

					Clamp(my, 0, cellCountY - 1);
					Clamp(py, 0, cellCountY - 1);

					Clamp(mz, 0, cellCountZ - 1);
					Clamp(pz, 0, cellCountZ - 1);

					toCheck.push(make_tuple(mx, y, z));
					toCheck.push(make_tuple(px, y, z));

					toCheck.push(make_tuple(x, my, z));
					toCheck.push(make_tuple(x, py, z));

					toCheck.push(make_tuple(x, y, mz));
					toCheck.push(make_tuple(x, y, pz));
				}
			}
		}
	}

	void RegularGrid::InvertSelectedCells()
	{
		auto t = Time("InvertSelectedCells()");

		for (size_t z = 0; z < cellCountZ; z++)
		{
			for (size_t y = 0; y < cellCountY; y++)
			{
				for (size_t x = 0; x < cellCountX; x++)
				{
					auto cell = GetCell(x, y, z);
					if (nullptr != cell)
					{
						cell->selected = !cell->selected;
					}
				}
			}
		}
	}

	void RegularGrid::ShrinkSelectedCells(int iteration)
	{
		auto t = Time("ShrinkSelectedCells()");

		for (size_t i = 0; i < iteration; i++)
		{
			for (size_t z = 0; z < cellCountZ; z++)
			{
				for (size_t y = 0; y < cellCountY; y++)
				{
					for (size_t x = 0; x < cellCountX; x++)
					{
						auto cell = GetCell(x, y, z);
						if (nullptr != cell)
						{
							auto mx = x - 1; auto px = x + 1;
							auto my = y - 1; auto py = y + 1;
							auto mz = z - 1; auto pz = z + 1;

							auto cellMX = GetCell(mx, y, z);
							auto cellPX = GetCell(px, y, z);

							auto cellMY = GetCell(x, my, z);
							auto cellPY = GetCell(x, py, z);

							auto cellMZ = GetCell(x, y, mz);
							auto cellPZ = GetCell(x, y, pz);

							bool allSelected = true;
							if (nullptr != cellMX) { if (false == cellMX->selected) allSelected = false; }
							else { allSelected = false; }
							if (nullptr != cellPX) { if (false == cellPX->selected) allSelected = false; }
							else { allSelected = false; }
							if (nullptr != cellMY) { if (false == cellMY->selected) allSelected = false; }
							else { allSelected = false; }
							if (nullptr != cellPY) { if (false == cellPY->selected) allSelected = false; }
							else { allSelected = false; }
							if (nullptr != cellMZ) { if (false == cellMZ->selected) allSelected = false; }
							else { allSelected = false; }
							if (nullptr != cellPZ) { if (false == cellPZ->selected) allSelected = false; }
							else { allSelected = false; }

							if (false == allSelected)
							{
								cell->tempFlag = 128;
							}
						}
					}
				}
			}

			for (size_t z = 0; z < cellCountZ; z++)
			{
				for (size_t y = 0; y < cellCountY; y++)
				{
					for (size_t x = 0; x < cellCountX; x++)
					{
						auto cell = GetCell(x, y, z);
						if (nullptr != cell)
						{
							if (cell->tempFlag == 128)
							{
								cell->tempFlag = 0;
								cell->selected = false;
							}
						}
					}
				}
			}
		}
	}

	void RegularGrid::ExtrudeSelectedCells(const glm::vec3& direction, int iteration)
	{
		auto t = Time("ExtrudeSelectedCells()");

		for (size_t i = 0; i < iteration; i++)
		{
			for (size_t z = 0; z < cellCountZ; z++)
			{
				for (size_t y = 0; y < cellCountY; y++)
				{
					for (size_t x = 0; x < cellCountX; x++)
					{
						auto cell = GetCell(x, y, z);
						if (cell->selected)
						{
							auto offset = direction;
							offset.x *= cell->GetXLength();
							offset.y *= cell->GetYLength();
							offset.z *= cell->GetZLength();
							auto index = GetIndex(cell->GetCenter() + offset);
							auto nextCell = GetCell(index);
							if (nullptr != nextCell)
							{
								nextCell->tempFlag = 512;
							}
						}
					}
				}
			}

			for (size_t z = 0; z < cellCountZ; z++)
			{
				for (size_t y = 0; y < cellCountY; y++)
				{
					for (size_t x = 0; x < cellCountX; x++)
					{
						auto cell = GetCell(x, y, z);
						if (512 == cell->tempFlag)
						{
							cell->selected = true;
						}
					}
				}
			}
		}
	}

	//void RegularGrid::RemoveTriangle(Triangle* t)
	//{
	//	auto& p0 = mesh->GetVertex(t->v0->index);
	//	auto& p1 = mesh->GetVertex(t->v1->index);
	//	auto& p2 = mesh->GetVertex(t->v2->index);

	//	AABB taabb;
	//	taabb.Expand(p0);
	//	taabb.Expand(p1);
	//	taabb.Expand(p2);
	//	auto minIndex = GetIndex(taabb.GetMinPoint());
	//	auto maxIndex = GetIndex(taabb.GetMaxPoint());
	//	for (int z = get<2>(minIndex); z <= get<2>(maxIndex); z++) {
	//		for (int y = get<1>(minIndex); y <= get<1>(maxIndex); y++) {
	//			for (int x = get<0>(minIndex); x <= get<0>(maxIndex); x++) {
	//				auto cell = GetCell(make_tuple(x, y, z));
	//				if (nullptr != cell) {
	//					cell->GetTriangles().erase(t);
	//				}
	//			}
	//		}
	//	}
	//}

	//void RegularGrid::Build()
	//{
	//	auto nov = mesh->GetVertexBuffer()->Size();
	//	for (size_t i = 0; i < nov; i++)
	//	{
	//		vertices.push_back(Vertex{ i });

	//		InsertVertex(&(vertices[i]));
	//	}

	//	auto nof = mesh->GetIndexBuffer()->Size() / 3;
	//	for (size_t i = 0; i < nof; i++)
	//	{
	//		GLuint i0, i1, i2;
	//		mesh->GetTriangleVertexIndices(i, i0, i1, i2);

	//		triangles.push_back(Triangle{ &(vertices[i0]), &(vertices[i1]), &(vertices[i2]) });
	//		InsertTriangle(&(triangles[i]));
	//	}
	//}

	//set<RegularGridCell<RegularGrid::Vertex, RegularGrid::Triangle>*> RegularGrid::GetCellsWithRay(const Ray& ray)
	//{
	//	set<RegularGridCell<Vertex, Triangle>*> candidates;

	//	vector<glm::vec3> intersections;
	//	if (IntersectsRay(ray, intersections)) {
	//		auto origin = intersections[0];
	//		auto direction = glm::normalize(intersections[1] - intersections[0]);
	//		origin = origin - direction * cellSize * 2.0f;
	//		auto distance = glm::distance(intersections[0], intersections[1]) + cellSize * 2;
	//		auto accumulatedDistance = 0.0f;
	//		while (accumulatedDistance <= distance) {
	//			auto position = origin + direction * accumulatedDistance;
	//			auto index = GetIndex(position);
	//			for (int z = get<2>(index) - 1; z <= get<2>(index) + 1; ++z) {
	//				for (int y = get<1>(index) - 1; y <= get<1>(index) + 1; ++y) {
	//					for (int x = get<0>(index) - 1; x <= get<0>(index) + 1; ++x) {
	//						auto cell = GetCell(make_tuple(x, y, z));
	//						if (nullptr != cell) {
	//							candidates.insert(cell);
	//						}
	//					}
	//				}
	//			}
	//			accumulatedDistance += cellHalfSize;
	//		}
	//	}
	//	else {
	//		cout << "NOT intersects" << endl;
	//	}

	//	return candidates;
	//}
}
