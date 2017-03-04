#pragma once
#include <Resources/ResourceLoader.h>
#include "Q1Utils.h"
#include "Quake1Model.h"

namespace uutQ1
{
	class Quake1ModelLoader : public ResourceLoaderImpl<Quake1Model>
	{
		UUT_OBJECT(uutQ1, Quake1ModelLoader, ResourceLoader)
	public:
		SharedPtr<Resource> Load(const SharedPtr<Stream>& stream) override;
	};
}