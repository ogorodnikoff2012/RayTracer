//
// Created by xenon on 20.08.17.
//

#ifndef RAYTRACER_OPTIONAL_H
#define RAYTRACER_OPTIONAL_H

#include <utility>
#include <exception>

namespace util {
    class OptionalBadAccess : public std::exception {};

    template <class T>
    class Optional {
        union Storage {
            T val;
            char dummy;
        public:
            Storage() : dummy() {}
            Storage(const T &t) : val(t) {}
            Storage(T &&t) : val(std::move(t)) {}
            ~Storage() {}
        };
        Storage s;
        bool valid;
    public:
        Optional(const T &t) : s(t), valid(true) {}
        Optional(T &&t) : s(std::move(t)), valid(true) {}
        Optional() : s(), valid(false) {}
        Optional(const Optional &o) : s(), valid(o.valid) {
            if (valid) {
                s = Storage(o.s.val);
            }
        }
        Optional(Optional &&o) : s(), valid(o.valid) {
            if (valid) {
                s = Storage(std::move(o.s.val));
            }
        }
        Optional &operator =(const Optional &o) {
            if (this == &o) {
                return *this;
            }
            reset();
            if (o.valid) {
                s = Storage(o.s.val);
                valid = true;
            }
            return *this;
        }
        Optional &operator =(Optional &&o) {
            if (this == &o) {
                return *this;
            }
            reset();
            if (o.valid) {
                s = Storage(std::move(o.s.val));
                valid = true;
            }
            return *this;
        }
        ~Optional() {
            reset();
        }
        void reset() {
            if (valid) {
                s.val.T::~T();
                valid = false;
            }
        }
        const T &operator *() const & {
            return s.val;
        }
        const T &&operator *() const && {
            return s.val;
        }
        const T *operator ->() const {
            return &s.val;
        }
        T &operator *() & {
            return s.val;
        }
        T &&operator *() && {
            return s.val;
        }
        T *operator ->() {
            return &s.val;
        }
        operator bool() const {
            return valid;
        }
        bool hasValue() const {
            return valid;
        }
        const T &value() const & {
            if (!valid) {
                throw OptionalBadAccess();
            }
            return s.val;
        }
        T &value() & {
            if (!valid) {
                throw OptionalBadAccess();
            }
            return s.val;
        }
        const T &&value() const && {
            if (!valid) {
                throw OptionalBadAccess();
            }
            return s.val;
        }
        T &&value() && {
            if (!valid) {
                throw OptionalBadAccess();
            }
            return s.val;
        }
    };

    template <class T, class ...Args>
    Optional<T> makeOptional(Args &&...args) {
        return Optional<T>(T(std::forward<Args>(args)...));
    }

}


#endif //RAYTRACER_OPTIONAL_H
