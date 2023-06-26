#pragma once

// Win API warnings
#pragma warning(disable : 4081)
#pragma warning(disable : 4103)
#pragma warning(disable : 4067)

// Platform
#include <core/platform_detection.h>

// STL
#include <random>
#include <string>
#include <algorithm>
#include <functional>
#include <array>
#include <vector>
#include <thread>
#include <future>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <queue>
#include <memory>
#include <mutex>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <cassert>
#include <cstdarg>
#include <ctime>
#include <utility>
#include <chrono>

// GLM
#include <glm.hpp>
#include <gtx/quaternion.hpp>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/string_cast.hpp>

// Core
#include <core/common.h>
#include <core/build.h>
#include <core/time.h>
#include <core/logger.h>
#include <core/keycodes.h>
#include <core/timer.h>
#include <core/pointers.h>
#include <core/debug_break.h>

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