/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) 2026 MPK Software, St.-Petersburg, Russia
 *
 * @author Stepan Orlov <majorsteve@mail.ru>
 */

#pragma once

#if defined(__clang__)
    #define GCLIB_DIAGNOSTIC_PUSH()            _Pragma("clang diagnostic push")
    #define GCLIB_DIAGNOSTIC_POP()             _Pragma("clang diagnostic pop")
    #define GCLIB_DISABLE_DANGLING_REFERENCE() _Pragma("clang diagnostic ignored \"-Wdangling-reference\"")
#elif defined(__GNUC__)
    #define GCLIB_DIAGNOSTIC_PUSH()            _Pragma("GCC diagnostic push")
    #define GCLIB_DIAGNOSTIC_POP()             _Pragma("GCC diagnostic pop")
    #define GCLIB_DISABLE_DANGLING_REFERENCE() _Pragma("GCC diagnostic ignored \"-Wdangling-reference\"")
#else
    #define GCLIB_DIAGNOSTIC_PUSH()
    #define GCLIB_DIAGNOSTIC_POP()
    #define GCLIB_DISABLE_DANGLING_REFERENCE()
#endif
