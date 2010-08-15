#ifdef __cplusplus
extern "C"
{
    #endif
    #ifndef BTEA_H
    #define BTEA_H

    #include <stdint.h>

    int btea( uint32_t *v, int32_t n, const uint32_t k[4] );
    void swap_bytes( uint32_t *v, uint32_t n );
    #endif
    #ifdef __cplusplus
}
#endif
