#include "CPU.h"

#include "SDL.h"
#include "imgui.h"
#include "imgui_sdl.h"

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main(int argc, char* args[])
{


	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Window* window = SDL_CreateWindow("SDL2 ImGui Renderer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_RESIZABLE);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

	ImGui::CreateContext();
	ImGuiSDL::Initialize(renderer, 800, 600);

	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, 100, 100);
	{
		SDL_SetRenderTarget(renderer, texture);
		SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
		SDL_RenderClear(renderer);
		SDL_SetRenderTarget(renderer, nullptr);
	}

	CPU* cpu = new CPU();
	cpu->loadRom("./tetris.gb");

	bool run = true;
	while (run)
	{
		ImGuiIO& io = ImGui::GetIO();

		int wheel = 0;

		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT) run = false;
			else if (e.type == SDL_WINDOWEVENT)
			{
				if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				{
					io.DisplaySize.x = static_cast<float>(e.window.data1);
					io.DisplaySize.y = static_cast<float>(e.window.data2);
				}
			}
			else if (e.type == SDL_MOUSEWHEEL)
			{
				wheel = e.wheel.y;
			}
		}

		int mouseX, mouseY;
		const int buttons = SDL_GetMouseState(&mouseX, &mouseY);

		// Setup low-level inputs (e.g. on Win32, GetKeyboardState(), or write to those fields from your Windows message loop handlers, etc.)

		io.DeltaTime = 1.0f / 60.0f;
		io.MousePos = ImVec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
		io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
		io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
		io.MouseWheel = static_cast<float>(wheel);

		ImGui::NewFrame();
		

		// ImGui::ShowDemoWindow();
		CPU::registers_t registers = cpu->getRegisters();
		ImGui::Begin("Debugger Controls");
		if (ImGui::Button("Step")) {
			cpu->step();
		}
		if (ImGui::Button("Step x10")) {
			for(int i = 0; i < 10; i++)
				cpu->step();
		}
		if (ImGui::Button("Step x1000")) {
			for (int i = 0; i < 1000; i++)
				cpu->step();
		}
		if (ImGui::Button("Run Until 0x279E")) {
			while (registers.pc < 0x2700) {
				cpu->step();
				registers = cpu->getRegisters();
			}
		}
		ImGui::End();

		ImGui::Begin("CPU Info");
		ImGui::Text("Register Values: ");
		ImGui::Text("PC: 0x%X; B: 0x%X; C: 0x%X; D: 0x%X; E: 0x%X; H: 0x%X; L: 0x%X; ACC: 0x%X", registers.pc, registers.b, registers.c, registers.d, registers.e, registers.h, registers.l, registers.acc);
		ImGui::Spacing();
		ImGui::Text("Flags: ");
		ImGui::Text("Flag: 0x%X; ZERO: %d; SUB: %d; HC: %d; C: %d", registers.flag, ACCESS_BIT(registers.flag, ZERO_FLAG), ACCESS_BIT(registers.flag, SUB_FLAG), ACCESS_BIT(registers.flag, HALF_CARRY_FLAG), ACCESS_BIT(registers.flag, CARRY_FLAG));
		ImGui::End();

		SDL_SetRenderDrawColor(renderer, 114, 144, 154, 255);
		SDL_RenderClear(renderer);

		ImGui::Render();
		ImGuiSDL::Render(ImGui::GetDrawData());

		SDL_RenderPresent(renderer);
	}

	ImGuiSDL::Deinitialize();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	ImGui::DestroyContext();

	return 0;
	
}