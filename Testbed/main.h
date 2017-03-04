#pragma once
#include <Core/Windows/Application.h>
#include <GUI/DebugGUI.h>
#include <Video/Graphics.h>

#pragma comment(lib, "SDL2/SDL2.lib")

namespace uutQ1
{
	class Quake1Model;
	class BSPLevel;
}

namespace uut
{
	using namespace uutQ1;

	class Tilemap;
	class Player;
	class FreeCamera;

	class SampleApp : public Application
	{
	public:
		SampleApp();

	protected:
		static const int texSize = 402;

		SharedPtr<FreeCamera> _camera;
		SharedPtr<Quake1Model> _model;
		SharedPtr<BSPLevel> _level;
		SharedPtr<Graphics> _graphics;

		virtual void OnInit() override;
		virtual void OnFrame() override;
	};
}