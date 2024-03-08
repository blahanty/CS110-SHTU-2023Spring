#include "d2q9_bgk.h"


/* The main processes in one step */
int collision(const t_param params, t_speed *cells, t_speed *tmp_cells, int *obstacles);

int streaming(const t_param params, t_speed *cells, t_speed *tmp_cells);

int boundary(const t_param params, t_speed *cells, t_speed *tmp_cells, float *inlets);

/*
** The main calculation methods.
** timestep calls, in order, the functions:
** collision(), obstacle(), streaming() & boundary()
*/
int timestep(const t_param params, t_speed *cells, t_speed *tmp_cells, float *inlets, int *obstacles) {
    /* The main time overhead, you should mainly optimize these processes. */
    collision(params, cells, tmp_cells, obstacles);
    streaming(params, cells, tmp_cells);
    boundary(params, cells, tmp_cells, inlets);
    return EXIT_SUCCESS;
}

/*
** The collision of fluids in the cell is calculated using 
** the local equilibrium distribution and relaxation process
*/
int collision(const t_param params, t_speed *cells, t_speed *tmp_cells, int *obstacles) {
    const float c_sq = 1.f / 3.f; /* square of speed of sound */
    const float w0 = 4.f / 9.f;   /* weighting factor */
    const float w1 = 1.f / 9.f;   /* weighting factor */
    const float w2 = 1.f / 36.f;  /* weighting factor */

    /* loop over the cells in the grid
    ** the collision step is called before
    ** the streaming step and so values of interest
    ** are in the scratch-space grid */

    __m256 f1 = _mm256_set1_ps(1.f), f2 = _mm256_set1_ps(2.f * c_sq * c_sq), c_sq_m = _mm256_set1_ps(c_sq);
#pragma omp parallel for simd collapse(2)
    for (int jj = 0; jj < params.ny; ++jj) {
        for (int ii = 0; ii < params.nx; ++ii) {
            size_t index = ii + jj * params.nx;
            if (!obstacles[index]) {
                /* compute local density total */
                float local_density = 0.f;

                for (int kk = 0; kk < NSPEEDS; ++kk)
                    local_density += cells[index].speeds[kk];

                /* compute x velocity component */
                float u_x = (cells[index].speeds[1] + cells[index].speeds[5] + cells[index].speeds[8] -
                             (cells[index].speeds[3] + cells[index].speeds[6] + cells[index].speeds[7])) /
                            local_density;
                /* compute y velocity component */
                float u_y = (cells[index].speeds[2] + cells[index].speeds[5] + cells[index].speeds[6] -
                             (cells[index].speeds[4] + cells[index].speeds[7] + cells[index].speeds[8])) /
                            local_density;

                /* velocity squared */
                float u_sq = u_x * u_x + u_y * u_y;
                __m256 f3 = _mm256_set1_ps(u_sq / 2.f / c_sq);

                /* directional velocity components */
                __m256 u_m = _mm256_set_ps(u_x - u_y, -u_x - u_y, -u_x + u_y, u_x + u_y, -u_y, -u_x, u_y, u_x);

                /* weighting factor */
                __m256 w_m = _mm256_set_ps(w2, w2, w2, w2, w1, w1, w1, w1);

                /* calculate densities */
                float d_equ0 = w0 * local_density * (1.f - u_sq / (2.f * c_sq));
                tmp_cells[index].speeds[0] = cells[index].speeds[0] + params.omega * (d_equ0 - cells[index].speeds[0]);
                __m256 d_equ_m = _mm256_mul_ps(_mm256_mul_ps(w_m, _mm256_set1_ps(local_density)),
                                               _mm256_add_ps(_mm256_add_ps(f1, _mm256_div_ps(u_m, c_sq_m)),
                                                             _mm256_sub_ps(_mm256_div_ps(_mm256_mul_ps(u_m, u_m), f2),
                                                                           f3)));

                /* relaxation step */
                _mm256_storeu_ps(tmp_cells[index].speeds + 1, _mm256_add_ps(_mm256_loadu_ps(cells[index].speeds + 1),
                                                                            _mm256_mul_ps(_mm256_set1_ps(params.omega),
                                                                                          _mm256_sub_ps(d_equ_m,
                                                                                                        _mm256_loadu_ps(
                                                                                                                cells[index].speeds +
                                                                                                                1)))));

            } else { /* if the cell contains an obstacle */
                /* For obstacles, mirror their speed. */
                /* called after collision, so taking values from scratch space
                ** mirroring, and writing into main grid */
                _mm256_storeu_ps(tmp_cells[index].speeds,
                                 _mm256_set_ps(cells[index].speeds[5], cells[index].speeds[8], cells[index].speeds[7],
                                               cells[index].speeds[2], cells[index].speeds[1], cells[index].speeds[4],
                                               cells[index].speeds[3], cells[index].speeds[0]));
                tmp_cells[index].speeds[8] = cells[index].speeds[6];
            }
        }
    }

    return EXIT_SUCCESS;
}

