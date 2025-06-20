#pragma once
#include "EventSystem/Event.hpp"
#include "Renderer/GraphicsAPIs.hpp"

namespace Cori {

	struct WindowProperties {
		WindowProperties(const std::string& title = "Cori",
			unsigned int width = 1920,
			unsigned int height = 1080,
			GraphicsAPIs api = GraphicsAPIs::OpenGL
			)
			: Title(title), Width(width), Height(height), API(api) {
		}

		std::string Title;
		unsigned int Width;
		unsigned int Height;
		GraphicsAPIs API;

	};

	class Window {

	public:

		virtual ~Window() {}

		virtual void OnUpdate() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;
		
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual GraphicsAPIs GetAPI() const = 0;

		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeContext() const = 0;
		virtual void* GetNativeWindow() const = 0;
		
		static std::unique_ptr<Window> Create(const WindowProperties& props = WindowProperties());
	};


}
