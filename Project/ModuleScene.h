#pragma once

#include "Module.h"
#include "GameObject.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "UID.h"
#include "Pool.h"
#include "Quadtree.h"

#include <unordered_map>
#include <string>

class CubeMap;
struct aiScene;
struct aiNode;

class ModuleScene : public Module
{
public:
	bool Init() override;
	bool Start() override;
	UpdateStatus Update() override;
	bool CleanUp() override;

	void CreateEmptyScene();
	void ClearScene();
	void RebuildQuadtree(const AABB2D& bounds, unsigned max_depth, unsigned elements_per_node);

	GameObject* CreateGameObject(GameObject* parent);
	GameObject* DuplicateGameObject(GameObject* parent);
	void DestroyGameObject(GameObject* game_object);
	GameObject* GetGameObject(UID id) const;

public:
	std::string file_name = "";
	GameObject* root = nullptr;

	Pool<GameObject> game_objects;
	std::unordered_map<UID, GameObject*> game_objects_id_map;

	// Quadtree
	Quadtree<GameObject> quadtree;
	AABB2D quadtree_bounds = AABB2D({-100, -100}, {100, 100});
	unsigned quadtree_max_depth = 8;
	unsigned quadtree_elements_per_node = 4;

	// Skybox
	unsigned skybox_vao = 0;
	unsigned skybox_vbo = 0;
	CubeMap* skybox_cube_map = 0;
};
