#ifndef DISABLE_TESTS

#include "common/Debug.h"
#include "common/utils.h"
#include "common/test.h"
#include "QBouge/MotionController.h"

#include "test_mc.h"


void test_mc_calcNewPos(Debug *debug) {
    float angle;
    float x, y;

    int diff_l = 0, diff_r = 0;
    float cur_angle = 0;
    float cur_x = 0, cur_y = 0;

/*
    Stand still
*/

    // stand still
    mc_calcNewPos(diff_l, diff_r, cur_angle, cur_x, cur_y, &angle, &x, &y);
    ft_assert(angle == 0);
    ft_assert(x == 0);
    ft_assert(y == 0);

/*
    Moving straight
*/
    diff_l = 100, diff_r = 100;

    /*
        angle == 0 deg (facing east)
    */

    cur_angle = 0;

    // move forward to the east
    mc_calcNewPos(diff_l, diff_r, cur_angle, cur_x, cur_y, &angle, &x, &y);
    ft_assert(angle == 0);
    ft_assert(x > 0);
    ft_assert(y == 0);

    // move forward to the east - right
    mc_calcNewPos(diff_l+10, diff_r, cur_angle, cur_x, cur_y, &angle, &x, &y);
    ft_assert(angle < 0);
    ft_assert(x > 0);
    ft_assert(y < 0);

    // move forward to the east - left
    mc_calcNewPos(diff_l, diff_r+10, cur_angle, cur_x, cur_y, &angle, &x, &y);
    ft_assert(angle > 0);
    ft_assert(x > 0);
    ft_assert(y > 0);
    ft_assert(x > y);

    // move backward to the east (therefore to the west)
    mc_calcNewPos(-diff_l, -diff_r, cur_angle, cur_x, cur_y, &angle, &x, &y);
    ft_assert(angle == 0);
    ft_assert(x < 0);
    ft_assert(y == 0);

    /*
        angle == 45 deg (facing north east)
    */

    cur_angle = DEG2RAD(45);

    // move forward (to the north-east)
    mc_calcNewPos(diff_l, diff_r, cur_angle, cur_x, cur_y, &angle, &x, &y);
    ft_assert_equal(angle, DEG2RAD(45));
    ft_assert(x > 0);
    ft_assert(y > 0);
    ft_assert(x == y);

    // move forward to the north-east - down
    mc_calcNewPos(diff_l+10, diff_r, cur_angle, cur_x, cur_y, &angle, &x, &y);
    ft_assert(angle > 0);
    ft_assert(x > 0);
    ft_assert(y > 0);
    ft_assert(x > y);

    // move forward to the north-east - up
    mc_calcNewPos(diff_l, diff_r+10, cur_angle, cur_x, cur_y, &angle, &x, &y);
    ft_assert(angle > 0);
    ft_assert(x > 0);
    ft_assert(y > 0);
    ft_assert(y > x);

    // move backward to the north-east (therefore to the south-west)
    mc_calcNewPos(-diff_l, -diff_r, cur_angle, cur_x, cur_y, &angle, &x, &y);
    ft_assert_equal(angle, DEG2RAD(45));
    ft_assert(x < 0);
    ft_assert(y < 0);
    ft_assert(x == y);

/*
    Moving in curve
*/

    // TODO

}


void test_mc_calcDistThetaOrderPos(Debug *debug) {
    float dist = 0, theta = 0;

    dist = 100;
    theta = DEG2RAD(179);
    mc_calcDistThetaOrderPos(&dist, &theta);
    ft_assert_equal(dist, -100);
    ft_assert_equal(theta, DEG2RAD(-1));

    dist = 100;
    theta = DEG2RAD(135);
    mc_calcDistThetaOrderPos(&dist, &theta);
    ft_assert_equal(dist, -100);
    ft_assert_equal(theta, DEG2RAD(-45));

    dist = 100;
    theta = DEG2RAD(91);
    mc_calcDistThetaOrderPos(&dist, &theta);
    ft_assert_equal(dist, -100);
    ft_assert_equal(theta, DEG2RAD(-89));


    dist = 100;
    theta = DEG2RAD(89);
    mc_calcDistThetaOrderPos(&dist, &theta);
    ft_assert_equal(dist, 100);
    ft_assert_equal(theta, DEG2RAD(89));

    dist = 100;
    theta = DEG2RAD(0);
    mc_calcDistThetaOrderPos(&dist, &theta);
    ft_assert_equal(dist, 100);
    ft_assert_equal(theta, DEG2RAD(0));

    dist = 100;
    theta = DEG2RAD(-89);
    mc_calcDistThetaOrderPos(&dist, &theta);
    ft_assert_equal(dist, 100);
    ft_assert_equal(theta, DEG2RAD(-89));


    dist = 100;
    theta = DEG2RAD(-91);
    mc_calcDistThetaOrderPos(&dist, &theta);
    ft_assert_equal(dist, -100);
    ft_assert_equal(theta, DEG2RAD(89));

    dist = 100;
    theta = DEG2RAD(-135);
    mc_calcDistThetaOrderPos(&dist, &theta);
    ft_assert_equal(dist, -100);
    ft_assert_equal(theta, DEG2RAD(45));

    dist = 100;
    theta = DEG2RAD(-179);
    mc_calcDistThetaOrderPos(&dist, &theta);
    ft_assert_equal(dist, -100);
    ft_assert_equal(theta, DEG2RAD(1));
}


void test_mc_updateCurOrder(Debug *debug) {
    s_vector_float cur_pos = {.x=1500, .y=1000};
    s_order cur_order = {.type=ORDER_TYPE_POS, .pos={.x=1000, .y=1000}, .angle=DEG2RAD(0), .delay=0};
    float cur_angle = DEG2RAD(0), time_since_last_order_finished = 1.000;
    int ret = 0;
    float dist = 0, theta = 0;

    ret = mc_updateCurOrder(
        cur_pos, cur_angle, &cur_order, time_since_last_order_finished,
        &dist, &theta
    );
    ft_assert(ret == 0);
    ft_assert(dist < 0);
    ft_assert_equal(theta, 0);

    // todo more tests
}

#endif // DISABLE_TESTS
