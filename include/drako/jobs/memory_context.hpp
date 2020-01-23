#pragma once
#ifndef DRAKO_MEMORY_CONTEXT_HPP
#define DRAKO_MEMORY_CONTEXT_HPP

#include "drako/core/dk_compiler_spec.hpp"

namespace drako::jobs
{
    // STRUCTURE
    // Empty structure used for tag dispatching.
    struct memory_context final
    {
        // STRUCTURE
        // Scope limited to the executing job_unit.
        struct local final {};

        // STRUCTURE
        // Global scope.
        struct global final {};
    };


    // STRUCTURE
    struct sharing_context final {};


    // STRUCT TEMPLATE
    // Smart pointer used to enforce memory context semantic.
    template <typename T>
    struct local_ptr
    {
        DRAKO_FORCE_INLINE explicit constexpr local_ptr(T* ptr) noexcept
            : _p(ptr)
        {}

        local_ptr(local_ptr const&) = default;
        local_ptr& operator=(local_ptr const&) = default;

        local_ptr(local_ptr&&) = default;
        local_ptr& operator=(local_ptr&&) = default;

        DRAKO_FORCE_INLINE T* operator->() noexcept { return _p; }

    private:

        T* _p;
    };
}

#endif // !DRAKO_MEMORY_CONTEXT_HPP
