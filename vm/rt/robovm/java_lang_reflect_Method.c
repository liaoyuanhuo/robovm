#include <robovm.h>
#include <string.h>
#include "utlist.h"

Class* Java_java_lang_reflect_Method_getDeclaringClass(Env* env, Class* clazz, jlong methodPtr) {
    Method* method = (Method*) LONG_TO_PTR(methodPtr);
    return method->clazz;
}

jint Java_java_lang_reflect_Method_getModifiers(Env* env, Class* clazz, jlong methodPtr) {
    Method* method = (Method*) LONG_TO_PTR(methodPtr);
    return (method->access & METHOD_ACCESS_MASK) & ~(ACC_BRIDGE | ACC_VARARGS | ACC_SYNTHETIC);
}

Object* Java_java_lang_reflect_Method_getName(Env* env, Class* clazz, jlong methodPtr) {
    Method* method = (Method*) LONG_TO_PTR(methodPtr);
    return rvmNewStringUTF(env, method->name, -1);
}

Class* Java_java_lang_reflect_Method_getReturnType(Env* env, Class* clazz, jlong methodPtr) {
    Method* method = (Method*) LONG_TO_PTR(methodPtr);
    return rvmFindClassByDescriptor(env, rvmGetReturnType(method->desc), method->clazz->classLoader);
}

Object* Java_java_lang_reflect_Method_getSignatureAttribute(Env* env, Class* clazz, jlong methodPtr) {
    Method* method = (Method*) LONG_TO_PTR(methodPtr);
    Class* java_lang_reflect_Proxy = rvmFindClass(env, "java/lang/reflect/Proxy");
    if (method->clazz->superclass == java_lang_reflect_Proxy) {
        return rvmAttributeGetMethodSignature(env, ((ProxyMethod*) method)->proxiedMethod);
    }
    return rvmAttributeGetMethodSignature(env, method);
}

ObjectArray* Java_java_lang_reflect_Method_getParameterTypes(Env* env, Class* clazz, jlong methodPtr) {
    Method* method = (Method*) LONG_TO_PTR(methodPtr);

    jint argsCount = rvmGetParameterCount(method);

    Class* array_java_lang_Class = rvmFindClass(env, "[Ljava/lang/Class;");
    if (!array_java_lang_Class) return NULL;
    ObjectArray* paramTypes = rvmNewObjectArray(env, argsCount, NULL, array_java_lang_Class, NULL);
    if (!paramTypes) return NULL;

    const char* desc = method->desc;
    const char* s;
    jint i = 0;
    while ((s = rvmGetNextParameterType(&desc))) {
        char* paramTypeName = rvmAllocateMemory(env, desc - s + 1);
        if (!paramTypeName) return NULL;
        strncpy(paramTypeName, s, desc - s);
        Class* paramType = rvmFindClassByDescriptor(env, paramTypeName, method->clazz->classLoader);
        if (!paramType) return NULL;
        paramTypes->values[i++] = (Object*) paramType;
    }

    return paramTypes;
}

ObjectArray* Java_java_lang_reflect_Method_getExceptionTypes(Env* env, Class* clazz, jlong methodPtr) {
    Method* method = (Method*) LONG_TO_PTR(methodPtr);
    Class* java_lang_reflect_Proxy = rvmFindClass(env, "java/lang/reflect/Proxy");
    if (method->clazz->superclass == java_lang_reflect_Proxy) {
        return rvmAttributeGetExceptions(env, ((ProxyMethod*) method)->proxiedMethod);
    }
    return rvmAttributeGetExceptions(env, method);
}

Object* Java_java_lang_reflect_Method_getDefaultValue(Env* env, Class* clazz, jlong methodPtr) {
    Method* method = (Method*) LONG_TO_PTR(methodPtr);
    return rvmAttributeGetAnnotationDefault(env, method);
}

ObjectArray* Java_java_lang_reflect_Method_getDeclaredAnnotations(Env* env, Class* clazz, jlong methodPtr) {
    Method* method = (Method*) LONG_TO_PTR(methodPtr);
    return rvmAttributeGetMethodRuntimeVisibleAnnotations(env, method);
}

ObjectArray* Java_java_lang_reflect_Method_getParameterAnnotations(Env* env, Class* clazz, jlong methodPtr) {
    Method* method = (Method*) LONG_TO_PTR(methodPtr);
    return rvmAttributeGetMethodRuntimeVisibleParameterAnnotations(env, method);
}
