#include <jni.h>
#include "tslib.h"
#include <utils/Log.h>
#include <stdio.h>
#include <fcntl.h>

typedef struct {
    int x[5], xfb[5];
    int y[5], yfb[5];
    int a[7];
} calibration;

int perform_calibration(calibration *cal) {
    int j;
    float n, x, y, x2, y2, xy, z, zx, zy;
    float det, a, b, c, e, f, i;
    float scaling = 65536.0;

// Get sums for matrix
    n = x = y = x2 = y2 = xy = 0;
    for(j=0;j<5;j++) {
        n += 1.0;
        x += (float)cal->x[j];
        y += (float)cal->y[j];
        x2 += (float)(cal->x[j]*cal->x[j]);
        y2 += (float)(cal->y[j]*cal->y[j]);
        xy += (float)(cal->x[j]*cal->y[j]);
    }

// Get determinant of matrix -- check if determinant is too small
    det = n*(x2*y2 - xy*xy) + x*(xy*y - x*y2) + y*(x*xy - y*x2);
    if(det < 0.1 && det > -0.1) {
        printf("ts_calibrate: determinant is too small -- %f\n",det);
        return 0;
    }

// Get elements of inverse matrix
    a = (x2*y2 - xy*xy)/det;
    b = (xy*y - x*y2)/det;
    c = (x*xy - y*x2)/det;
    e = (n*y2 - y*y)/det;
    f = (x*y - n*xy)/det;
    i = (n*x2 - x*x)/det;

// Get sums for x calibration
    z = zx = zy = 0;
    for(j=0;j<5;j++) {
        z += (float)cal->xfb[j];
        zx += (float)(cal->xfb[j]*cal->x[j]);
        zy += (float)(cal->xfb[j]*cal->y[j]);
    }

// Now multiply out to get the calibration for framebuffer x coord
    cal->a[0] = (int)((a*z + b*zx + c*zy)*(scaling));
    cal->a[1] = (int)((b*z + e*zx + f*zy)*(scaling));
    cal->a[2] = (int)((c*z + f*zx + i*zy)*(scaling));

    printf("%f %f %f\n",(a*z + b*zx + c*zy),
    (b*z + e*zx + f*zy),(c*z + f*zx + i*zy));

// Get sums for y calibration
    z = zx = zy = 0;
    for(j=0;j<5;j++) {
        z += (float)cal->yfb[j];
        zx += (float)(cal->yfb[j]*cal->x[j]);
        zy += (float)(cal->yfb[j]*cal->y[j]);
    }

// Now multiply out to get the calibration for framebuffer y coord
    cal->a[3] = (int)((a*z + b*zx + c*zy)*(scaling));
    cal->a[4] = (int)((b*z + e*zx + f*zy)*(scaling));
    cal->a[5] = (int)((c*z + f*zx + i*zy)*(scaling));

    printf("%f %f %f\n",(a*z + b*zx + c*zy),
    (b*z + e*zx + f*zy),(c*z + f*zx + i*zy));

// If we got here, we're OK, so assign scaling to a[6] and return
    cal->a[6] = (int)scaling;
    return 1;

}

/*
    This function uses the Java Native Interface (JNI) to allow a
    calibration application in Java to communicate and utilize
    the tslib which is written in c.
*/
JNIEXPORT jint JNICALL \
Java_touchscreen_test_CalibrationTest_calibrateAndroid \
(JNIEnv *env, jclass this, jintArray incomingCoords)
{
    //Create the cal file with the coords obtained from java app
    calibration cal;
    char *calfile = NULL;
    jint i, j;
    char cal_buffer[256];
    int cal_fd;

    int* elements = (*env)->GetIntArrayElements(env, incomingCoords, 0);

    LOGV("tslib: JNI fn Java_touchscreen_test_CalibrationTest_calibrateAndroid start\n");
    //Assigning and filling the calibration struct
    j = 0;
    for(i=0; i<((*env)->GetArrayLength(env, incomingCoords))/2; i+=2){
        cal.x[j] =  elements[i];
        cal.y[j] =  elements[i+1];
        cal.xfb[j] = elements[i+10];
        cal.yfb[j] = elements[i+11];

        if(0) {
            LOGI("Value of X[%d]=%d in ts_Calibrate\n", j, cal.x[i/2]);
            LOGI("Value of Y[%d]=%d in ts_Calibrate\n", j, cal.y[i/2]);
            LOGI("Value of refX[%d]=%d in ts_Calibrate\n", j, cal.xfb[i/2]);
            LOGI("Value of refY[%d]=%d in ts_Calibrate\n", j, cal.yfb[i/2]);
        }
        j++;
    }
    LOGV("tslib: going to call perform_calibration() \n");
    if (perform_calibration (&cal)) {
        LOGV("tslib:perform_calibration succeeded\n ");

        printf ("Calibration constants: ");
        for (i = 0; i < 7; i++) {
            printf("%d ", cal.a [i]);
        }
        printf("\n");

        if ((calfile = getenv("TSLIB_CALIBFILE")) != NULL) {
            cal_fd = open (calfile, O_CREAT | O_RDWR);
        }
        else {
            LOGV("tslib: Writing to pointer cal in touchscreen.test\n");
            cal_fd = open ("/data/data/touchscreen.test/files/pointercal", O_CREAT | O_RDWR);
        }

        sprintf (cal_buffer,"%d %d %d %d %d %d %d",
            cal.a[1], cal.a[2], cal.a[0],
            cal.a[4], cal.a[5], cal.a[3], cal.a[6]);
        write (cal_fd, cal_buffer, strlen (cal_buffer) + 1);
        close (cal_fd);
    }
    else {
        printf("Calibration failed.\n");
        i = -1;
    }
    return i;
}
