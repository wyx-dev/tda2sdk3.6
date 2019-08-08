#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "vxlib.h"
#include "vlib.h"
#include "vxlib_cn.h"



typedef struct TBKRect {
    float left;
    float right;
    float top;
    float bottom;
} TBKRect;

typedef struct TBKTracker{
  TBKRect rect;
  TBKRect calc_roi;
  int32_t pre_width;
  int32_t pre_height;
  int32_t live;
  int32_t track_times;
  uint8_t init;
  uint8_t track_on;
  uint8_t *pre_resized_roi_frame;
  int32_t points_num;
  int32_t roi_store_size;
  float *points;
}TBKTracker;

typedef struct {
    uint8_t  *staticIn;
    float    *warpMatrixIn;
    uint8_t  *staticOut;
    uint16_t srcWidth;
    uint16_t srcHeight;
    uint16_t srcStride;
    uint16_t dstWidth;
    uint16_t dstHeight;
    uint16_t dstStride;
    uint8_t  borderValue;
} warpAffineBilinear_bc_i8u_c32f_o8u_Params_t;

typedef struct {
    uint16_t      *staticIn;
    const uint8_t *staticOut;
    int32_t       *static_good_points_result;
    int32_t        width;
    int32_t        height;
    int16_t        thresh;
    uint8_t        patch;
    uint8_t        qualitypar;
    int16_t        topNfeatures;
    uint8_t        mindist;
} goodFeatures_Params_t;



int DSPF_sp_svd_diy(const int Nrows,const int Ncols,float *restrict A,float *restrict U,float *restrict V,float *restrict U1,float *restrict diag,float *restrict superdiag);


VXLIB_STATUS VXLIB_trackFeaturesLK_i8u_diy(const uint8_t old_image[restrict],
                                                   const VXLIB_bufParams2D_t *old_image_addr,
                                                   const uint8_t new_image[restrict],
                                                   const VXLIB_bufParams2D_t *new_image_addr,
                                                   const int16_t old_gradX[restrict],
                                                   const VXLIB_bufParams2D_t *old_gradX_addr,
                                                   const int16_t old_gradY[restrict],
                                                   const VXLIB_bufParams2D_t *old_gradY_addr,
                                                   __float2_t old_points[restrict],
                                                   __float2_t new_points[restrict],
                                                   uint8_t tracking_status[restrict],
                                                   uint32_t num_points,
                                                   uint32_t max_iters,
                                                   VXLIB_F32 epsilon,
                                                   VXLIB_F32 scale,
                                                   uint8_t window_size,
                                                   uint8_t level,
                                                   uint8_t termination,
                                                   uint8_t scratch[restrict],
                                                   uint32_t scratch_size);

VXLIB_STATUS    VXLIB_trackFeaturesLK_i8u_checkParams_diy(const uint8_t old_image[restrict],
                                                   const VXLIB_bufParams2D_t *old_image_addr,
                                                   const uint8_t new_image[restrict],
                                                   const VXLIB_bufParams2D_t *new_image_addr,
                                                   const int16_t old_gradX[restrict],
                                                   const VXLIB_bufParams2D_t *old_gradX_addr,
                                                   const int16_t old_gradY[restrict],
                                                   const VXLIB_bufParams2D_t *old_gradY_addr,
                                                   const __float2_t old_points[restrict],
                                                   const __float2_t new_points[restrict],
                                                   const uint8_t tracking_status[restrict],
                                                   uint32_t num_points,
                                                   uint32_t max_iters,
                                                   VXLIB_F32 epsilon,
                                                   VXLIB_F32 scale,
                                                   uint8_t window_size,
                                                   uint8_t level,
                                                   uint8_t termination,
                                                   const uint8_t scratch[restrict],
                                                   uint32_t scratch_size);







float max_(float a, float b);
float min_(float a, float b);
int32_t max_I32(int32_t a, int32_t b);
int32_t min_I32(int32_t a, int32_t b);
void swap_(float* a, float* b, int32_t size);
void swap_f(float* a, float* b);
void swap_f(float* a, float* b);
void swap_U16(uint16_t* a, uint16_t* b);

void get_bounding_rect(TBKTracker *tracker, float* points, TBKRect *rect, uint8_t* tracking_status);
void update_car_roi(TBKTracker *tracker, TBKRect *car, int32_t img_rows, int32_t img_cols);
void get_car_resized_roi_img(TBKTracker *tracker, uint8_t* frame, int32_t frame_width, int32_t frame_height, uint8_t* target_image,float* x_scale, float* y_scale, int32_t* resize_width, int32_t* resize_height);
void get_rect_sample_points(TBKRect* rect, float points[][2]);
void perform_affine_transform(float affine_matrix[][3], float point[], float *x, float *y);

/********* sort by heap*************/
void perc_down(uint16_t* array, int32_t i, uint32_t K);
void sort_by_heap(uint16_t* array, int32_t top_k, uint32_t im_size);
void perc_down_f(float* array, int32_t i, uint32_t K);
void sort_by_heap_f(float* array, int32_t top_k, uint32_t im_size);
void perc_down_withloc_f(float* array_score, float* array_loc, int32_t i, uint32_t K);
void sort_by_heap_withloc_f(float* array_score, float* array_loc, int32_t top_k, uint32_t im_size);
/************************************/

