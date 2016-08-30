#pragma warning(disable: 4996)

#include <iostream>
#include <sstream>
#include <jni.h>
#include "synglyphx_util_ConvertHash.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/string_generator.hpp>
#include "../../../../Core/SGXUtility/hashid.h"

std::wstring Java_To_WStr( JNIEnv *env, jstring string )
{
	std::wstring value;

	const jchar *raw = env->GetStringChars( string, 0 );
	jsize len = env->GetStringLength( string );

	value.assign( raw, raw + len );

	env->ReleaseStringChars( string, raw );

	return value;
}

JNIEXPORT jstring JNICALL Java_synglyphx_util_ConvertHash_getHash(JNIEnv *env, jobject obj, jstring m_dsid, jstring m_t, jstring m_f) {

	using namespace SynGlyphX;

	HashID seed = 0;
	boost::uuids::string_generator gen;
	boost::uuids::uuid m_datasourceID = gen( Java_To_WStr( env, m_dsid ) );

	CombineHashID( seed, m_datasourceID );
	CombineHashID( seed, Java_To_WStr( env, m_t ) );
	CombineHashID( seed, Java_To_WStr( env, m_f ) );

	std::string str = std::to_string(seed);
	return (*env).NewStringUTF(str.c_str());
}
