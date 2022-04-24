#pragma once

#define GLM_ENABLE_EXPERIMENTAL

// Libraries
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <array>
#include <map>
#include <any>
#include <utility>

namespace Desk {
	// Pre-defs
	class Window;

	// Typedefs
	typedef void (*Callback)(void);

	class Widget {
		/* Widget
		* -------------
		* Base Widget class, used for
		* creating basic shapes with
		* hierarchy.
		*/
	private:
		virtual void initialize(Window* window) {
			std::cout << "DSD" << std::endl;
		};
	protected:
		// Position and size (p and s)
		std::array<float, 3> posi;
		std::array<float, 2> siz;

		const char* id;

		// Hierarchy vars
		std::vector<Widget*> children;
		const char* tex_path;

		friend GLFWwindow* getGLWindow(Window*);
	public:
		// Other properties
		bool hidden;

		// Initialization
		Widget(const char* id, std::array<float, 3> posi, std::array<float, 2> siz, const char* tex_path) : id(id), posi(posi), siz(siz), tex_path(tex_path) {
			this->hidden = false;
		}

		std::vector<float> getVertices() {
			/* Math Equation to calculate vertices (V vector)
			* -----------------------------------------------
			* V = [p, p+s[0]
			* ,p+s[1], p+s]
			*/
			return {
				this->posi[0],	this->posi[1],								posi[2], /*|*/ siz[0], siz[1],
				this->posi[0] + this->siz[0], this->posi[1],				posi[2], /*|*/ 0.0f, siz[1],
				this->posi[0],	this->posi[1] + this->siz[1],				posi[2], /*|*/ siz[0], 0.0f,
				this->posi[0] + this->siz[0], this->posi[1] + this->siz[1],	posi[2], /*|*/ 0.0f, 0.0f
			};
		}

		// Set and Get functions
		void setPos(std::array<float, 3> position) {
			posi = position;
		}

		void setPos(float x, float y, float z) {
			posi = { x, y, z };
		}

		void setSize(std::array<float, 2> size) {
			siz = size;
		}

		void setSize(float x, float y) {
			siz = { x, y };
		}

		std::vector<Widget*> getChildren() {
			return children;
		}

		void operator+=(Widget* widget) {
			children.push_back(widget);
		}

		friend class Window;
	};

	class Window {
	private:
		GLFWwindow* window;
		std::vector<Widget*> children;
	public:
		static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
		{
			glViewport(0, 0, width, height);
		}

		// CLASS FUNCTIONS
		void operator+=(Widget* widget) {  // Operator override
			this->children.push_back(widget);
		}

		void initChildren(std::vector<Widget*> children) {
			for (auto& child : children)  // Iterate over children
			{
				std::cout << typeid(child).name() << std::endl;
				initChildren(child->getChildren());
				child->initialize(this);  // Initialize children's basic window functions
			}
		}

		std::vector<std::map<const char*, unsigned int>> iterateChildren(std::vector<Widget*> children) {
			std::vector<std::map<const char*, unsigned int>> UInts;

			for (auto& child : children)
			{
				std::vector<std::map<const char*, unsigned int>> childrenUInts = iterateChildren(child->getChildren());

				Widget uchild = *child;

				unsigned int texture;
				glGenTextures(1, &texture);
				glBindTexture(GL_TEXTURE_2D, texture);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				int width, height, nrChannels;
				unsigned char* data = stbi_load(uchild.tex_path, &width, &height, &nrChannels, 0);
				if (data)
				{
					glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
					glGenerateMipmap(GL_TEXTURE_2D);
				}
				else
				{
					std::cout << "Failed to load texture" << std::endl;
				}
				stbi_image_free(data);

				std::vector<float> vertices = child->getVertices();

				unsigned int indices[] = { 0, 1, 3,
					0, 2 ,3 };

				unsigned int VAO;
				glGenVertexArrays(1, &VAO);

				glBindVertexArray(VAO);

				unsigned int VBO;
				glGenBuffers(1, &VBO);
				glBindBuffer(GL_ARRAY_BUFFER, VBO);
				glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

				unsigned int EBO;
				glGenBuffers(1, &EBO);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
				glEnableVertexAttribArray(0);

				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
				glEnableVertexAttribArray(1);

				UInts.insert(UInts.end(), childrenUInts.begin(), childrenUInts.end());
				UInts.push_back({ {"VAO", VAO}, {"texture", texture} });
			}

			return UInts;
		}

		Window(int width, int height, const char* title, bool maximized) {
			if (!glfwInit())
				throw std::invalid_argument("ERNO!");

			if (maximized)
				glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);

			window = glfwCreateWindow(width, height, title, NULL, NULL);
			if (!window)
			{
				glfwTerminate();
				throw std::invalid_argument("ERNO!");
			}

			glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		}

		void start() {
			glfwMakeContextCurrent(window);
			gladLoadGL();

			const char* vertexShaderSource = "#version 330 core\n"
				"layout (location = 0) in vec3 aPos;\n"
				"layout (location = 1) in vec2 aTexCoord;\n"
				"\n"
				"out vec2 TexCoord;\n"
				"\n"
				"void main()\n"
				"{\n"
				"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
				"   TexCoord = aTexCoord;\n"
				"}\0";

			unsigned int vertexShader;
			vertexShader = glCreateShader(GL_VERTEX_SHADER);

			glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
			glCompileShader(vertexShader);

			int  success;
			char infoLog[512];
			glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

			if (!success)
			{
				glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
			}

			const char* fragmentShaderSource = "#version 330 core\n"
				"out vec4 FragColor;\n"
				"\n"
				"in vec2 TexCoord;"
				"\n"
				"uniform sampler2D ourTexture;\n"
				"\n"
				"void main()\n"
				"{\n"
				"   FragColor = texture(ourTexture, TexCoord);\n"
				"}\0";

			unsigned int fragmentShader;
			fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

			glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
			glCompileShader(fragmentShader);

			glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

			if (!success)
			{
				glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
			}

			unsigned int shaderProgram;
			shaderProgram = glCreateProgram();

			glAttachShader(shaderProgram, vertexShader);
			glAttachShader(shaderProgram, fragmentShader);
			glLinkProgram(shaderProgram);

			glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
				std::cout << "ERROR::PROGRAM::LINKING_FAILED\n";
				std::cout << infoLog << std::endl;
			}

			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			initChildren(children);

			while (!glfwWindowShouldClose(window))
			{
				glClear(GL_COLOR_BUFFER_BIT);

				std::vector<std::map<const char*, unsigned int>> UInts = iterateChildren(children);

				glUseProgram(shaderProgram);
				for (auto& UIntL : UInts) {
					glBindTexture(GL_TEXTURE_2D, UIntL["texture"]);
					glBindVertexArray(UIntL["VAO"]);
					glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
				}

				glBindVertexArray(0);

				glfwSwapBuffers(window);

				glfwPollEvents();
			}

			glfwTerminate();
		}

		GLFWwindow* getGLWindow() {
			return window;
		}
		friend GLFWwindow* getGLWindow(Window*);
	};
	GLFWwindow* getGLWindow(Window* window) {
		return window->getGLWindow();
	};
}
