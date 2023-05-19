#pragma once

// Win API warnings
#pragma warning(disable : 4081)
#pragma warning(disable : 4103)
#pragma warning(disable : 4067)

// STL
#include <random>
#include <string>
#include <algorithm>
#include <functional>
#include <array>
#include <vector>
#include <thread>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <memory>
#include <mutex>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <cassert>
#include <cstdarg>
#include <ctime>

// GLM
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

// Core
#include <core/common.h>
#include <core/logger.h>
#include <core/keycodes.h>
#include <core/timer.h>
#include <core/pointers.h>

// Debugging
#include <debugging/debug_break.h>
#include <debugging/debugger.h>

// ECS
#include <ecs/entity.h>

// Math
#include <math/maths.h>
#include <math/matrices.h>
#include <math/raycast.h>
#include <math/aabb.h>
#include <math/plane.h>
#include <math/sphere.h>
#include <math/frustrum.h>
#include <math/collisions.h>