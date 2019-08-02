#include "scene.h"

#include "resource_manager.h"

namespace scythe {

	Scene::Scene()
	: attached_scene_(nullptr)
	{

	}
	Scene::~Scene()
	{

	}
	void Scene::Attach(Scene * scene)
	{
		attached_scene_ = scene;
	}
	void Scene::Detach()
	{
		attached_scene_ = nullptr;
	}
	Scene * Scene::attached_scene()
	{
		return attached_scene_;
	}
	void Scene::UpdatePhysics(float sec)
	{

	}
	void Scene::Load()
	{

	}
	void Scene::Unload()
	{

	}
	void Scene::RequestLoad()
	{
		ResourceManager * resource_manager = ResourceManager::GetInstance();
		for (auto resource_id : resources_)
			resource_manager->RequestLoad(resource_id);
	}
	void Scene::RequestUnload()
	{
		ResourceManager * resource_manager = ResourceManager::GetInstance();
		for (auto resource_id : resources_)
			resource_manager->RequestUnload(resource_id);
	}
	ResourceID Scene::AddResourceIdByName(StringID string_id)
	{
		ResourceManager * resource_manager = ResourceManager::GetInstance();
		ResourceID id = resource_manager->GetResourceIdByName(string_id);
		resources_.push_back(id);
		return id;
	}

} // namespace scythe