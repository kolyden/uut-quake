#include "Q1Utils.h"
#include <Core/Math/Quaternion.h>
#include <Core/Math/Matrix4.h>
#include <Core/IO/BinaryReader.h>
#include <Video/Renderer.h>
#include "Constants.h"

namespace uutQ1
{
	uut::SPtr<uut::Texture2D> Q1Utils::LoadTexture(const uut::SPtr<uut::Stream>& stream, const uut::IntVector2& size)
	{
		auto reader = uut::MakeShared<uut::BinaryReader>(stream);

		uut::List<uint8_t> data;
		data.SetSize(size.Area());
		reader->ReadBytes(data.GetDataSize(), data.GetData());

		return LoadTexture(data.GetData(), size);
	}

	uut::SPtr<uut::Texture2D> Q1Utils::LoadTexture(const void* data, const uut::IntVector2& size)
	{
		auto renderer = uut::Renderer::Instance();
		auto texture = renderer->CreateTexture(size, uut::TextureAccess::Streaming);
		int pitch;
		auto bytes = (uint8_t*)texture->Lock(&pitch);
		if (bytes != nullptr)
		{
			for (int y = 0; y < size.y; y++)
			{
				for (int x = 0; x < size.x; x++)
				{
					const uint8_t index = ((uint8_t*)data)[x + y*size.x];
					const uint32_t offset = x * 4 + y * pitch;
					uint32_t* target = (uint32_t*)(bytes + offset);
// 						if (index != 0xFF)
					target[0] = g_palette[index].ToInt();
// 						else target[0] = Color32::Transparent.ToInt();
				}
			}
			texture->Unlock();
		}

		return texture;
	}

	uut::Vector3 Q1Utils::ConvertFrom(const uut::Vector3& vec)
	{
		static const auto rot = uut::Quaternion::RotationAxis(uut::Vector3::AxisX, -uut::Degree::Angle90) *
			uut::Quaternion::RotationAxis(uut::Vector3::AxisY, uut::Degree::Angle90);
		static const uut::Matrix4 mat = uut::Matrix4::RotationQuaternion(rot);

		return mat.VectorTransform(vec);
	}
}