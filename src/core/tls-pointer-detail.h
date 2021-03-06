/**
 * @file tls-pointer-detail.h
 * @brief TLS pointer implementation
 *
 * You should never include this file directly
 */
#ifndef EPYX_TLS_POINTER_DETAIL_H
#define EPYX_TLS_POINTER_DETAIL_H

namespace Epyx {

    template<typename T> void TLSPointer<T>::_default_destructor(T* object){
        delete object;
    }

    template<typename T> TLSPointer<T>::TLSPointer(void (*destructor_)(T*))
        :destructor(destructor_), constructor(NULL), key(new pthread_key_t) {
        int key_create_status = pthread_key_create(this->key,
            reinterpret_cast<void_destructor_func*>(this->destructor));
        EPYX_ASSERT_NO_LOG(key_create_status == 0);
    }

    template<typename T> TLSPointer<T>::TLSPointer(T* (*constructor_)(), void (*destructor_)(T*))
        :destructor(destructor_), constructor(constructor_), key(new pthread_key_t){
        int key_create_status = pthread_key_create(this->key,
            reinterpret_cast<void_destructor_func*>(this->destructor));
        EPYX_ASSERT_NO_LOG(key_create_status == 0);
    }

    template<typename T> TLSPointer<T>::~TLSPointer(){//TODO
    };

    template<typename T> T* TLSPointer<T>::get(){
        T* result = static_cast<T*>(pthread_getspecific(*this->key));

        if(result == NULL && this->constructor != NULL){
            result = this->constructor();
            pthread_setspecific(*this->key, static_cast<void *>(result));
        }

        return result;
    }

    template<typename T> void TLSPointer<T>::reset(T* new_value){
        T* current = static_cast<T*>(pthread_getspecific(*this->key));

        if(new_value != current){
            pthread_setspecific(*this->key, static_cast<void *>(new_value));
        }
    }

    template<typename T> T* TLSPointer<T>::release(){
        T* temp = static_cast<T*>(pthread_getspecific(*this->key));
        pthread_setspecific(*this->key, NULL);
        return temp;
    }
}

#endif /* EPYX_TLS_POINTER_DETAIL_H */
