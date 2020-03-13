#pragma once

#include <iostream>

#define NOMINMAX
#include <windows.h>
#include <renderdoc/renderdoc_app.h>

struct RenderDoc
{
	const RENDERDOC_API_1_1_2* handle;

	RenderDoc(RENDERDOC_API_1_1_2* handle) : handle(handle) {}

	void start_capture()
	{
		if (this->handle)
		{
			std::cout << "*** RenderDoc > Starting capture" << std::endl;
			this->handle->StartFrameCapture(NULL, NULL);
		}
	}

	void end_capture()
	{
		if (this->handle)
		{
			this->handle->EndFrameCapture(NULL, NULL);
			std::cout << "*** RenderDoc > Ending capture" << std::endl;
		}
	}

	static RenderDoc init()
	{
		RENDERDOC_API_1_1_2* handle = NULL;

		if (HMODULE module = GetModuleHandleA("renderdoc.dll"))
		{
			pRENDERDOC_GetAPI RENDERDOC_GetAPI = (pRENDERDOC_GetAPI)GetProcAddress(module, "RENDERDOC_GetAPI");
			int result = RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_1_2, (void**) &handle);
			assert(result == 1);
		}

		return RenderDoc(handle);
	}
};
