#pragma once
#include <Core/Plugin.h>
#include "Q1Utils.h"

namespace uutQ1
{
	class Quake1Plugin : public Plugin
	{
	public:
		Quake1Plugin();

	protected:
		void OnRegister() override;
	};
}