#pragma once

#include "engine.h"

namespace Eagle
{
	class Viewer {
	public:
		Viewer();
		~Viewer();

		void run();

		std::shared_ptr<EagleEngine> m_engine;
	};
}