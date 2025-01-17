/****************************************************************************
 *
 *   Copyright (c) 2012-2016 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/




/**
 * @file obstacle_avoidance_main.c
 * test application.
 */
#include <px4_config.h>
#include <px4_tasks.h>
#include <px4_posix.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <px4_log.h>
#include <stdio.h>
#include <errno.h>
#include <poll.h>
#include <uORB/topics/distance_sensor.h>
#include <uORB/uORB.h>
#include <uORB/topics/parameter_update.h>
 

__EXPORT int obstacle_avoidance_main(int argc, char *argv[]);

  
int obstacle_avoidance_main(int argc, char *argv[])
{
 
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int _range_finder_subs = orb_subscribe(ORB_ID(distance_sensor));
orb_set_interval(_range_finder_subs, 200);
/* one could wait for multiple topics with this technique, just using one here */
px4_pollfd_struct_t fds[] = { { .fd = _range_finder_subs,   .events = POLLIN },};
int error_counter = 0;
int i;

 for ( i = 0; i < 180; i++) 
{
/* wait for sensor update of 1 file descriptor for 1000 ms (1 second) */
 int poll_ret = px4_poll(fds, 1, 1000);

  /* handle the poll result */
  if (poll_ret == 0) 
  {
            /* this means none of our providers is giving us data */
     PX4_ERR("Got no data within a second");

  }
  else if (poll_ret<0)
  {
  	if (error_counter < 10 || error_counter % 50 == 0)
  	{
  	/* use a counter to prevent flooding (and slowing us down) */
      PX4_ERR("ERROR return value from poll(): %d", poll_ret);	
  	}
  	error_counter++;
  }
  else
  {
  	if (fds[0].revents & POLLIN)
  	{
  	struct distance_sensor_s range_finder;
    orb_copy(ORB_ID(distance_sensor), _range_finder_subs, &range_finder);	
    PX4_INFO("Obstacle Distnace = \t%8.4f",(double)range_finder.current_distance);
  	}
  }

 }

	return OK;

}