/*
** Particles flow to the corresponding cell according to their speed direction.
*/
int streaming(const t_param params, t_speed *cells, t_speed *tmp_cells) {
    const size_t x = params.nx - 1, y = params.ny - 1;
#pragma omp parallel for simd
    /* loop over _all_ cells */
    for (int jj = 0; jj < params.ny; ++jj) {
        int y_n = (jj + 1) % params.ny;
        int y_s = (jj + y) % params.ny;
        int y_nx = y_n * params.nx;
        int y_sx = y_s * params.nx;
        int jjx = jj * params.nx;
        for (int ii = 0; ii < params.nx; ++ii) {
            size_t index = ii + jjx;
            /* determine indices of axis-direction neighbours
            ** respecting periodic boundary conditions (wrap around) */
            int x_e = (ii + 1) % params.nx;
            int x_w = (ii + x) % params.nx;
            /* propagate densities from neighbouring cells, following
            ** appropriate directions of travel and writing into
            ** scratch space grid */
            _mm256_storeu_ps(cells[index].speeds,
                             _mm256_set_ps(tmp_cells[x_e + y_nx].speeds[7], tmp_cells[x_e + y_sx].speeds[6],
                                           tmp_cells[x_w + y_sx].speeds[5], tmp_cells[ii + y_nx].speeds[4],
                                           tmp_cells[x_e + jjx].speeds[3], tmp_cells[ii + y_sx].speeds[2],
                                           tmp_cells[x_w + jjx].speeds[1], tmp_cells[index].speeds[0]));
            cells[index].speeds[8] = tmp_cells[x_w + y_nx].speeds[8];
        }
    }

    return EXIT_SUCCESS;
}

/*
** Work with boundary conditions. The upper and lower boundaries use the rebound plane, 
** the left border is the inlet of fixed speed, and 
** the right border is the open outlet of the first-order approximation.
*/
int boundary(const t_param params, t_speed *cells, t_speed *tmp_cells, float *inlets) {
    /* Set the constant coefficient */
    const float cst1 = 2.0 / 3.0;
    const float cst2 = 1.0 / 6.0;
    const float cst3 = 1.0 / 2.0;

    // top wall (bounce)
    int jj = (params.ny - 1) * params.nx;
#pragma omp parallel for simd
    for (int ii = 0; ii < params.nx; ++ii) {
        size_t index = ii + jj;
        cells[index].speeds[4] = tmp_cells[index].speeds[2];
        cells[index].speeds[7] = tmp_cells[index].speeds[5];
        cells[index].speeds[8] = tmp_cells[index].speeds[6];
    }

    // bottom wall (bounce)
    // jj = 0
#pragma omp parallel for simd
    for (int ii = 0; ii < params.nx; ++ii) {
        cells[ii].speeds[2] = tmp_cells[ii].speeds[4];
        cells[ii].speeds[5] = tmp_cells[ii].speeds[7];
        cells[ii].speeds[6] = tmp_cells[ii].speeds[8];
    }

    // left wall (inlet)
    // ii = 0
#pragma omp parallel for simd
    for (int jj = 0; jj < params.ny; ++jj) {
        size_t index = jj * params.nx;
        float local_density = (cells[index].speeds[0] + cells[index].speeds[2] + cells[index].speeds[4] +
                               2.0 * cells[index].speeds[3] + 2.0 * cells[index].speeds[6] +
                               2.0 * cells[index].speeds[7]) / (1.0 - inlets[jj]);

        cells[index].speeds[1] = cells[index].speeds[3] + cst1 * local_density * inlets[jj];

        cells[index].speeds[5] = cells[index].speeds[7] - cst3 * (cells[index].speeds[2] - cells[index].speeds[4]) +
                                 cst2 * local_density * inlets[jj];

        cells[index].speeds[8] = cells[index].speeds[6] + cst3 * (cells[index].speeds[2] - cells[index].speeds[4]) +
                                 cst2 * local_density * inlets[jj];
    }

    // right wall (outlet)
    int ii = params.nx - 1;
#pragma omp parallel for simd
    for (int jj = 0; jj < params.ny; ++jj) {
        size_t index = ii + jj * params.nx;
        _mm256_storeu_ps(cells[index].speeds, _mm256_loadu_ps(cells[index - 1].speeds));
        cells[index].speeds[8] = cells[index - 1].speeds[8];
    }

    return EXIT_SUCCESS;
}
