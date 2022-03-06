#pragma once
#include "class_context_create.h"
#include "class_context_static_var.h"
#include "class_context_static_method.h"
#include "class_context_instance_method.h"
#include "class_context_superclass.h"
#include "class_context_constructor.h"
#include "object_context.h"

#define IS_CONSTRUCTOR 0
#define IS_INSTANCE_METHOD 1
#define IS_STATIC_METHOD 2
#define IS_CONSTRUCTOR_CALLED_IN_CONSTRUCTOR 3