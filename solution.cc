#include <cstdio>
#include <cstddef>
#include <cstdlib>
#include <immintrin.h>

void optimized_pre_phase1(size_t) {}

void optimized_post_phase1() {}

void optimized_pre_phase2(size_t) {}

void optimized_post_phase2() {}

static int cmp(void const* a, void const* b) {
    return *(float*)a < *(float*)b ? -1 : 1;
}

void optimized_do_phase1(float* data, size_t size) {
    qsort(data, size, sizeof(data[0]), cmp);
}

#define BATCH_SIZE 8

void optimized_do_phase2(size_t* result, float* data, float* query, size_t size) {
    const __m512i one = _mm512_set1_epi64(1);

#pragma omp parallel for
    for (size_t batch = 0; batch < size - BATCH_SIZE; batch += BATCH_SIZE) {
        const __m256 key = _mm256_loadu_ps(query + batch);
        __m512i left = _mm512_set1_epi64(0);
        __m512i right = _mm512_set1_epi64(size);
        while (_mm512_cmp_epi64_mask(left, right, _MM_CMPINT_LT)) {
            const __m512i mid = _mm512_add_epi64(left,
                _mm512_srai_epi64(_mm512_sub_epi32(right, left), 1)
            );
            __m256 current = _mm512_i64gather_ps(mid, data, sizeof(float));
            const __mmask8 mask_lt = _mm256_cmp_ps_mask(current, key, _MM_CMPINT_LT);
            const __mmask8 mask_ge = mask_lt ^ 0xff;
            const __m512i mid_plus1 = _mm512_add_epi64(mid, one);
            left = _mm512_mask_blend_epi64(mask_lt, left, mid_plus1);
            right = _mm512_mask_blend_epi64(mask_ge, right, mid);
        }

        _mm512_storeu_epi64(result + batch, left);
    }

    for (size_t i = size - BATCH_SIZE; i < size; ++i) {
        size_t l = 0, r = size;
        while (l < r) {
            size_t m = l + (r - l) / 2;
            if (data[m] < query[i]) {
                l = m + 1;
            } else {
                r = m;
            }
        }
        result[i] = l;
    }
}
