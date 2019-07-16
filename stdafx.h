#ifndef stdafxH
#define stdafxH

//network
#include <ws2tcpip.h>
//opengl
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <gl/gl.h>
#include <gl/glu.h>
//dos
#include <time.h>
#include <stdio.h>
#include <math.h>
#include <conio.h>
//file stats
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
//stl
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <fstream>
#include <map>
//glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

//global log file variable - in stdafx.cpp
extern ofstream LogFile;

//sign template
template <typename T> int sgn(T val)
{
    return (T(0) < val) - (val < T(0));
}

//structures
#include "structures/Quad.h"
#include "structures/Texture.h"

//IMPORTANT!
#define SHR_LAYOUT_VERTEX 4
#define SHR_LAYOUT_UV 5
#define SHR_LAYOUT_OFFSET_VERTEX 6
#define SHR_LAYOUT_OFFSET_UV 7

#endif
