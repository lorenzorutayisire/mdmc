#include "render_doc.hpp"

#include <iostream>

RenderDoc::RenderDoc()
{
	if (HMODULE module = GetModuleHandleA("renderdoc.dll"))
	{
		pRENDERDOC_GetAPI RENDERDOC_GetAPI = (pRENDERDOC_GetAPI) GetProcAddress(module, "RENDERDOC_GetAPI");
		int result = RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_1_2, (void**) &handle);
		if (result != 1)
		{
			std::cerr << "RenderDoc failed to initialize, result: " << result << std::endl;
			return;
		}
	}
}

void RenderDoc::capture(std::function<void(void)> const& code)
{
	if (this->handle)
		this->handle->StartFrameCapture(NULL, NULL);
	
	code();

	if (this->handle)
		this->handle->EndFrameCapture(NULL, NULL);
}
