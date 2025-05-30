// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
#include "Layer.hpp"

namespace Cori {
	
	Layer::Layer(const std::string& name) : m_DebugName(name) {
		CORI_CORE_INFO("Layer {0} created", m_DebugName);
	}

	Layer::~Layer() {
		CORI_CORE_INFO("Layer {0} destroyed", m_DebugName);
	}

	void Layer::OnAttach() {
		CORI_CORE_INFO("Layer {0} attached", m_DebugName);
	}

	void Layer::OnDetach() {
		CORI_CORE_INFO("Layer {0} detached", m_DebugName);
	}


}