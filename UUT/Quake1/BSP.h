#pragma once
#include <Core/Math/Vector3.h>

namespace uutQ1
{
	namespace bsp
	{
		struct Lump
		{
			int fileofs;
			int filelen;
		};

		static const int MaxMapHulls = 4;

		enum LumpType
		{
			LUMP_ENTITIES,
			LUMP_PLANES,
			LUMP_TEXTURES,
			LUMP_VERTEXES,
			LUMP_VISIBILITY,
			LUMP_NODES,
			LUMP_TEXINFO,
			LUMP_FACES,
			LUMP_LIGHTNING,
			LUMP_CLIPNODES,
			LUMP_LEAFS,
			LUMP_MARKSURFACES,
			LUMP_EDGES,
			LUMP_SURFEDGES,
			LUMP_MODELS,
			MAX_LUMP,
		};

		static const int HEADER_ID = 0x1D;

		struct Header
		{
			int version; // Must be 0x1d (29)
			Lump lumps[MAX_LUMP];
		};

		struct BBox // Bounding Box, Float values
		{
			uut::Vector3 min; // minimum values of X,Y,Z
			uut::Vector3 max; // maximum values of X,Y,Z
		};

		struct BBoxShort // Bounding Box, Short values
		{
			short min; // minimum values of X,Y,Z
			short max; // maximum values of X,Y,Z
		} ;

		struct Model
		{
			BBox bbox;
			uut::Vector3 origin;
			int headnode[MaxMapHulls];
			int visleafs; // not including the solid leaf 0
			int firstface, numfaces;
		};

		struct MipTexLump
		{
			int version;
			int dataofs[4];
		};

		struct MipTex
		{
			char name[16];
			unsigned width, height;
			unsigned offsets[4]; // four mip maps stored
		};

		enum PlaneType
		{
			PLANE_X,
			PLANE_Y,
			PLANE_Z,
			// 3-5 are non-axial planes snapped to the nearest
			PLANE_ANYX,
			PLANE_ANYY,
			PLANE_ANYZ,
		};

		struct Plane
		{
			uut::Vector3 normal;
			float dist;
			PlaneType type;
		};

		struct Node
		{
			int planenum;
			short children[2]; // negative numbers are -(leafs+1), not nodes
			short mins[3];
			short maxs[3];
			unsigned short firstface;
			unsigned short numfaces; // counting both sides
		};

		struct ClipNode
		{
			int planenum;
			short children[2];
		};

		struct TexInfo
		{
			uut::Vector3 sofs;
			uut::Vector3 tofs;
			int miptex;
			int flags;
		};

		// note that edge 0 is never used, because negative edge nums are used for
		// counterclockwise use of the edge in a face
		struct Edge
		{
			unsigned short v[2];
		};

		static const int MaxLightmaps = 4;

		struct Face
		{
			short planenum;
			short side;

			int firstedge;
			short numedges;
			short texinfo;

			unsigned char styles[MaxLightmaps];
			int lightofs; // start of [numstyles*surfsize] samples
		};

		enum Ambient : unsigned short
		{
			AMBIENT_WATER,
			AMBIENT_SKY,
			AMBIENT_SLIME,
			AMBIENT_LAVA,
			NUM_AMBIENTS,
		};

		// leaf 0 is the generic CONTENTS_SOLID leaf, used for all solid areas
		// all other leafs need visibility info
		struct Leaf
		{
			int contents;
			int visofs; // -1 = no visibility info

			short mins[3]; // for frustum culling
			short maxs[3];

			unsigned short firstmarksurface;
			unsigned short nummarksurfaces;

			unsigned short ambient_level[NUM_AMBIENTS];
		};

		static const int ANGLE_UP = -1;
		static const int ANGLE_DOWN = -2;
	}
}