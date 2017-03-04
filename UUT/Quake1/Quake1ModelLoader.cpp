#include "Quake1ModelLoader.h"
#include <Core/Debug.h>
#include <Core/IO/BinaryReader.h>
#include <Core/IO/Stream.h>
#include <Core/Math/Quaternion.h>
#include <Video/Renderer.h>
#include <Video/Mesh.h>
#include <Video/Texture2D.h>
#include "Quake1Model.h"
#include "Constants.h"
#include "Q1Utils.h"

namespace uutQ1
{
	static constexpr uint32_t mdl_id = 1330660425;

	struct mdl_header_t
	{
		int32_t id;                     // 0x4F504449 = "IDPO" for IDPOLYGON
		int32_t version;                // Version = 6

		Vector3 scale;                // Model scale factors.
		Vector3 translate;               // Model origin.
		float boundingradius;             // Model bounding radius.
		Vector3 eyeposition;              // Eye position (useless?)

		int32_t numskins;              // the number of skin textures
		int32_t skinwidth;              // Width of skin texture must be multiple of 8
		int32_t skinheight;             // Height of skin texture must be multiple of 8

		int32_t num_verts;               // Number of vertices
		int32_t num_tris;                // Number of triangles surfaces
		int32_t num_frames;              // Number of frames

		int32_t synctype;               // 0 = synchron, 1 = random
		int32_t flags;                  // 0 (see Alias models)
		float size;               // average size of triangles
	};

	static constexpr size_t mdl_header_size = sizeof(mdl_header_t);
	static_assert(mdl_header_size == 84, "Invalid header size");

	struct mdl_texcoord_t
	{
		int onseam;
		int s;
		int t;
	};

	struct mdl_triangle_t
	{
		int facesfront;  /* 0 = backface, 1 = frontface */
		int vertex[3];   /* vertex indices */
	};

	/* Compressed vertex */
	struct mdl_vertex_t
	{
		unsigned char v[3];
		unsigned char normalIndex;
	};

	struct mdl_simpleframe_t
	{
		struct mdl_vertex_t bboxmin; /* bouding box min */
		struct mdl_vertex_t bboxmax; /* bouding box max */
		char name[16];
	};

	UUT_OBJECT_IMPLEMENT(Quake1ModelLoader)
	{}

	SharedPtr<Resource> Quake1ModelLoader::Load(const SharedPtr<Stream>& stream)
	{
		auto reader = SharedPtr<BinaryReader>::Make(stream);

		mdl_header_t header;
		if (reader->ReadBytes(mdl_header_size, &header) != mdl_header_size)
			return nullptr;

		if ((header.id != mdl_id) || (header.version != 6))
			return nullptr;

// 		if (((header.skinwidth % 8) != 0) || ((header.skinheight % 8) != 0))
// 			return nullptr;

		auto model = SharedPtr<Quake1Model>::Make();

		// Read texture data
		List<uint8_t> data;
		data.SetSize(header.skinwidth * header.skinheight);
		for (int i = 0; i < header.numskins; i++)
		{
			const int32_t type = reader->ReadUint32();
			int32_t count;
			switch (type)
			{
			case 0:
				reader->ReadBytes(data.GetDataSize(), data.GetData());
				break;

			case 1:
				count = reader->ReadUint32();
				reader->SkipBytes(sizeof(float) * count);
				reader->ReadBytes(data.GetDataSize(), data.GetData());
				reader->SkipBytes(header.skinwidth * header.skinheight * (count - 1));
				break;

			default:
				return nullptr;
			}

			auto skin = uutQ1::Q1Utils::LoadTexture(data.GetData(), IntVector2(header.skinwidth, header.skinheight));
			model->_skins.Add(skin);
		}
		data.Clear();

		// Texture coordinates
		List<mdl_texcoord_t> mdl_tex;
		mdl_tex.SetSize(header.num_verts);
		reader->ReadBytes(mdl_tex.GetDataSize(), mdl_tex.GetData());

		// Triangles
		List<mdl_triangle_t> mdl_tri;
		mdl_tri.SetSize(header.num_tris);
		reader->ReadBytes(mdl_tri.GetDataSize(), mdl_tri.GetData());

		// Frames
		List<mdl_vertex_t> mdl_vert;
		mdl_vert.SetSize(header.num_verts);
		List<Vector3> vertices;
		List<Vector2> uv;
		List<Color32> colors;
		List<size_t> indexes;

		vertices.SetSize(header.num_tris * 3);
		uv.SetSize(header.num_tris * 3);
		colors.SetSize(header.num_tris * 3);
		indexes.SetSize(header.num_tris * 3);

		for (int i = 0; i < header.num_tris * 3; i++)
		{
			indexes[i] = i;
			colors[i] = Color32::White;
		}

		for (int i = 0; i < header.num_frames; i++)
		{
			mdl_simpleframe_t frame;

			const int32_t type = reader->ReadInt32();
			if (type == 0)
			{
				reader->ReadBytes(sizeof(mdl_simpleframe_t), &frame);
				reader->ReadBytes(mdl_vert.GetDataSize(), mdl_vert.GetData());
				model->_animations.Add(frame.name, model->_frames.Count());
			}
			else
			{
				Debug::LogWarning("Can't load Quake 1 MDL with group frames");
				return nullptr;
			}

			auto mesh = SharedPtr<Mesh>::Make();

			for (int i = 0; i < header.num_tris; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					const int index = i * 3 + j;

					const mdl_vertex_t* pvert = &mdl_vert[mdl_tri[i].vertex[j]];

					float s = 1.0f * mdl_tex[mdl_tri[i].vertex[j]].s;
					float t = 1.0f * mdl_tex[mdl_tri[i].vertex[j]].t;

					if (!mdl_tri[i].facesfront && mdl_tex[mdl_tri[i].vertex[j]].onseam)
						s += 0.5f*header.skinwidth;

					const float tx = (s + 0.5f) / header.skinwidth;
					const float ty = (t + 0.5f) / header.skinheight;
					uv[index] = Vector2(tx, ty);

					const Vector3 normal = uutQ1::g_normals[pvert->normalIndex];

					const float x = (header.scale[0] * pvert->v[0]) + header.translate[0];
					const float y = (header.scale[1] * pvert->v[1]) + header.translate[1];
					const float z = (header.scale[2] * pvert->v[2]) + header.translate[2];
					vertices[index] = uutQ1::Q1Utils::ConvertFrom(Vector3(x, -y, z));
				}
			}

			mesh->SetVertices(vertices);
			mesh->SetUV(uv);
			mesh->SetColors32(colors);
			mesh->SetIndexes(indexes);

			model->_frames << mesh;
		}

		return model;
	}
}