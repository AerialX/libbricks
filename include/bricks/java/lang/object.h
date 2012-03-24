#pragma once

#include "bricks/java/jobject.h"
#include "bricks/java/jmethoddelegate.h"

namespace Bricks { namespace Java { namespace Lang {
	BRICKS_JAVA_QUALIFIED_CLASS_START(ObjectClass, "java/lang/Object");
		BRICKS_JAVA_QUALIFIED_CLASS_CONSTRUCTOR_END();
	BRICKS_JAVA_QUALIFIED_CLASS_END();
	BRICKS_JAVA_QUALIFIED_OBJECT_START(Object, ObjectClass);
		BRICKS_JAVA_QUALIFIED_OBJECT_CONSTRUCTOR_END();
	BRICKS_JAVA_QUALIFIED_OBJECT_END();
} } }

namespace Bricks { namespace Java { namespace Internal {
	template<> struct JSignature<JObject> { static String Signature() { return JSignature<Lang::Object>::Signature(); } };
} } }
