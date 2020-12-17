#pragma once

#include "Component.h"

enum class ShaderType
{
	STANDARD,
	PHONG
};

class ComponentMaterial : public Component
{
public:
	static const ComponentType static_type = ComponentType::MATERIAL;

	ComponentMaterial(GameObject& owner);

public:
	unsigned texture = 0;
	ShaderType material_type = ShaderType::PHONG;

	// Phong
	float Kd = 1;
	float Ks = 0;
	int n = 1;


};
