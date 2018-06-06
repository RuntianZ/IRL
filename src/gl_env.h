// Author: Runtian Zhai 1600012737

#pragma once
// Use the following if generating dll
#define DLL_MODE

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <cmath>
#include <cassert>
#include <vector>

#ifndef M_PI
#define M_PI (3.1415926535897932)
#endif

#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <FreeImage.h>
#include <GL/glut.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "freeglut.lib")

#ifdef GLEW_STATIC
#pragma comment(lib, "glew32s.lib")
#else
#pragma comment(lib, "glew32.lib")
#endif

#ifdef GLFW_STATIC
#pragma comment(lib, "glfw3.lib")
#else
#pragma comment(lib, "glfw3dll.lib")
#endif

#pragma comment(lib, "FreeImage.lib")