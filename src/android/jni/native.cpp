#include <jni.h>
#include <string>
#include <botan_all.h>
#include <memory> // for auto_ptr
#include <android/log.h>

#define ALGO "AES-256"

using namespace Botan;

// http://blogs.gentoo.org/ferdy/
std::string from_java_string(JNIEnv * const env, jstring s)
{
    const char* const c_s(env->GetStringUTFChars(s, 0));
    std::string result(c_s);
    env->ReleaseStringUTFChars(s, c_s);
    return result;
}

jstring to_java_string(JNIEnv * const env, const std::string & s)
{
    return env->NewStringUTF(s.c_str());
}

jbyteArray to_byte_array(JNIEnv * const env, Botan::byte* array, long length)
{
	jbyte jbytesBuffer[length];
	for (int i = 0; i < length; i++)
	{
		jbytesBuffer[i] = *array++;
	}

	jbyteArray jbytes = env->NewByteArray(length);
	env->SetByteArrayRegion(jbytes, 0, length, jbytesBuffer);
	return jbytes;
}

#ifdef __cplusplus
extern "C" {
#endif

jbyteArray Java_com_petroules_silverlock_Native_generateKeys(JNIEnv *env, jobject obj, jstring password)
{
	const std::string algo = ALGO;
	const std::string passphrase = from_java_string(env, password);

	LibraryInitializer init;

	const BlockCipher* cipher_proto = global_state().algorithm_factory().prototype_block_cipher(algo);
	const u32bit key_len = cipher_proto->maximum_keylength();
	const u32bit iv_len = cipher_proto->block_size();
	const u32bit iterations = 8192;

	AutoSeeded_RNG rng;
	SecureVector<Botan::byte> salt(8);
	rng.randomize(&salt[0], salt.size());

    std::auto_ptr<PBKDF> pbkdf(get_pbkdf("PBKDF2(SHA-1)"));
    SymmetricKey bc_key = pbkdf->derive_key(key_len, "BLK" + passphrase, &salt[0], salt.size(), iterations);
    InitializationVector iv = pbkdf->derive_key(iv_len, "IVL" + passphrase, &salt[0], salt.size(), iterations);
    SymmetricKey mac_key = pbkdf->derive_key(16, "MAC" + passphrase, &salt[0], salt.size(), iterations);

    Botan::byte* bc_keyBytes = new Botan::byte[bc_key.length()];
    bc_key.bits_of().copy(bc_keyBytes, bc_key.length());

    return to_byte_array(env, bc_keyBytes, bc_key.length());
}

#ifdef __cplusplus
}
#endif
