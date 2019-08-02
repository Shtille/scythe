#include "scene_manager.h"

#include "scene.h"
#include "resource_manager.h"

namespace {
	// Required render count to bypass loading time spikes
	const int kRequiredRenderCount = 2;
}

namespace scythe {

	SceneManager::SceneManager()
	: current_scene_(nullptr)
	, next_scene_(nullptr)
	, loading_scene_(nullptr)
	, listener_(nullptr)
	, transition_phase_(0)
	, render_count_(0)
	, transition_type_(SceneTransition::kNone)
	, transition_finalization_(false)
	{

	}
	SceneManager::~SceneManager()
	{
		// Force resource manager to unload resources
		ResourceManager * resource_manager = ResourceManager::GetInstance();
		resource_manager->CleanUp();
	}
	void SceneManager::Update()
	{
		UpdateTransition();
		if (loading_scene_)
			loading_scene_->Update();
		else if (current_scene_)
			current_scene_->Update();
	}
	void SceneManager::UpdatePhysics(float sec)
	{
		if (loading_scene_)
			loading_scene_->UpdatePhysics(sec);
		else if (current_scene_)
			current_scene_->UpdatePhysics(sec);
	}
	void SceneManager::Render()
	{
		if (transition_finalization_)
		{
			++render_count_;
		}
		if (loading_scene_)
			loading_scene_->Render();
		else if (current_scene_)
			current_scene_->Render();
	}
	void SceneManager::RequestImmediateTransition(Scene * scene)
	{
		next_scene_ = scene;
		loading_scene_ = nullptr;
		listener_ = nullptr;
		transition_type_ = SceneTransition::kImmediate;
	}
	void SceneManager::RequestDeferredTransition(Scene * scene, Scene * loading_scene, SceneTransitionListener * listener)
	{
		next_scene_ = scene;
		loading_scene_ = loading_scene;
		listener_ = listener;
		transition_type_ = SceneTransition::kDeferred;
	}
	bool SceneManager::ImmediateTransition()
	{
		if (transition_finalization_)
		{
			if (render_count_ >= kRequiredRenderCount)
			{
				render_count_ = 0;
				transition_finalization_ = false;
				return true;
			}
			else
				return false;
		}

		Scene * scene;

		scene = current_scene_;
		while (scene != nullptr)
		{
			scene->RequestUnload();
			scene = scene->attached_scene();
		}
		scene = next_scene_;
		while (scene != nullptr)
		{
			scene->RequestLoad();
			scene = scene->attached_scene();
		}

		ResourceManager::GetInstance()->Perform();

		scene = current_scene_;
		while (scene != nullptr)
		{
			scene->Unload();
			scene = scene->attached_scene();
		}
		scene = next_scene_;
		while (scene != nullptr)
		{
			scene->Load();
			scene = scene->attached_scene();
		}
		// Nullify the current scene
		current_scene_ = nullptr;

		transition_finalization_ = true;
		return false;
	}
	bool SceneManager::DeferredTransition()
	{
		Scene * scene;
		ResourceManager * resource_manager = ResourceManager::GetInstance();
		switch (transition_phase_)
		{
		case 0:
			scene = current_scene_;
			while (scene != nullptr)
			{
				scene->RequestUnload();
				scene = scene->attached_scene();
			}
			scene = next_scene_;
			while (scene != nullptr)
			{
				scene->RequestLoad();
				scene = scene->attached_scene();
			}
			++transition_phase_;
			if (listener_)
				listener_->OnObtainCount(resource_manager->GetResourcesCountToProcess());
			return false;
		case 1:
			if (resource_manager->PerformStep())
				++transition_phase_;
			else if (listener_)
				listener_->OnStep();
			return false;
		case 2:
			// User overriden functions
			scene = current_scene_;
			while (scene != nullptr)
			{
				scene->Unload();
				scene = scene->attached_scene();
			}
			scene = next_scene_;
			while (scene != nullptr)
			{
				scene->Load();
				scene = scene->attached_scene();
			}
			// Nullify the current scene
			current_scene_ = nullptr;
			++transition_phase_;
			return false;
		case 3:
			if (!transition_finalization_)
				transition_finalization_ = true;
			if (render_count_ >= kRequiredRenderCount)
			{
				// SceneTransition has been completed
				if (listener_)
					listener_->OnFinish();
				transition_phase_ = 0;
				render_count_ = 0;
				transition_finalization_ = false;
				return true;
			}
			else
				return false;
		default:
			return true;
		}
	}
	void SceneManager::UpdateTransition()
	{
		if (transition_type_ != SceneTransition::kNone)
		{
			if (transition_type_ == SceneTransition::kImmediate)
			{
				if (ImmediateTransition())
				{
					current_scene_ = next_scene_;
					next_scene_ = nullptr;
					// Immediate transition doesn't use a listener
					transition_type_ = SceneTransition::kNone;
				}
			}
			else if (transition_type_ == SceneTransition::kDeferred)
			{
				if (DeferredTransition())
				{
					current_scene_ = next_scene_;
					next_scene_ = nullptr;
					loading_scene_ = nullptr;
					listener_ = nullptr;
					transition_type_ = SceneTransition::kNone;
				}
			}
			else if (transition_type_ == SceneTransition::kAsyncronous)
			{
				// Haven't been done yet
			}
		}
	}

} // namespace scythe