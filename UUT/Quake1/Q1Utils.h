#pragma once
#include <Core/Collections/List.h>
#include <Core/IO/Stream.h>
#include <Core/Math/Vector3.h>
#include <Core/Math/IntVector2.h>
#include <Video/Texture2D.h>

namespace uutQ1
{
	using namespace uut;

	class Q1Utils
	{
	public:
		static uut::SPtr<uut::Texture2D> LoadTexture(const uut::SPtr<uut::Stream>& stream, const uut::IntVector2& size);
		static uut::SPtr<uut::Texture2D> LoadTexture(const void* data, const uut::IntVector2& size);

		static uut::Vector3 ConvertFrom(const uut::Vector3& vec);
	};
}