#pragma once
#include <Core/Collections/Dictionary.h>
#include <Resources/Resource.h>
#include "Q1Utils.h"

namespace uut
{
	class Texture2D;
	class Mesh;
}

namespace uutQ1
{
	class Quake1Model : public Resource
	{
		UUT_OBJECT(uutQ1, Quake1Model, Resource)
	public:

		const List<SharedPtr<Texture2D>>& GetSkins() const { return _skins; }
		const List<SharedPtr<Mesh>>& GetFrames() const { return _frames; }
		const Dictionary<HashString, uint>& GetAnimations() const { return _animations; }

	protected:
		List<SharedPtr<Texture2D>> _skins;
		List<SharedPtr<Mesh>> _frames;
		Dictionary<HashString, uint> _animations;

		friend class Quake1ModelLoader;
	};
}