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

#define GCLIB_IDENTITY(x) x

#define GCLIB_MAP_SEP_LIST_0(m, p, s)
#define GCLIB_MAP_SEP_LIST_1(m, p, s, a0) \
    m(p, a0)
#define GCLIB_MAP_SEP_LIST_2(m, p, s, a0, a1) \
    m(p, a0)s m(p, a1)
#define GCLIB_MAP_SEP_LIST_3(m, p, s, a0, a1, a2) \
    m(p, a0)s m(p, a1)s m(p, a2)
#define GCLIB_MAP_SEP_LIST_4(m, p, s, a0, a1, a2, a3) \
    m(p, a0)s m(p, a1)s m(p, a2)s m(p, a3)
#define GCLIB_MAP_SEP_LIST_5(m, p, s, a0, a1, a2, a3, a4) \
    m(p, a0)s m(p, a1)s m(p, a2)s m(p, a3)s m(p, a4)
#define GCLIB_MAP_SEP_LIST_6(m, p, s, a0, a1, a2, a3, a4, a5) \
    m(p, a0)s m(p, a1)s m(p, a2)s m(p, a3)s m(p, a4)s m(p, a5)
#define GCLIB_MAP_SEP_LIST_7(m, p, s, a0, a1, a2, a3, a4, a5, a6) \
    m(p, a0)s m(p, a1)s m(p, a2)s m(p, a3)s m(p, a4)s m(p, a5)s m(p, a6)
#define GCLIB_MAP_SEP_LIST_8(m, p, s, a0, a1, a2, a3, a4, a5, a6, a7) \
    m(p, a0)s m(p, a1)s m(p, a2)s m(p, a3)s m(p, a4)s m(p, a5)s m(p, a6)s \
    m(p, a7)
#define GCLIB_MAP_SEP_LIST_9(m, p, s, a0, a1, a2, a3, a4, a5, a6, a7, a8) \
    m(p, a0)s m(p, a1)s m(p, a2)s m(p, a3)s m(p, a4)s m(p, a5)s m(p, a6)s \
    m(p, a7)s m(p, a8)
#define GCLIB_MAP_SEP_LIST_10(m, p, s, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9) \
    m(p, a0)s m(p, a1)s m(p, a2)s m(p, a3)s m(p, a4)s m(p, a5)s m(p, a6)s \
    m(p, a7)s m(p, a8)s m(p, a9)

#define GCLIB_MAP_SEP_LIST(map, p, sep, ...) \
    GCLIB_MAP_SEP_LIST_N(map, p, sep, GCLIB_NUM_ARGS(__VA_ARGS__), \
    ##__VA_ARGS__)
#define GCLIB_MAP_SEP_LIST_N(map, p, sep, n, ...) \
    GCLIB_MAP_SEP_LIST_N_IMPL(map, p, sep, n, ##__VA_ARGS__)
#define GCLIB_MAP_SEP_LIST_N_IMPL(map, p, sep, n, ...) \
    GCLIB_MAP_SEP_LIST_##n(map, p, sep##_IMPL, ##__VA_ARGS__)

#define GCLIB_MAP_COMMA_SEP_LIST(map, p, ...) \
    GCLIB_MAP_SEP_LIST(map, p, GCLIB_COMMA, ##__VA_ARGS__)

#define GCLIB_STRINGIZE(x) GCLIB_STRINGIZE_IMPL(x)
#define GCLIB_STRINGIZE_IMPL(x) #x

#define GCLIB_DEFAULT_TO(DEFAULT, ...) \
    GCLIB_DEFAULT_TO_N(DEFAULT, GCLIB_NUM_ARGS(__VA_ARGS__), ##__VA_ARGS__)

#define GCLIB_DEFAULT_TO_N(DEFAULT, n, ...) \
    GCLIB_DEFAULT_TO_N_IMPL(DEFAULT, n, ##__VA_ARGS__)

#define GCLIB_DEFAULT_TO_N_IMPL(DEFAULT, n, ...) \
    GCLIB_DEFAULT_TO_##n(DEFAULT, ##__VA_ARGS__)

#define GCLIB_DEFAULT_TO_0(DEFAULT) DEFAULT
#define GCLIB_DEFAULT_TO_1(DEFAULT, VALUE) VALUE
