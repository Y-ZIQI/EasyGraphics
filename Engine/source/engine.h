#pragma once

#include "function/global/global.h"
#include "function/global/global_resource.h"

namespace Eagle
{
	class EagleEngine
	{
	public:
		EagleEngine() {};
		~EagleEngine() {};

		void initialize();
		void mainLoop();
		void cleanup();

	};
}