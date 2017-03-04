#include "Quake1Plugin.h"
#include <Core/Context.h>
#include "Quake1ModelLoader.h"
#include "Quake1Model.h"
#include "BSPLevel.h"
#include "BSPLevelLoader.h"

namespace uutQ1
{
	Quake1Plugin::Quake1Plugin()
		: Plugin("Quake", Version(0, 1))
	{
	}

	void Quake1Plugin::OnRegister()
	{
		UUT_REGISTER_OBJECT(Quake1ModelLoader);
		UUT_REGISTER_OBJECT(Quake1Model);
		UUT_REGISTER_OBJECT(BSPLevel);
		UUT_REGISTER_OBJECT(BSPLevelLoader);
	}
}