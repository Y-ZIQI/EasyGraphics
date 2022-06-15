#include "include/viewer.h"

namespace Eagle
{
	Viewer::Viewer(){
		m_engine = std::make_shared<EagleEngine>();
		m_engine->initialize();
	}

	Viewer::~Viewer(){
		m_engine->cleanup();
		m_engine.reset();
	}

	void Viewer::run() {
		m_engine->mainLoop();
	}
}