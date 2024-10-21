#pragma once

#define GCLIB_NUM_ARGS(...) \
    GCLIB_NUM_ARGS_IMPL(one_extra, ##__VA_ARGS__, \
                        10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define GCLIB_NUM_ARGS_IMPL(extra, a0, a1, a2, a3, a4, \
                            a5, a6, a7, a8, a9, a10, ...) \
    GCLIB_NUM_ARGS_##a10

#define GCLIB_NUM_ARGS_0 0
#define GCLIB_NUM_ARGS_1 1
#define GCLIB_NUM_ARGS_2 2
#define GCLIB_NUM_ARGS_3 3
#define GCLIB_NUM_ARGS_4 4
#define GCLIB_NUM_ARGS_5 5
#define GCLIB_NUM_ARGS_6 6
#define GCLIB_NUM_ARGS_7 7
#define GCLIB_NUM_ARGS_8 8
#define GCLIB_NUM_ARGS_9 9
#define GCLIB_NUM_ARGS_10 10

#define GCLIB_COMMA_IMPL ,
#define GCLIB_SEMICOLON_IMPL ;
#define GCLIB_COLON_IMPL :

#define GCLIB_MAP_SEP_LIST_0(m, s)
#define GCLIB_MAP_SEP_LIST_1(m, s, a0) \
    m(a0)
#define GCLIB_MAP_SEP_LIST_2(m, s, a0, a1) \
    m(a0)s m(a1)
#define GCLIB_MAP_SEP_LIST_3(m, s, a0, a1, a2) \
    m(a0)s m(a1)s m(a2)
#define GCLIB_MAP_SEP_LIST_4(m, s, a0, a1, a2, a3) \
    m(a0)s m(a1)s m(a2)s m(a3)
#define GCLIB_MAP_SEP_LIST_5(m, s, a0, a1, a2, a3, a4) \
    m(a0)s m(a1)s m(a2)s m(a3)s m(a4)
#define GCLIB_MAP_SEP_LIST_6(m, s, a0, a1, a2, a3, a4, a5) \
    m(a0)s m(a1)s m(a2)s m(a3)s m(a4)s m(a5)
#define GCLIB_MAP_SEP_LIST_7(m, s, a0, a1, a2, a3, a4, a5, a6) \
    m(a0)s m(a1)s m(a2)s m(a3)s m(a4)s m(a5)s m(a6)
#define GCLIB_MAP_SEP_LIST_8(m, s, a0, a1, a2, a3, a4, a5, a6, a7) \
    m(a0)s m(a1)s m(a2)s m(a3)s m(a4)s m(a5)s m(a6)s m(a7)
#define GCLIB_MAP_SEP_LIST_9(m, s, a0, a1, a2, a3, a4, a5, a6, a7, a8) \
    m(a0)s m(a1)s m(a2)s m(a3)s m(a4)s m(a5)s m(a6)s m(a7)s m(a8)
#define GCLIB_MAP_SEP_LIST_10(m, s, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9) \
    m(a0)s m(a1)s m(a2)s m(a3)s m(a4)s m(a5)s m(a6)s m(a7)s m(a8)s m(a9)

#define GCLIB_MAP_SEP_LIST(map, sep, ...) \
    GCLIB_MAP_SEP_LIST_N(map, sep, GCLIB_NUM_ARGS(__VA_ARGS__), ##__VA_ARGS__)
#define GCLIB_MAP_SEP_LIST_N(map, sep, n, ...) \
    GCLIB_MAP_SEP_LIST_N_IMPL(map, sep, n, ##__VA_ARGS__)
#define GCLIB_MAP_SEP_LIST_N_IMPL(map, sep, n, ...) \
    GCLIB_MAP_SEP_LIST_##n(map, sep##_IMPL, ##__VA_ARGS__)

#define GCLIB_MAP_COMMA_SEP_LIST(map, ...) \
    GCLIB_MAP_SEP_LIST(map, GCLIB_COMMA, ##__VA_ARGS__)

#define GCLIB_STRINGIZE(x) GCLIB_STRINGIZE_IMPL(x)
#define GCLIB_STRINGIZE_IMPL(x) #x
