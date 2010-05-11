/*
 *  Copyright (C) 2010 Kan-Ru Chen <kanru@0xlab.org>
 *
 * This file is placed under the LGPL.  Please see the file
 * COPYING for more details.
 *
 */
#include "config.h"

#include "tslib-private.h"

int ts_reload(struct tsdev *ts)
{
    int result = 0;
    result = ts->list->ops->reload(ts->list, ts);
    return result;
}
