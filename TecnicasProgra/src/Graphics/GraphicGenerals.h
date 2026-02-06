#pragma once

namespace GAPI_FORMAT
{
	enum K
	{
		FORMAT_UNKNOWN = 0,
		FORMAT_D24_UNORM_S8_UINT,
        FORMAT_R8G8B8A8_UNORM
	};
};

namespace GAPI_BIND_FLAGS
{
    enum K
    {
        BIND_VERTEX_BUFFER = (1<<0),
        BIND_INDEX_BUFFER = (1<<1),
        BIND_CONSTANT_BUFFER = (1<<2),
        SHADER_RESOURCE = (1<<3),
        RENDER_TARGET = (1<<4),
        DEPTH_STENCIL = (1<<5)
    };
};