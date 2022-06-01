#pragma once

#include <iostream>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

//START_OBJ_DEF
enum class GEN_CLASS_UIFLAGS
    //END_OBJ_SIG
{
    ALL_GOOD = 0,
    GLFW_INIT_ERROR = 1,
    WINDOW_CREATION_ERROR = 2,
    GLEW_INIT_ERROR = 4
};
//END_OBJ_DEF

//START_OBJ_DEF
class GEN_CLASS_UI
    //END_OBJ_SIG
{
public:
    GEN_CLASS_UI() {};

    void init()
    {
        /* Initialize the library */
        if (!glfwInit())
        {
            error_flags = GEN_CLASS_UIFLAGS::GLFW_INIT_ERROR;
            return;
        }

        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
        /* Create a windowed mode window and its OpenGL context */
        window = glfwCreateWindow(window_width, window_height, "Hello World", NULL, NULL);
        if (!window)
        {
            error_flags = GEN_CLASS_UIFLAGS::WINDOW_CREATION_ERROR;
            glfwTerminate();
            return;
        }

        /* Make the window's context current */
        glfwMakeContextCurrent(window);

        if (glewInit() != GLEW_OK)
        {
            error_flags = GEN_CLASS_UIFLAGS::GLEW_INIT_ERROR;
            std::cout << "Could not init glew!\n";
            glfwTerminate();
            return;
        }

        ImGui::CreateContext();
        glfwSwapInterval(1); // Enable vsync

        const char* glsl_version = "#version 130";

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);
    }

    void loop(void (*callback)(ImVec4 &), ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f))
    {
        /* Poll for and process events */
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        callback(clear_color);

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        /* Render here */
        //glClear(GL_COLOR_BUFFER_BIT);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
    }

    bool running()
    {
        return !glfwWindowShouldClose(window);
    }

    void clear()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(window);
        glfwTerminate();
    }

	virtual ~GEN_CLASS_UI()
	{
        clear();
	}
private:

public:
    GEN_CLASS_UIFLAGS error_flags = GEN_CLASS_UIFLAGS::ALL_GOOD;
    GLFWwindow* window = nullptr;
    int window_width = 640;
    int window_height = 480;
};
//END_OBJ_DEF