void update_car_rect(TBKTracker* tracker, float affine_matrix[3][3], float* x_scale, float* y_scale);

/************good features to track by lch ******************/
void warp_affine_by_linear_interpolation(uint8_t* src, uint8_t* target_buffer, float* affine_matrix, int32_t target_rows, int32_t target_cols);
void get_gradient(int32_t rows, int32_t cols, int32_t block_size, uint8_t* img, float* dx, float* dy);
uint8_t is_pos_valid(int32_t y, int32_t x, int32_t src_rows, int32_t src_cols);
uint8_t is_max_eigen_point_neibor(float* eigen_value_vector, int32_t pos, int32_t src_rows, int32_t src_cols);
void calc_min_eigenvalue(float* _cov, float *_dst, int32_t src_rows, int32_t src_cols);
int32_t get_pos(int32_t x, int32_t border);
void box_filter(float* src,float* dst, int32_t block_size, int32_t src_rows, int32_t src_cols);
void get_eigen_value_vec( uint8_t *src, float* eigen_values, int block_size, int32_t src_rows, int32_t src_cols);
int32_t good_features_to_track(uint8_t* src, float* dst, int32_t block_size, int32_t min_distance, int32_t points_num, int32_t src_rows, int32_t src_cols);
/******************************************************/

int32_t VLIB_harrisScore_7x7_f_diy(const int16_t *restrict gradX, const int16_t *restrict gradY, int32_t width, int32_t height, float *outm, float k, float *buffer);
void VLIB_goodFeaturestoTrack_diy(const uint16_t* im, uint8_t* out, int32_t width, int32_t height, uint32_t thresh, int32_t patch, uint8_t qualitypar, int16_t topNfeatures, uint8_t mindist, uint16_t* outTemp, int32_t *good_points_number, int16_t* pixIndex_gft_cn, uint16_t* internalBuf_cn, int32_t* ind_cn);

void update_car_previous_frame(TBKTracker* tracker, uint8_t* img, int32_t img_width, int32_t img_height);
uint8_t get_mapping_points_between_two_frames(TBKTracker* tracker, uint8_t* pre_img, uint8_t* cur_img, float* new_flow_points, uint8_t* tracking_status);
void getHmatrix(float pre_sample_points[][2], float cur_sample_points[][2], float affine_matirx[][3]);
uint8_t check_subset(float points[][2], uint32_t model_points);
uint8_t get_subset(float* pre_points, float* cur_points, float pre_set[][2], float cur_set[][2], int points_num, uint32_t model_points, uint64_t* seed);
int32_t find_inliers(float* pre_points, float* cur_points, float matrix[][3], int32_t points_num, uint8_t* mask, float THRESHOLD);
int32_t RANSAC_update_numiters(float p, float ep, int32_t modelPoints, int32_t maxIters);
void calcHmatrix(float* pre_points,float* cur_points, float matrix[][3], int32_t points_num);
void compress_elems(float* points, uint8_t* mask, float* subpoints, int32_t points_num);
void copy(float* src, float* dst, int32_t rows, int32_t cols);
uint8_t levmarq_compute(float* presub, float* cursub, float* h, float* err, float* J, int32_t points_nm, int32_t J_rows);
float norm(float* src, int32_t src_rows, int32_t src_cols, uint8_t do_sqrt);
void diagonal(float* matrix, float* diag_vector, int32_t matrix_rows, int32_t matrix_cols);
void get_solution(float* A, float* X, float* b, int32_t rows, int32_t cols);
void get_inverse(float* A, float* A_, int32_t rows, int32_t cols);
void levmarq(float* presub, float* cursub, float matrix[][3], int32_t points_num);
void findHomography(TBKTracker* tracker, float* pre_points, float* cur_points, float THRESHOLD, float matirx[][3]);
void get_affine_matrix(TBKTracker *tracker, float* src, float* dst, uint8_t* tarcking_status, int32_t frame_height, int32_t frame_width, float affine_matrix[3][3]);
void track_by_klt(TBKTracker *tracker, uint8_t* img, TBKRect* rect_car, int32_t frame_width, int32_t frame_height);

/***********major module designed for pipeline*************************/
uint8_t init_tracklet(TBKTracker* tracker, uint8_t* img, TBKRect* rect, int32_t frame_width, int32_t frame_height);
uint8_t update_tracklet(TBKTracker* tracker, uint8_t* img, TBKRect* rect, int32_t frame_width, int32_t frame_height);
uint8_t tracklet_tracking(TBKTracker* tracker);
uint8_t optical_flow(TBKTracker* tracker, uint8_t* img, TBKRect* rect, int32_t frame_width, int32_t frame_height, uint8_t type);
/**********************************************************************/

/***********test case***************/
void test_warp_function();
void test_getHmatirx();
void test_homography();
void test_calcHmatrix();
void test_get_gradient(uint8_t* input_img, uint32_t src_cols, uint32_t src_rows);
/************************************/
