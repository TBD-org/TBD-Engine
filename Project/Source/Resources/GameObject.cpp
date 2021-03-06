#include "GameObject.h"

#include "Globals.h"
#include "Components/ComponentType.h"

#include "Math/myassert.h"
#include "rapidjson/document.h"

#include "Utils/Leaks.h"

#define JSON_TAG_ID "Id"
#define JSON_TAG_NAME "Name"
#define JSON_TAG_ACTIVE "Active"
#define JSON_TAG_PARENT_ID "ParentId"
#define JSON_TAG_TYPE "Type"
#define JSON_TAG_COMPONENTS "Components"

void GameObject::Init() {
	id = GenerateUID();
}

void GameObject::InitComponents() {
	for (Component* component : components) {
		component->Init();
	}
}

void GameObject::Update() {
	for (Component* component : components) {
		component->Update();
	}

	for (GameObject* child : children) {
		child->Update();
	}
}

void GameObject::DrawGizmos() {
	for (Component* component : components) {
		component->DrawGizmos();
	}

	for (GameObject* child : children) {
		child->DrawGizmos();
	}
}

void GameObject::Enable() {
	active = true;
}

void GameObject::Disable() {
	active = false;
}

bool GameObject::IsActive() const {
	return active;
}

UID GameObject::GetID() {
	return id;
}

void GameObject::RemoveComponent(Component* toRemove) {
	for (Component* component : components) {
		if (component == toRemove) {
			components.erase(std::remove(components.begin(), components.end(), toRemove), components.end());
		}
	}
}

void GameObject::SetParent(GameObject* gameObject) {
	if (parent != nullptr) {
		bool found = false;
		for (std::vector<GameObject*>::iterator it = parent->children.begin(); it != parent->children.end(); ++it) {
			if (*it == this) {
				found = true;
				parent->children.erase(it);
				break;
			}
		}
		assert(found);
	}
	parent = gameObject;
	if (gameObject != nullptr) {
		gameObject->children.push_back(this);
	}
}

GameObject* GameObject::GetParent() const {
	return parent;
}

void GameObject::AddChild(GameObject* gameObject) {
	gameObject->SetParent(this);
}

void GameObject::RemoveChild(GameObject* gameObject) {
	gameObject->SetParent(nullptr);
}

const std::vector<GameObject*>& GameObject::GetChildren() const {
	return children;
}

bool GameObject::IsDescendantOf(GameObject* gameObject) {
	if (GetParent() == nullptr) return false;
	if (GetParent() == gameObject) return true;
	return GetParent()->IsDescendantOf(gameObject);
}

void GameObject::Save(JsonValue jGameObject) const {
	jGameObject[JSON_TAG_ID] = id;
	jGameObject[JSON_TAG_NAME] = name.c_str();
	jGameObject[JSON_TAG_ACTIVE] = active;
	jGameObject[JSON_TAG_PARENT_ID] = parent != nullptr ? parent->id : 0;

	JsonValue jComponents = jGameObject[JSON_TAG_COMPONENTS];
	for (unsigned i = 0; i < components.size(); ++i) {
		JsonValue jComponent = jComponents[i];
		Component& component = *components[i];

		jComponent[JSON_TAG_TYPE] = (unsigned) component.GetType();
		jComponent[JSON_TAG_ACTIVE] = component.IsActive();
		component.Save(jComponent);
	}
}

void GameObject::Load(JsonValue jGameObject) {
	id = jGameObject[JSON_TAG_ID];
	name = jGameObject[JSON_TAG_NAME];
	active = jGameObject[JSON_TAG_ACTIVE];

	JsonValue jComponents = jGameObject[JSON_TAG_COMPONENTS];
	for (unsigned i = 0; i < jComponents.Size(); ++i) {
		JsonValue jComponent = jComponents[i];

		ComponentType type = (ComponentType)(unsigned) jComponent[JSON_TAG_TYPE];
		bool active = jComponent[JSON_TAG_ACTIVE];

		Component* component = CreateComponentByType(*this, type);
		component->Load(jComponent);
	}

	isInQuadtree = false;
}

void GameObject::PostLoad(JsonValue jGameObject) {
	UID parentId = jGameObject[JSON_TAG_PARENT_ID];
	GameObject* parent = App->scene->GetGameObject(parentId);
	SetParent(parent);
}
