#include "Scene.hpp"
#include "../Renderer/Renderer2D.hpp"
#include "../Renderer/CameraController.hpp"

namespace Cori {

	Scene::Scene(const std::string& name) : m_Name(name){
		AddContextComponent<CameraContextComponent>();
		ActiveCamera.BindCameraComponent(&GetContextComponent<CameraContextComponent>());
		CORI_CORE_DEBUG("Scene: '{0}' created.", m_Name);
		auto renderGroup = m_Registry.group<PositionComponent, RenderingComponent, SpriteComponent>();
	}

	Scene::~Scene() {
		m_Registry.clear();
		CORI_CORE_DEBUG("Scene: '{0}' destroyed.", m_Name);
	}

	Entity Scene::CreateEntity(const std::string& name) {
		CORI_CORE_ASSERT_FATAL(this != nullptr, "Called scene instance is null (instance pointer = nullptr), this causes undefined behavior, and this assert also relies on this undefined behavior and is not guarantied.");

		entt::entity entity = m_Registry.create();
		m_Registry.emplace<NameComponent>(entity, name);
		m_NamedEntities.insert({ name, entt::handle{m_Registry, entity} });
		CORI_CORE_TRACE("Created Named Entity With ID: {0}, Version: {1}, Name: {2}",entt::to_integral(entity), entt::to_version(entity), name);
		return Entity{ {m_Registry, entity} };
	}

	Entity Scene::CreateEntity() {
		entt::entity entity = m_Registry.create();
		//CORI_CORE_TRACE("Created Unnamed Entity With ID: {0}, Version: {1}", entt::to_integral(entity), entt::to_version(entity));
		return Entity{ {m_Registry, entity} };

	}

	Entity Scene::GetNamedEntity(const std::string& name) {
		if (CORI_CORE_ASSERT_ERROR(m_NamedEntities.contains(name), "Named entity with name '{0}' doesn't exist, returned null entity.", name)) { return Entity{}; }
		return Entity{ m_NamedEntities.at(name) };
	}


	void Scene::OnUpdate(const double deltaTime) {

		Renderer2D::BeginBatch(GetContextComponent<CameraContextComponent>().ViewProjectionMatrix);

		auto renderGroup = m_Registry.group<PositionComponent, RenderingComponent, SpriteComponent>();

		{
			CORI_PROFILE_SCOPE("Group sort");
			renderGroup.sort<SpriteComponent>([](const SpriteComponent& lhs, const SpriteComponent& rhs) {
				return reinterpret_cast<uint64_t>(lhs.Texture.get()) > reinterpret_cast<uint64_t>(rhs.Texture.get());
			});
		}

		for (auto entity : renderGroup) {
			auto [posComp, renderComp, spriteComp] = renderGroup.get<PositionComponent, RenderingComponent, SpriteComponent>(entity);
			if (renderComp.Visible) {
				Renderer2D::DrawQuad(posComp.Position, renderComp.Size, spriteComp.Texture, spriteComp.UV);
			}
		}

		Renderer2D::EndBatch();
	}

	bool Scene::OnBind(const EventCallbackFn& callback) {
		m_TriggerEventCallback = callback;
		return true;
	}

	bool Scene::OnUnbind() {
		m_TriggerEventCallback = EventCallbackFn();
		return true;
	}


}