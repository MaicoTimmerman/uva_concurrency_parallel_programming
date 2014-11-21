/*
 * simulate.h
 */

#pragma once

double *simulate(const int i_max, const int t_max, double *old_array,
        double *current_array, double *next_array,
        const int num_taks, const int task_id);
