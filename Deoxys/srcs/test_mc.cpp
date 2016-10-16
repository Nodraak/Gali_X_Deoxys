
#include "MotionController.h"
#include "utils.h"

#include "test.h"
#include "test_mc.h"


void test_mc(Debug *debug) {
    float angle;
    int16_t x, y;

    int diff_l = 0, diff_r = 0;
    float cur_angle = 0;
    int cur_x = 0, cur_y = 0;

    /*
        Stand still
    */

    // stand still
    calcNewPos(diff_l, diff_r, cur_angle, cur_x, cur_y, &angle, &x, &y);
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
    calcNewPos(diff_l, diff_r, cur_angle, cur_x, cur_y, &angle, &x, &y);
    ft_assert(angle == 0);
    ft_assert(x > 0);
    ft_assert(y == 0);

    // move forward to the east - right
    calcNewPos(diff_l+10, diff_r, cur_angle, cur_x, cur_y, &angle, &x, &y);
    ft_assert(angle < 0);
    ft_assert(x > 0);
    ft_assert(y < 0);

    // move forward to the east - left
    calcNewPos(diff_l, diff_r+10, cur_angle, cur_x, cur_y, &angle, &x, &y);
    ft_assert(angle > 0);
    ft_assert(x > 0);
    ft_assert(y > 0);
    ft_assert(x > y);

    // move backward to the east (therefore to the west)
    calcNewPos(-diff_l, -diff_r, cur_angle, cur_x, cur_y, &angle, &x, &y);
    ft_assert(angle == 0);
    ft_assert(x < 0);
    ft_assert(y == 0);

    /*
        angle == 45 deg (facing north east)
    */

    cur_angle = DEG2RAD(45);

    // move forward (to the north-east)
    calcNewPos(diff_l, diff_r, cur_angle, cur_x, cur_y, &angle, &x, &y);
    ft_assert_equal(angle, DEG2RAD(45));
    ft_assert(x > 0);
    ft_assert(y > 0);
    ft_assert(x == y);

    // move forward to the north-east - down
    calcNewPos(diff_l+10, diff_r, cur_angle, cur_x, cur_y, &angle, &x, &y);
    ft_assert(angle > 0);
    ft_assert(x > 0);
    ft_assert(y > 0);
    ft_assert(x > y);

    // move forward to the north-east - up
    calcNewPos(diff_l, diff_r+10, cur_angle, cur_x, cur_y, &angle, &x, &y);
    ft_assert(angle > 0);
    ft_assert(x > 0);
    ft_assert(y > 0);
    ft_assert(y > x);

    // move backward to the north-east (therefore to the south-west)
    calcNewPos(-diff_l, -diff_r, cur_angle, cur_x, cur_y, &angle, &x, &y);
    ft_assert_equal(angle, DEG2RAD(45));
    ft_assert(x < 0);
    ft_assert(y < 0);
    ft_assert(x == y);

/*
    Moving in curve
*/

    // TODO

}
