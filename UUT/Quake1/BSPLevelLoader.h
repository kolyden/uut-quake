#pragma once
#include <Resources/ResourceLoader.h>
#include "Q1Utils.h"
#include "BSP.h"

namespace uutQ1
{
	class BSPLevel;

	class BSPLevelLoader : public ResourceLoaderImpl<BSPLevel>
	{
		UUT_OBJECT(uutQ1, BSPLevelLoader, ResourceLoader)
	public:
		SharedPtr<Resource> Load(const SharedPtr<Stream>& stream) override;

		template<typename T>
		static void LoadLump(const uutQ1::bsp::Lump& lump, List<T>& list, const SPtr<Stream>& stream)
		{
			stream->SetPosition(lump.fileofs);
			list.SetSize(lump.filelen / sizeof(T));
			stream->Read(list.GetData(), list.GetDataSize());
		}
	};
}