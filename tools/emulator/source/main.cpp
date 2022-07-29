#include "input.h"
#include "ui/ui.h"
#include "emulator.h"

#include "imgui.h"
#include "imgui_impl/imgui_impl_sdl.h"
#include "imgui_impl/imgui_impl_opengl3.h"

#include "SDL2/SDL.h"
#ifdef IMGUI_IMPL_OPENGL_ES2
#include <SDL2/SDL_opengles2.h>
#else
#include <SDL2/SDL_opengl.h>
#endif

#include <algorithm>
#include <time.h>


#define WINDOW_SCALE 10
#define WINDOW_WIDTH WINDOW_SCALE*CHIP8_DISPLAY_WIDTH
#define WINDOW_HEIGHT WINDOW_SCALE*CHIP8_DISPLAY_HEIGHT


static SDL_Window* window;
static SDL_GLContext gl_context;


static bool init_platform()
{
    // Setup SDL
    // (Some versions of SDL before <2.0.10 appears to have performance/stalling issues on a minority of Windows systems,
    // depending on whether SDL_INIT_GAMECONTROLLER is enabled or disabled.. updating to latest version of SDL is recommended!)
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return false;
    }

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    window = SDL_CreateWindow("CHIP-8 Emulator", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
        1920, 1080, 
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    return true;
}


int main(int, char**)
{
    srand(time(NULL));

    if (!init_platform())
    {
        return -1;
    }
    unsigned int current_time = SDL_GetTicks();

    Emulator* emulator = emulator_new();

    // Our state
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Main loop
    bool running = true;
    unsigned int last_time = current_time;
    double delta_time;
    while (running)
    {
        /* Platform-Events */
        current_time = SDL_GetTicks();
        delta_time = (current_time - last_time) / 1000.0;
        last_time = current_time;

        input_new_frame();
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            switch (event.type)
            {
                case SDL_QUIT: {
                    running = false;
                    break;
                }
                case SDL_WINDOWEVENT: {
                    if (event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window)) {
                        running = false;
                    }
                    break;
                }
                case SDL_KEYUP:
                case SDL_KEYDOWN: {
                    input_update(event.type, event.key.keysym.sym);
                    break;
                }
                default: {
                    break;
                }
            }
        }

        /* Logic */

        if (input_is_key_pressed(SDLK_F10))
        {
            emulator->configuration.mode = Emulator_Ticking;
        }
        if (input_is_key_pressed(SDLK_F5))
        {
            emulator->configuration.mode = Emulator_Running;
        }
        if (input_is_key_pressed(SDLK_SPACE))
        {
            emulator->configuration.mode = Emulator_Paused;
        }

        emulator_tick(emulator, delta_time);


        /* Rendering */

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        {
            ImGui::ShowDemoWindow(nullptr);

            ui_emulation_controls(emulator);
            ui_chip8_ram(emulator->ch8);
            ui_chip8_inspector(emulator->ch8);
            ui_chip8_disassembly(emulator->ch8);
            ui_chip8_vram(emulator->ch8);
        }

        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    emulator_delete(emulator);

    return 0;
}
