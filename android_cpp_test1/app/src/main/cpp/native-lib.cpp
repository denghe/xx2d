#include <jni.h>
#include <string>
#include <android/log.h>
#define LOG_TAG "***************** test1 cpp ******************"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

//#include <boost/core/demangle.hpp>
//boost::core::demangle( typeid(T).name() ).c_str()

//template<typename T> concept Has_Abc = requires(T t) { t.Abc(); };
//
//struct Foo {
//    void Abc() {}
//};
//struct Bar {
//    void Def() {}
//};
//template<typename T>
//void DoSth() {
//    if constexpr(Has_Abc<T>) {
//        LOGI( "%s has Abc func", typeid(T).name() );
//    } else {
//        LOGI( "%s miss Abc func", typeid(T).name() );
//    }
//}

extern "C" JNIEXPORT void JNICALL
Java_cpp_denghe_test1_MainActivity_onCreateJNI(JNIEnv* env, jobject /* this */) {
    LOGD("%s", "onCreate");
}

extern "C" JNIEXPORT void JNICALL
Java_cpp_denghe_test1_MainActivity_onResumeJNI(JNIEnv* env, jobject /* this */) {
    LOGD("%s", "onResume");
}

extern "C" JNIEXPORT void JNICALL
Java_cpp_denghe_test1_MainActivity_onPauseJNI(JNIEnv* env, jobject /* this */) {
    LOGD("%s", "onPause");
}

extern "C" JNIEXPORT jstring JNICALL
Java_cpp_denghe_test1_MainActivity_stringFromJNI(JNIEnv* env, jobject /* this */) {
    std::string s = "Hello from C++. size_t size = ";
    s += std::to_string(sizeof(size_t));
    LOGD("%s", "stringFromJNI");

//    DoSth<Foo>();
//    DoSth<Bar>();

    return env->NewStringUTF(s.c_str());
}
