/*
* Copyright (C) 2012-2013 Simon Lynen, ASL, ETH Zurich, Switzerland
* You can contact the author at <slynen at ethz dot ch>
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#pragma once

#include <iostream>

#ifndef NUMERIC_PREC
#define NUMERIC_PREC 4 // Number of decimal places.
#endif

#ifdef UNUSEDPARAM
#elif defined(__GNUC__)
# define UNUSEDPARAM(x) UNUSED_ ## x __attribute__((unused))
#elif defined(__LCLINT__)
# define UNUSEDPARAM(x) /*@unused@*/ x
#else
# define UNUSEDPARAM(x) x
#endif

#ifndef UNUSED
# define UNUSED(x) (void)x;
#endif

#ifndef STREAMQUAT
#define STREAMQUAT(q) "["<<std::setprecision(3)<<(q).w()<<", "<<(q).x()<<", "<< \
(q).y()<<", "<<(q).z()<<std::setprecision(NUMERIC_PREC)<<"]"
#endif

#ifndef DEG2RAD
#define DEG2RAD (M_PI/180.0)
#endif

#ifdef WIN32
#define ASCTEC_LIKELY(x) (x)
#define ASCTEC_UNLIKELY(x) (x)
#else
#define ASCTEC_LIKELY(x) __builtin_expect((x),1)
#define ASCTEC_UNLIKELY(x) __builtin_expect((x),0)
#endif

#ifdef ROS_PACKAGE_NAME // Use ROS if it is available.
#include <ros/console.h>
#define ASCTEC_DEBUG_STREAM(x) ROS_DEBUG_STREAM(x)
#define ASCTEC_INFO_STREAM(x) ROS_INFO_STREAM(x)
#define ASCTEC_WARN_STREAM(x) ROS_WARN_STREAM(x)
#define ASCTEC_ERROR_STREAM(x) ROS_ERROR_STREAM(x)

#define ASCTEC_INFO_STREAM_ONCE(x) ROS_INFO_STREAM_ONCE(x)
#define ASCTEC_WARN_STREAM_ONCE(x) ROS_WARN_STREAM_ONCE(x)
#define ASCTEC_ERROR_STREAM_ONCE(x) ROS_ERROR_STREAM_ONCE(x)

#define ASCTEC_INFO_STREAM_THROTTLE(rate, x) ROS_INFO_STREAM_THROTTLE(rate, x)
#define ASCTEC_WARN_STREAM_THROTTLE(rate, x) ROS_WARN_STREAM_THROTTLE(rate, x)
#define ASCTEC_ERROR_STREAM_THROTTLE(rate, x) ROS_ERROR_STREAM_THROTTLE(rate, x)

#define ASCTEC_INFO_STREAM_COND(cond, x) ROS_INFO_STREAM_COND(cond, x)
#define ASCTEC_WARN_STREAM_COND(cond, x) ROS_WARN_STREAM_COND(cond, x)
#define ASCTEC_ERROR_STREAM_COND(cond, x) ROS_ERROR_STREAM_COND(cond, x)

#else
#include <chrono> // Using std::chrono instead of ros::Time.
// Adapted from rosconsole.
//Copyright (c) 2008, Willow Garage, Inc.

#ifndef ASCTEC_DEBUG_STREAM
#ifdef ASCTEC_SEVERITY_DEBUG
#define ASCTEC_DEBUG_STREAM(x) std::cerr<<"\033[0;0m[INFO] "<<x<<"\033[0;0m"<<std::endl;
#else
#define ASCTEC_DEBUG_STREAM(x)
#endif
#endif

#ifndef ASCTEC_INFO_STREAM
#define ASCTEC_INFO_STREAM(x) std::cerr<<"\033[0;0m[INFO] "<<x<<"\033[0;0m"<<std::endl;
#endif

#ifndef ASCTEC_WARN_STREAM
#define ASCTEC_WARN_STREAM(x) std::cerr<<"\033[0;33m[WARN] "<<x<<"\033[0;0m"<<std::endl;
#endif

