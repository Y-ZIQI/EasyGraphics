#pragma once

#include "function/global/global.h"

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