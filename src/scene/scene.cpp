#include "scene.h"

#include "resource_manager.h"

namespace scythe {

	Scene::Scene(Renderer * renderer)
	: renderer_(renderer)
	, next_(nullptr)
	{

	}
	Scene::~Scene()
	{

	}
	void Scene::SetNextScene(Scene * scene)
	{
		next_ = scene;
	}
	Scene * Scene::next()
	{
		return next_;
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
	void Scene::OnChar(unsigned short code)
	{

	}
	void Scene::OnKeyDown(PublicKey key, int mods)
	{

	}
	void Scene::OnMouseDown(MouseButton button, int modifiers, float x, float y)
	{

	}
	void Scene::OnMouseUp(MouseButton button, int modifiers, float x, float y)
	{

	}
	void Scene::OnMouseMove(float x, float y)
	{
		
	}

} // namespace scythe