#ifndef ASCTEC_ERROR_STREAM
#define ASCTEC_ERROR_STREAM(x) std::cerr<<"\033[1;31m[ERROR] "<<x<<"\033[0;0m"<<std::endl;
#endif

#define ASCTEC_INFO_STREAM_ONCE(x) \
do { \
static bool __log_stream_once__hit__ = false; \
if (ASCTEC_UNLIKELY(!__log_stream_once__hit__)) { \
__log_stream_once__hit__ = true; \
ASCTEC_INFO_STREAM(x); \
} \
} while(0)

#define ASCTEC_WARN_STREAM_ONCE(x) \
do { \
static bool __log_stream_once__hit__ = false; \
if (ASCTEC_UNLIKELY(!__log_stream_once__hit__)) { \
__log_stream_once__hit__ = true; \
ASCTEC_WARN_STREAM(x); \
} \
} while(0)

#define ASCTEC_ERROR_STREAM_ONCE(x) \
do { \
static bool __log_stream_once__hit__ = false; \
if (ASCTEC_UNLIKELY(!__log_stream_once__hit__)) { \
__log_stream_once__hit__ = true; \
ASCTEC_ERROR_STREAM(x); \
} \
} while(0)

#define ASCTEC_LOG_STREAM_THROTTLE(rate, x) \
do { \
static double __log_stream_throttle__last_hit__ = 0.0; \
std::chrono::time_point<std::chrono::system_clock> __log_stream_throttle__now__ = \
std::chrono::system_clock::now(); \
if (ASCTEC_UNLIKELY(__log_stream_throttle__last_hit__ + rate <= \
std::chrono::duration_cast<std::chrono::seconds>( \
__log_stream_throttle__now__.time_since_epoch()).count())) { \
__log_stream_throttle__last_hit__ = std::chrono::duration_cast< \
std::chrono::seconds>(__log_stream_throttle__now__.time_since_epoch()).count(); \
ASCTEC_INFO_STREAM(x); \
} \
} while(0)

#define ASCTEC_WARN_STREAM_THROTTLE(rate, x) \
do { \
static double __log_stream_throttle__last_hit__ = 0.0; \
std::chrono::time_point<std::chrono::system_clock> __log_stream_throttle__now__ = \
std::chrono::system_clock::now(); \
if (ASCTEC_UNLIKELY(__log_stream_throttle__last_hit__ + rate <= \
std::chrono::duration_cast<std::chrono::seconds>( \
__log_stream_throttle__now__.time_since_epoch()).count())) { \
__log_stream_throttle__last_hit__ = \
std::chrono::duration_cast<std::chrono::seconds>( \
__log_stream_throttle__now__.time_since_epoch()).count(); \
ASCTEC_WARN_STREAM(x); \
} \
} while(0)

#define ASCTEC_ERROR_STREAM_THROTTLE(rate, x) \
do { \
static double __log_stream_throttle__last_hit__ = 0.0; \
std::chrono::time_point<std::chrono::system_clock> __log_stream_throttle__now__ = \
std::chrono::system_clock::now(); \
if (ASCTEC_UNLIKELY(__log_stream_throttle__last_hit__ + rate <= \
std::chrono::duration_cast<std::chrono::seconds>(\
__log_stream_throttle__now__.time_since_epoch()).count())) { \
__log_stream_throttle__last_hit__ = \
std::chrono::duration_cast<std::chrono::seconds>\
(__log_stream_throttle__now__.time_since_epoch()).count(); \
ASCTEC_ERROR_STREAM(x); \
} \
} while(0)

#define ASCTEC_INFO_STREAM_COND(cond, x) \
do { \
if (ASCTEC_UNLIKELY(cond)) { \
ASCTEC_INFO_STREAM(x); \
} \
} while(0)

#define ASCTEC_WARN_STREAM_COND(cond, x) \
do { \
if (ASCTEC_UNLIKELY(cond)) { \
ASCTEC_WARN_STREAM(x); \
} \
} while(0)

#define ASCTEC_ERROR_STREAM_COND(cond, x) \
do { \
if (ASCTEC_UNLIKELY(cond)) { \
ASCTEC_ERROR_STREAM(x); \
} \
} while(0)

#endif
