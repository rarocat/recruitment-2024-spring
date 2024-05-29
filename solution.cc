#include <cstdio>
#include <cstring>
#include <cstddef>
#include <cstdlib>
#include <algorithm>
#include <immintrin.h>

void optimized_pre_phase1(size_t) {}

void optimized_post_phase1() {}

void optimized_pre_phase2(size_t) {}

void optimized_post_phase2() {}

void merge_sort(float * const data, float * const aux, const size_t size, const size_t depth) {
    if (depth >= 8) {
        std::sort(data, data + size);
        return;
    }

    float * const left = data;
    const size_t left_len = size / 2;

    float * const right = data + left_len;
    const size_t right_len = size - left_len;

#pragma omp taskgroup
    {
#pragma omp task
        merge_sort(left, aux, left_len, depth + 1);
#pragma omp task
        merge_sort(right, aux + left_len, right_len, depth + 1);
#pragma omp taskyield
    }

    size_t l = 0, r = 0;
    size_t cnt = 0;

    while (l < left_len && r < right_len) {
        if (left[l] < right[r]) {
            aux[cnt++] = left[l++];
        } else {
            aux[cnt++] = right[r++];
        }
    }

    memcpy(aux + cnt, left + l, sizeof(float) * (left_len - l));
    memcpy(aux + cnt + left_len - l, right + r, sizeof(float) * (right_len - r));
    memcpy(data, aux, sizeof(float) * size);
}

void optimized_do_phase1(float* data, size_t size) {
    auto aux = (float *)malloc(sizeof(float) * size);
#pragma omp parallel
#pragma omp single
    merge_sort(data, aux, size, 0);
    free(aux);
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
