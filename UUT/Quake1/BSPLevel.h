#pragma once
#include <Resources/Resource.h>
#include <Core/Collections/Dictionary.h>
#include <Core/HashString.h>
#include "Q1Utils.h"
#include "BSP.h"

namespace uut
{
	class Mesh;
	class Texture2D;
}

namespace uutQ1
{
	class BSPLevel : public Resource
	{
		UUT_OBJECT(uut, BSPLevel, Resource)
	public:
		const List<SharedPtr<Mesh>>& GetMeshes() const { return _meshes; }
		const Dictionary<HashString, SharedPtr<Texture2D>>& GetTextures() const { return _textures; }

	protected:
		List<uutQ1::bsp::Model> _models;
		List<uint8_t> _visdata;
		List<uint8_t> _lightdata;
		List<uutQ1::bsp::Leaf> _leafs;
		List<uutQ1::bsp::Plane> _planes;
		List<Vector3> _vertexes;
		List<uutQ1::bsp::Node> _nodes;
		List<uutQ1::bsp::TexInfo> _texinfo;
		List<uutQ1::bsp::Face> _faces;
		List<uutQ1::bsp::ClipNode> _clipnodes;
		List<uutQ1::bsp::Edge> _edges;
		List<unsigned short> _marksurfaces;
		List<int> _surfedges;

		List<SharedPtr<Mesh>> _meshes;
		Dictionary<HashString, SharedPtr<Texture2D>> _textures;

		friend class BSPLevelLoader;
	};
}