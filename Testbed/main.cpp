#include "main.h"
#include <Core/Math/Math.h>
#include <Core/Math/Quaternion.h>
#include <Core/Context.h>
#include <Core/Variant.h>
#include <Core/Enum.h>
#include <Core/EnumFlags.h>
#include <Core/Reflection/PropertyInfo.h>
#include <Windows.h>
#include <Video/BitmapFont.h>
#include <Core/Attribute.h>
#include <CES/EntityPool.h>
#include <CES/Entity.h>
#include <CES/EntityMatcher.h>
#include <Video/Color.h>
#include <Video/Sprite.h>
#include <Video/Mesh.h>
#include <Core/Time.h>
#include <GUI/GUI.h>
#include <GUI/ContextGUI.h>
#include <Video/FreeCamera.h>
#include <Quake1/Quake1Plugin.h>
#include <Quake1/Quake1ModelLoader.h>
#include <Quake1/Quake1Model.h>
#include <Quake1/BSPLevel.h>
#include <Quake1/BSPLevelLoader.h>
#include <Core/IO/JSONFile.h>
#include <Core/IO/YamlFile.h>
#include <Core/IO/YamlSerializer.h>
#include <IMGUI/imgui.h>

namespace uut
{
	
	SampleApp::SampleApp()
	{
		_windowSize = IntVector2(800, 600);
	}

	void SampleApp::OnInit()
	{
		Context::CreateModule<DebugGUI>();

		ModuleInstance<ResourceCache> cache;
		cache->AddLoader(SharedPtr<Quake1ModelLoader>::Make());
		cache->AddLoader(SharedPtr<BSPLevelLoader>::Make());

		_model = cache->Load<Quake1Model>("player.mdl");
		_level = cache->Load<BSPLevel>("start.bsp");

		_graphics = SharedPtr<Graphics>::Make(Graphics::MT_OPAQUE, Graphics::PM_3D);

		_camera = SharedPtr<FreeCamera>::Make();
		_camera->SetPosition(Vector3(8.5f, 10, -50));
	}

	static List<HashString> MakeRange(const String& prefix, int start, int end)
	{
		List<HashString> list;
		list.SetReserve(end - start + 1);
		for (int i = start; i <= end; i++)
			list.Add(String::Format("%s%d", prefix.GetData(), i).GetData());

		return list;
	}

