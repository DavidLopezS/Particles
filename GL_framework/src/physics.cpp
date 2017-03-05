#include <imgui\imgui.h>
#include <imgui\imgui_impl_glfw_gl3.h>

struct Particles {
	float pos[3];
	float speed[3];
	float lifeEx;
};

bool show_test_window = true;
void GUI() {
	{	//FrameRate
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		//TODO
	}

	// ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
	if(show_test_window) {
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
		ImGui::ShowTestWindow(&show_test_window);
	}
}

void ParticlesGenerator() {

}

void PhysicsInit() {
	//TODO
}
void PhysicsUpdate(float dt) {
	//TODO
}
void PhysicsCleanup() {
	//TODO
}