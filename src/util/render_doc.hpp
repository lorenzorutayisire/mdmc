#pragma once

#include <iostream>
#include <functional>

#define NOMINMAX
#include <windows.h>
#include <renderdoc_app.h>

#define RenderDoc_capture(x) RenderDoc().capture([]{x})

class RenderDoc
{
public:
	RENDERDOC_API_1_1_2* handle = NULL;

	RenderDoc();

	void capture(std::function<void(void)> const& code);
};