	void SampleApp::OnFrame()
	{
		DebugGUI::Instance()->NewFrame();

		///////////////////////////////////////////////////////////////
		ImGui::SetNextWindowPos(ImVec2(350, 50), ImGuiSetCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiSetCond_FirstUseEver);
		ImGui::SetNextWindowCollapsed(true, ImGuiSetCond_FirstUseEver);
		if (ImGui::Begin("Textures"))
		{
			static bool texturesFoldout = true;
			ImGui::PushItemWidth(-1);
			texturesFoldout = GUI::Foldout("Textures", texturesFoldout);
			if (texturesFoldout)
			{
				GUI::BeginListBox("##textures");
				if (_level)
				{
					for (auto& it : _level->GetTextures())
					{
						GUI::BeginHorizontal();
						GUI::Image(it.second, it.second->GetSize());
						GUI::Text(it.first.GetData());
						GUI::EndHorizontal();
					}
				}
				GUI::EndListBox();
			}
			ImGui::PopItemWidth();
		}
		ImGui::End();

		float moveSpeed = 50.0f;
		Radian rotateSpeed = Math::PI / 2;
		if (Input::IsKey(Scancode::Space))
		{
			moveSpeed *= 4;
			rotateSpeed *= 2;
		}
		if (Input::IsKey(Scancode::A))
			_camera->MoveRight(-moveSpeed * Time::GetDeltaTime());
		if (Input::IsKey(Scancode::D))
			_camera->MoveRight(+moveSpeed * Time::GetDeltaTime());
		if (Input::IsKey(Scancode::S))
			_camera->MoveForward(-moveSpeed * Time::GetDeltaTime());
		if (Input::IsKey(Scancode::W))
			_camera->MoveForward(+moveSpeed * Time::GetDeltaTime());
		if (Input::IsKey(Scancode::Q))
			_camera->MoveUp(-moveSpeed * Time::GetDeltaTime());
		if (Input::IsKey(Scancode::E))
			_camera->MoveUp(+moveSpeed * Time::GetDeltaTime());

		if (Input::IsKey(Scancode::Left))
			_camera->Yaw(-rotateSpeed*Time::GetDeltaTime());
		if (Input::IsKey(Scancode::Right))
			_camera->Yaw(+rotateSpeed*Time::GetDeltaTime());

		if (Input::IsKey(Scancode::Up))
			_camera->Pitch(-rotateSpeed*Time::GetDeltaTime());
		if (Input::IsKey(Scancode::Down))
			_camera->Pitch(+rotateSpeed*Time::GetDeltaTime());

		///////////////////////////////////////////////////////////////
		auto renderer = Renderer::Instance();
		auto gui = DebugGUI::Instance();

		if (renderer->BeginScene())
		{
			_graphics->BeginRecord();
			_graphics->SetViewport(Viewport(0, 0, renderer->GetScreenSize()));
			_graphics->Clear(Color32(114, 144, 154));
			_graphics->SetProjection(Graphics::PM_3D);
			_graphics->SetMaterial(Graphics::MT_OPAQUE);
			_graphics->SetCamera(_camera);

			_graphics->DrawLine(Vector3::Zero, Vector3::AxisX * 100, Color32::Red);
			_graphics->DrawLine(Vector3::Zero, Vector3::AxisY * 100, Color32::Green);
			_graphics->DrawLine(Vector3::Zero, Vector3::AxisZ * 100, Color32::Blue);

			if (_model)
			{				
				static const List<HashString> anim = List<HashString>::MakeRange(1, 12,
					[](int index) -> HashString { return String::Format("axstnd%d", index).GetData(); });

				static const float frameTime = 0.15f;
				static int index = 0;
				static float time = 0;

				const Vector3 pos = Vector3(20, 0, 30);
				_graphics->DrawLine(pos, pos + Vector3::AxisY * 10, Color32::Magenta);

				static const auto rot = Quaternion::RotationAxis(Vector3::AxisX, -Degree::Angle90) *
					Quaternion::RotationAxis(Vector3::AxisY, Degree::Angle90);

				static const Matrix4 mat = Matrix4::Transformation(pos, Quaternion::Identity, Vector3::One);
				auto& frames = _model->GetFrames();
				auto it = _model->GetAnimations().Find(anim[index]);
				if (it != _model->GetAnimations().End())
					_graphics->DrawMesh(mat, frames[it->second], _model->GetSkins()[0]);

				time += Time::GetDeltaTime();
				while (time >= frameTime)
				{
					time -= frameTime;
					index = (index + 1) % anim.Count();
				}
			}

			if (_level && _level->GetMeshes().Count() > 0)
			{
				static const Matrix4 mat = Matrix4::Scaling(Vector3(0.5f));
				_graphics->DrawMesh(mat, _level->GetMeshes()[0]);
			}

			_graphics->SetMaterial(Graphics::MT_TRANSPARENT);
			_graphics->SetProjection(Graphics::PM_2D);
			_graphics->SetViewMatrix(Matrix4::Identity);

			if (_model)
			{
				const List<SharedPtr<Texture2D>>& skins = _model->GetSkins();
				int x = 0;
				for (size_t i = 0; i < skins.Count(); i++)
				{
					x += 10;
					auto& tex = skins[i];
					_graphics->DrawQuad(IntRect(x, 50, tex->GetSize()), 15, tex);
					x += tex->GetSize().x;
				}
			}
			_graphics->EndRecord();
			_graphics->Draw();

			gui->Draw();

			renderer->EndScene();
		}
	}
}

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow)
{
	uut::SampleApp app;
	app.Run();

	return 0;
}