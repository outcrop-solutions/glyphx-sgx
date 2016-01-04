#pragma warning(disable: 4996)

#include <iostream>
#include <sstream>
#include <rpc.h>
#include <jni.h>
#include "synglyphx_util_ConvertHash.h"
#include <boost/lexical_cast.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/functional/hash.hpp>

JNIEXPORT jstring JNICALL Java_synglyphx_util_ConvertHash_getHash(JNIEnv *env, jobject obj, jstring m_dsid, jstring m_t, jstring m_f) {

	const char *nativeString = env->GetStringUTFChars(m_dsid, JNI_FALSE);
	const char *nativeString1 = env->GetStringUTFChars(m_t, JNI_FALSE);
	const char *nativeString2 = env->GetStringUTFChars(m_f, JNI_FALSE);
	std::size_t seed = 0;
	boost::uuids::string_generator gen;
	boost::uuids::uuid m_datasourceID = gen(std::string(nativeString));
	std::string m_table = nativeString1;
	std::string m_field = nativeString2;

	boost::hash_combine(seed, m_datasourceID);
	boost::hash_combine(seed, m_table);
	boost::hash_combine(seed, m_field);

	env->ReleaseStringUTFChars(m_dsid, nativeString);
	env->ReleaseStringUTFChars(m_t, nativeString1);
	env->ReleaseStringUTFChars(m_f, nativeString2);

	std::string str = std::to_string(seed);
	return (*env).NewStringUTF(str.c_str());
}
