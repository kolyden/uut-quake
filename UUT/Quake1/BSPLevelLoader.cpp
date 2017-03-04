#include "BSPLevelLoader.h"
#include <Core/Debug.h>
#include <Core/Math/Math.h>
#include <Core/Math/Vector3.h>
#include <Core/IO/Stream.h>
#include <Core/IO/BinaryReader.h>
#include <Video/Mesh.h>
#include "BSPLevel.h"

namespace uutQ1
{
	UUT_OBJECT_IMPLEMENT(BSPLevelLoader)
	{}

// 	struct dentry_t                // A Directory entry
// 	{
// 		long  offset;                // Offset to entry, in bytes, from start of file
// 		long  size;                  // Size of entry in file, in bytes
// 	};
// 
// 	struct dheader_t                // The BSP file header
// 	{
// 		long  version;               // Model version, must be 0x17 (23).
// 		dentry_t entities;           // List of Entities.
// 		dentry_t planes;             // Map Planes.
// 									 // numplanes = size/sizeof(plane_t)
// 		dentry_t miptex;             // Wall Textures.
// 		dentry_t vertices;           // Map Vertices.
// 									 // numvertices = size/sizeof(vertex_t)
// 		dentry_t visilist;           // Leaves Visibility lists.
// 		dentry_t nodes;              // BSP Nodes.
// 									 // numnodes = size/sizeof(node_t)
// 		dentry_t texinfo;            // Texture Info for faces.
// 									 // numtexinfo = size/sizeof(texinfo_t)
// 		dentry_t faces;              // Faces of each surface.
// 									 // numfaces = size/sizeof(face_t)
// 		dentry_t lightmaps;          // Wall Light Maps.
// 		dentry_t clipnodes;          // clip nodes, for Models.
// 									 // numclips = size/sizeof(clipnode_t)
// 		dentry_t leaves;             // BSP Leaves.
// 									 // numlaves = size/sizeof(leaf_t)
// 		dentry_t lface;              // List of Faces.
// 		dentry_t edges;              // Edges of faces.
// 									 // numedges = Size/sizeof(edge_t)
// 		dentry_t ledges;             // List of Edges.
// 		dentry_t models;             // List of Models.
// 									 // nummodels = Size/sizeof(model_t)
// 	};
// 
// 	typedef struct                 // Bounding Box, Float values
// 	{
// 		Vector3   min;                // minimum values of X,Y,Z
// 		Vector3   max;                // maximum values of X,Y,Z
// 	} boundbox_t;
// 
// 	typedef struct                 // Bounding Box, Short values
// 	{
// 		short   min;                 // minimum values of X,Y,Z
// 		short   max;                 // maximum values of X,Y,Z
// 	} bboxshort_t;
// 
// 	typedef struct                 // Mip Texture
// 	{
// 		char   name[16];             // Name of the texture.
// 		uint32_t width;                // width of picture, must be a multiple of 8
// 		uint32_t height;               // height of picture, must be a multiple of 8
// 		uint32_t offset1;              // offset to u_char Pix[width   * height]
// 		uint32_t offset2;              // offset to u_char Pix[width/2 * height/2]
// 		uint32_t offset4;              // offset to u_char Pix[width/4 * height/4]
// 		uint32_t offset8;              // offset to u_char Pix[width/8 * height/8]
// 	} miptex_t;
// 
// 	typedef struct
// 	{
// 		boundbox_t bound;            // The bounding box of the Model
// 		Vector3 origin;               // origin of model, usually (0,0,0)
// 		long node_id0;               // index of first BSP node
// 		long node_id1;               // index of the first Clip node
// 		long node_id2;               // index of the second Clip node
// 		long node_id3;               // usually zero
// 		long numleafs;               // number of BSP leaves
// 		long face_id;                // index of Faces
// 		long face_num;               // number of Faces
// 	} model_t;
// 
// 	typedef struct
// 	{
// 		uint16_t vertex0;             // index of the start vertex
// 									 //  must be in [0,numvertices[
// 		uint16_t vertex1;             // index of the end vertex
// 									 //  must be in [0,numvertices[
// 	} edge_t;
// 
// 	typedef struct
// 	{
// 		uint16_t plane_id;            // The plane in which the face lies
// 									 //           must be in [0,numplanes[ 
// 		uint16_t side;                // 0 if in front of the plane, 1 if behind the plane
// 		long ledge_id;               // first edge in the List of edges
// 									 //           must be in [0,numledges[
// 		uint16_t ledge_num;           // number of edges in the List of edges
// 		uint16_t texinfo_id;          // index of the Texture info the face is part of
// 									 //           must be in [0,numtexinfos[ 
// 		uint8_t typelight;            // type of lighting, for the face
// 		uint8_t baselight;            // from 0xFF (dark) to 0 (bright)
// 		uint8_t light[2];             // two additional light models  
// 		long lightmap;               // Pointer inside the general light map, or -1
// 									 // this define the start of the face light map
// 	} face_t;

	SharedPtr<Resource> BSPLevelLoader::Load(const SharedPtr<Stream>& stream)
	{
		auto reader = SharedPtr<BinaryReader>::Make(stream);

		uutQ1::bsp::Header header;
		if (!reader->Read(header))
			return nullptr;

		if (header.version != uutQ1::bsp::HEADER_ID)
		{
			Debug::LogError("Invalid BSP Version");
			return nullptr;
		}

		auto level = MakeShared<BSPLevel>();

		// Load Textures
		stream->SetPosition(header.lumps[uutQ1::bsp::LUMP_TEXTURES].fileofs);
		long numtex = reader->ReadInt32();
		List<int32_t> texoffsets;
		texoffsets.SetSize(numtex);
		for (int i = 0; i < numtex; i++)
			texoffsets[i] = reader->ReadInt32();

		for (int i = 0; i < numtex; i++)
		{
			stream->SetPosition(header.lumps[uutQ1::bsp::LUMP_TEXTURES].fileofs + texoffsets[i]);
			uutQ1::bsp::MipTex texheader;
			if (!reader->Read(texheader))
				continue;

			stream->SetPosition(header.lumps[uutQ1::bsp::LUMP_TEXTURES].fileofs + texoffsets[i] + texheader.offsets[0]);
			auto tex = uutQ1::Q1Utils::LoadTexture(stream, IntVector2(texheader.width, texheader.height));
			level->_textures.Add(texheader.name, tex);
		}		

		return level;
	}
}