#include "PanelConfiguration.h"

#include "Application.h"
#include "Utils/Logging.h"
#include "Modules/ModuleEditor.h"
#include "Modules/ModuleTime.h"
#include "Modules/ModuleHardwareInfo.h"
#include "Modules/ModuleWindow.h"
#include "Modules/ModuleScene.h"
#include "Modules/ModuleRender.h"
#include "Modules/ModuleCamera.h"
#include "Modules/ModuleResources.h"

#include "GL/glew.h"
#include "imgui.h"
#include "IconsForkAwesome.h"

#include "Utils/Leaks.h"

PanelConfiguration::PanelConfiguration()
	: Panel("Configuration", true) {}

void PanelConfiguration::Update() {
	ImGui::SetNextWindowDockID(App->editor->dockLeftId, ImGuiCond_FirstUseEver);
	std::string windowName = std::string(ICON_FK_COGS " ") + name;
	if (ImGui::Begin(windowName.c_str(), &enabled)) {
		// Application
		if (ImGui::CollapsingHeader("Application")) {
			if (ImGui::InputText("App name", App->appName, IM_ARRAYSIZE(App->appName))) {
				App->window->SetTitle(App->appName);
			}
			ImGui::InputText("Organization", App->organization, IM_ARRAYSIZE(App->organization));
		}

		// Time
		if (ImGui::CollapsingHeader("Time")) {
			ImGui::SliderInt("Max FPS", &App->time->maxFps, 1, 240);
			ImGui::Checkbox("Limit framerate", &App->time->limitFramerate);
			if (ImGui::Checkbox("VSync", &App->time->vsync)) {
				App->renderer->SetVSync(App->time->vsync);
			}
			ImGui::SliderInt("Step delta time (MS)", &App->time->stepDeltaTimeMs, 1, 1000);

			// FPS Graph
			char title[25];
			sprintf_s(title, 25, "Framerate %.1f", fpsLog[fpsLogIndex]);
			ImGui::PlotHistogram("##framerate", &fpsLog[0], FPS_LOG_SIZE, fpsLogIndex, title, 0.0f, 100.0f, ImVec2(310, 100));
			sprintf_s(title, 25, "Milliseconds %0.1f", msLog[fpsLogIndex]);
			ImGui::PlotHistogram("##milliseconds", &msLog[0], FPS_LOG_SIZE, fpsLogIndex, title, 0.0f, 40.0f, ImVec2(310, 100));
		}

		// Hardware
		if (ImGui::CollapsingHeader("Hardware")) {
			ImGui::Text("GLEW version:");
			ImGui::SameLine();
			ImGui::TextColored(App->editor->textColor, App->hardware->glewVersion);
			ImGui::Text("SDL version:");
			ImGui::SameLine();
			ImGui::TextColored(App->editor->textColor, App->hardware->sdlVersion);
			ImGui::Text("Assimp version:");
			ImGui::SameLine();
			ImGui::TextColored(App->editor->textColor, App->hardware->assimpVersion);
			ImGui::Text("DeviL version:");
			ImGui::SameLine();
			ImGui::TextColored(App->editor->textColor, App->hardware->devilVersion);

			ImGui::Separator();

			ImGui::Text("CPUs:");
			ImGui::SameLine();
			ImGui::TextColored(App->editor->textColor, "%i (Cache: %i kb)", App->hardware->cpuCount, App->hardware->cacheSizeKb);
			ImGui::Text("System RAM:");
			ImGui::SameLine();
			ImGui::TextColored(App->editor->textColor, "%.1f Gb", App->hardware->ramGb);
			ImGui::Text("Caps:");
			const char* items[] = {"3DNow", "ARMSIMD", "AVX", "AVX2", "AVX512F", "AltiVec", "MMX", "NEON", "RDTSC", "SSE", "SSE2", "SSE3", "SSE41", "SSE42"};
			for (int i = 0; i < IM_ARRAYSIZE(items); ++i) {
				if (App->hardware->caps[i]) {
					ImGui::SameLine();
					ImGui::TextColored(App->editor->textColor, items[i]);
				}

				// Line break to avoid too many items in the same line
				if (i == 6) {
					ImGui::Text("");
				}
			}

			ImGui::Separator();

			ImGui::Text("GPU Vendor:");
			ImGui::SameLine();
			ImGui::TextColored(App->editor->textColor, "%s", App->hardware->gpuVendor);
			ImGui::Text("GPU Renderer:");
			ImGui::SameLine();
			ImGui::TextColored(App->editor->textColor, "%s", App->hardware->gpuRenderer);
			ImGui::Text("GPU OpenGL Version:");
			ImGui::SameLine();
			ImGui::TextColored(App->editor->textColor, "%s", App->hardware->gpuOpenglVersion);
			ImGui::Text("VRAM Budget:");
			ImGui::SameLine();
			ImGui::TextColored(App->editor->textColor, "%.1f Mb", App->hardware->vramBudgetMb);
			ImGui::Text("VRAM Usage:");
			ImGui::SameLine();
			ImGui::TextColored(App->editor->textColor, "%.1f Mb", App->hardware->vramUsageMb);
			ImGui::Text("VRAM Available:");
			ImGui::SameLine();
			ImGui::TextColored(App->editor->textColor, "%.1f Mb", App->hardware->vramAvailableMb);
			ImGui::Text("VRAM Reserved:");
			ImGui::SameLine();
			ImGui::TextColored(App->editor->textColor, "%.1f Mb", App->hardware->vramReservedMb);
		}

		// Window
		if (ImGui::CollapsingHeader("Window")) {
			// Window mode combo box
			const char* items[] = {"Windowed", "Borderless", "Fullscreen", "Fullscreen desktop"};
			const char* itemCurrent = items[int(App->window->GetWindowMode())];
			if (ImGui::BeginCombo("Window mode", itemCurrent)) {
				for (int n = 0; n < IM_ARRAYSIZE(items); ++n) {
					bool isSelected = (itemCurrent == items[n]);
					if (ImGui::Selectable(items[n], isSelected)) {
						App->window->SetWindowMode(WindowMode(n));
					}
					if (isSelected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}

			float brightness = App->window->GetBrightness();
			if (ImGui::SliderFloat("Brightness", &brightness, 0.25f, 1.0f)) {
				App->window->SetBrightness(brightness);
			}

			if (App->window->GetWindowMode() == WindowMode::BORDERLESS || App->window->GetWindowMode() == WindowMode::WINDOWED) {
				bool resizable = App->window->GetResizable();
				if (ImGui::Checkbox("Resizable", &resizable)) {
					App->window->SetResizable(resizable);
				}
				if (resizable) {
					bool sizeChanged = false;
					bool sizeChanging = false;
					ImGui::SliderInt("Width", &windowWidth, 640, 4096);
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						sizeChanged = true;
					}
					if (ImGui::IsItemActive()) {
						sizeChanging = true;
					}
					ImGui::SliderInt("Height", &windowHeight, 480, 2160);
					if (ImGui::IsItemDeactivatedAfterEdit()) {
						sizeChanged = true;
					}
					if (ImGui::IsItemActive()) {
						sizeChanging = true;
					}

					if (sizeChanged) {
						App->window->SetSize(windowWidth, windowHeight);
					} else if (!sizeChanging) {
						windowWidth = App->window->GetWidth();
						windowHeight = App->window->GetHeight();
					}
				}
			} else {
				int currentDisplayModeIndex = App->window->GetCurrentDisplayMode();
				const SDL_DisplayMode& currentDisplayMode = App->window->displayModes[currentDisplayModeIndex];
				char currentDisplayModeLabel[40];
				sprintf_s(currentDisplayModeLabel, " %i bpp\t%i x %i @ %iHz", SDL_BITSPERPIXEL(currentDisplayMode.format), currentDisplayMode.w, currentDisplayMode.h, currentDisplayMode.refresh_rate);

				if (ImGui::BeginCombo("Display Modes", currentDisplayModeLabel)) {
					int displayModeIndex = 0;
					for (const SDL_DisplayMode& displayMode : App->window->displayModes) {
						bool isSelected = (currentDisplayModeIndex == displayModeIndex);
						char displayModeLabel[40];
						sprintf_s(displayModeLabel, " %i bpp\t%i x %i @ %iHz", SDL_BITSPERPIXEL(displayMode.format), displayMode.w, displayMode.h, displayMode.refresh_rate);

						if (ImGui::Selectable(displayModeLabel, isSelected)) {
							App->window->SetCurrentDisplayMode(displayModeIndex);
						}

						displayModeIndex += 1;
					}
					ImGui::EndCombo();
				}
			}
		}

		// Scene
		if (ImGui::CollapsingHeader("Scene")) {
			// TODO: Change the Skybox images
			ImGui::TextColored(App->editor->titleColor, "Gizmos");
			ImGui::Checkbox("Draw Bounding Boxes", &App->renderer->drawAllBoundingBoxes);
			ImGui::Checkbox("Draw Quadtree", &App->renderer->drawQuadtree);
			ImGui::Separator();
			ImGui::InputFloat2("Min Point", App->scene->quadtreeBounds.minPoint.ptr());
			ImGui::InputFloat2("Max Point", App->scene->quadtreeBounds.maxPoint.ptr());
			ImGui::InputScalar("Max Depth", ImGuiDataType_U32, &App->scene->quadtreeMaxDepth);
			ImGui::InputScalar("Elements Per Node", ImGuiDataType_U32, &App->scene->quadtreeElementsPerNode);
			if (ImGui::Button("Clear Quadtree")) {
				App->scene->ClearQuadtree();
			}
			ImGui::SameLine();
			if (ImGui::Button("Rebuild Quadtree")) {
				App->scene->RebuildQuadtree();
			}
			ImGui::Separator();

			ImGui::Checkbox("Skybox", &App->renderer->skyboxActive);
			ImGui::ColorEdit3("Background", App->renderer->clearColor.ptr());
			ImGui::ColorEdit3("Ambient Color", App->renderer->ambientColor.ptr());
		}

		// Camera
		if (ImGui::CollapsingHeader("Engine Camera")) {
			Frustum& frustum = App->camera->GetEngineFrustum();
			vec front = frustum.Front();
			vec up = frustum.Up();
			ImGui::TextColored(App->editor->titleColor, "Frustum");
			ImGui::InputFloat3("Front", front.ptr(), "%.3f", ImGuiInputTextFlags_ReadOnly);
			ImGui::InputFloat3("Up", up.ptr(), "%.3f", ImGuiInputTextFlags_ReadOnly);

			float nearPlane = frustum.NearPlaneDistance();
			float farPlane = frustum.FarPlaneDistance();
			if (ImGui::DragFloat("Near Plane", &nearPlane, 0.1f, 0.0f, farPlane, "%.2f")) {
				App->camera->engineCameraFrustum.SetViewPlaneDistances(nearPlane, farPlane);
			}
			if (ImGui::DragFloat("Far Plane", &farPlane, 1.0f, nearPlane, inf, "%.2f")) {
				App->camera->engineCameraFrustum.SetViewPlaneDistances(nearPlane, farPlane);
			}
		}
	}
	ImGui::End();
}
