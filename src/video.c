/*
    Copyright (C) 2011  EPFL (Ecole Polytechnique Fédérale de Lausanne)
    Pierluca Borsò <pierluca.borso@epfl.ch>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdarg.h>
#include <string.h>

#include <gst/gst.h>
#include <glib.h>
#include <gst/base/gstbasetransform.h>
#include <gst/app/gstappsink.h>

#include <SDL/SDL.h>

#include "dtk_video.h"
#include "vidpipe_creation.h"

#include "drawtk.h"
#include "texmanager.h"
#include "shapes.h"
#include "window.h"
#include "dtk_time.h"


struct dtk_pipeline {
	// pipeline object
	GstElement *pipe;
	GstBus *bus;
	GstAppSink* sink;

	// pipeline status
	pthread_t thread;
	int status;
	pthread_mutex_t status_lock;
};


static
void init_gstreamer(void)
{
	static bool is_initialized = false;
	if (!is_initialized) {
		// initialize glib threading
		if (!g_thread_supported()) 
			g_thread_init(NULL);
		// initialize gstreamer
		gst_init(NULL, NULL);

		// set sentinel
		is_initialized = true;
	}
}


static
int alloc_compatible_image(GstBuffer* buffer, struct dtk_texture* tex)
{
	GstCaps *caps;
	GstStructure *structure;
	int h, w;

	caps = gst_buffer_get_caps(buffer);
	if (!caps) {
		fprintf(stderr, "could not get caps for the buffer\n");
		return -1;
	}

	structure = gst_caps_get_structure(caps, 0);
	gst_structure_get_int(structure, "height", &h);
	gst_structure_get_int(structure, "width", &w);

	tex->intfmt = GL_RGB;
	tex->fmt = GL_RGB;
	tex->type = GL_UNSIGNED_BYTE;
	alloc_image_data(tex, w, h, 0, 24);

	gst_caps_unref(caps);

	return 0;
}


static
GstFlowReturn newbuffer_callback(GstAppSink *sink,  gpointer data)
{
	unsigned char *tdata, *bdata;
	unsigned int tstride, bstride, h, i;
	GstBuffer* buffer; 
	dtk_htex tex = data;
	struct dtk_pipeline* pl = tex->aux;

	pthread_mutex_lock(&(tex->lock));
	buffer = gst_app_sink_pull_buffer(sink);
	if (tex->sizes == NULL) 
		alloc_compatible_image(buffer, tex);

	if (pl->status == DTKV_READY)
		pl->status = DTKV_PLAYING;

	// load data into memory (gstreamer is flipped in GL conventions)
	h = tex->sizes[0].h;
	tstride = tex->sizes[0].stride;
	bstride = tex->sizes[0].w*3;
	tdata = tex->data[0];
	bdata = GST_BUFFER_DATA(buffer) + (h-1)*bstride;
	for (i=0; i<h; i++) {
		memcpy(tdata, bdata, bstride);
		tdata += tstride;
		bdata -= bstride;
	}

	gst_buffer_unref(buffer);
	tex->isinit = false;

	pthread_mutex_unlock(&(tex->lock));
	return GST_FLOW_OK;
}


/**************************************************************************
 *                           Pipeline creation                            *
 **************************************************************************/
static
void destroyPipeline(dtk_htex tex)
{
	struct dtk_pipeline* pl = tex->aux;

	// set pipeline status to dead
	pl->status = DTKV_STOPPED;

	// join the threads
	pthread_join(pl->thread, NULL);

	// lock status
	//pthread_mutex_lock(&(pl->status_lock));

	// kill bus
	gst_element_set_state(pl->pipe, GST_STATE_NULL);
	gst_object_unref(GST_OBJECT(pl->bus));

	// kill pipeline
	gst_object_unref(GST_OBJECT(pl->pipe));

	// unlock status
	//pthread_mutex_unlock(&(pl->status_lock));

	// delete dtk_pipeline
	pthread_mutex_destroy(&(pl->status_lock));
	free(tex->aux);

	tex->aux = NULL;
}


static
int init_video_tex(dtk_htex tex, GstElement* pipe)
{
	struct dtk_pipeline* pl;
	GstAppSink* sink;
	GstAppSinkCallbacks callbacks = {
		.new_buffer = newbuffer_callback
	};

	pl = malloc(sizeof(*pl));
	pl->pipe = pipe;
	pl->bus = gst_pipeline_get_bus(GST_PIPELINE(pipe));
	pl->status = DTKV_STOPPED;
	pthread_mutex_init(&(pl->status_lock), NULL);

	sink = GST_APP_SINK(gst_bin_get_by_name(GST_BIN(pipe), "dtksink"));
	gst_app_sink_set_callbacks(sink, &callbacks, tex, NULL);
	pl->sink = sink;

	tex->id = 0;
	tex->isvideo = true;
	tex->sizes = NULL;
	tex->aux = pl;
	tex->destroyfn = &(destroyPipeline);

	return 0;
}


static
dtk_htex create_video_any(const struct pipeline_opt* opt,
                          const char* stringid, int flags)
{
	dtk_htex tex;
	GstElement* pipe;

	if ((tex = get_texture(stringid)) == NULL)
		return NULL;

	pthread_mutex_lock(&(tex->lock));
	if (!tex->isinit) {
		init_gstreamer();
		pipe = create_pipeline(opt);
		init_video_tex(tex, pipe);
	}
	pthread_mutex_unlock(&(tex->lock));

	if (tex && (flags & DTK_AUTOSTART))
		dtk_video_exec(tex, DTKV_CMD_PLAY);
		
	return tex;
}


/**************************************************************************
 *                           Pipeline execution                           *
 **************************************************************************/
static
bool bus_callback(GstMessage * msg)
{
	GError *error;

	switch (GST_MESSAGE_TYPE(msg)) {
	case GST_MESSAGE_EOS:
		return false;

	case GST_MESSAGE_ERROR:
		gst_message_parse_error(msg, &error, NULL);
		fprintf(stderr, "Error: %s\n", error->message);
		g_error_free(error);
		return false;

	default:
		return true;
	}
}


static
void *run_pipeline_loop(void *arg)
{
	GstMessage *msg = NULL;
	struct dtk_pipeline* pl = arg;

	// main loop
	bool loop = true;
	while (loop && pl->status != DTKV_STOPPED) {
		if (pthread_mutex_trylock(&(pl->status_lock)) == 0) {
			// Read new messages from bus
			while (loop && (msg = gst_bus_pop(pl->bus))) {
				loop = bus_callback(msg);
				gst_message_unref(msg);
			}

			// if the bus hasn't requested termination, unlock 
			if (loop)
				pthread_mutex_unlock(&(pl->status_lock));
		}
	}

	// set status to STOPPED
	pl->status = DTKV_STOPPED;
	gst_element_set_state(pl->pipe, GST_STATE_READY);

	// if the bus HAS requested termination, unlock only now
	if (!loop)
		pthread_mutex_unlock(&(pl->status_lock));

	return NULL;
}


static
bool run_pipeline(struct dtk_pipeline* pl)
{
	bool paused;
	GstStateChangeReturn ret;
	struct dtk_timespec delay = { 0, 50000000 };	// 50 ms

	pthread_mutex_lock(&(pl->status_lock));

	if (pl->status == DTKV_PLAYING) {
		pthread_mutex_unlock(&(pl->status_lock));
		return true;
	}

	paused = (pl->status == DTKV_PAUSED);

	// prepare pipeline status
	pl->status = DTKV_READY;

	// execute state change and wait for state change to take effect
	ret = gst_element_set_state(pl->pipe, GST_STATE_PLAYING);
	while (ret == GST_STATE_CHANGE_ASYNC)
		ret = gst_element_get_state(pl->pipe, NULL, NULL,
					  500 * GST_MSECOND);

	// handle failure
	if (ret == GST_STATE_CHANGE_FAILURE) {
		pthread_mutex_unlock(&(pl->status_lock));
		fprintf(stderr, "failed to run pipeline\n");
		return false;
	}
	// handle case in which pipeline was simply paused
	if (paused) {
		pl->status = DTKV_PLAYING;
		pthread_mutex_unlock(&(pl->status_lock));
		return true;
	}

	pthread_mutex_unlock(&(pl->status_lock));

	// launch execution thread
	pthread_create(&(pl->thread), NULL, run_pipeline_loop, pl);

	// wait till the state changes
	while (pl->status == DTKV_READY)
		dtk_nanosleep(0, &delay, NULL);

	return (pl->status == DTKV_PLAYING);
}


static
void stop_pipeline(struct dtk_pipeline* pl)
{
	pthread_mutex_lock(&(pl->status_lock));

	pl->status = DTKV_STOPPED;
	pthread_join(pl->thread, NULL);

	pthread_mutex_unlock(&(pl->status_lock));
}


static
bool pause_pipeline(struct dtk_pipeline* pl)
{
	GstStateChangeReturn ret;
	pthread_mutex_lock(&(pl->status_lock));

	if (pl->status != DTKV_PLAYING) {
		pthread_mutex_unlock(&(pl->status_lock));
		return false;
	}
	// Change state and wait for state change to take effect
	ret = gst_element_set_state(pl->pipe, GST_STATE_PAUSED);
	while (ret == GST_STATE_CHANGE_ASYNC)
		ret = gst_element_get_state(pl->pipe, NULL, NULL,
					    500 * GST_MSECOND);

	if (ret == GST_STATE_CHANGE_FAILURE) {
		pthread_mutex_unlock(&(pl->status_lock));
		return false;
	}
	// set pipeline status to paused
	pl->status = DTKV_PAUSED;

	pthread_mutex_unlock(&(pl->status_lock));

	return true;
}


/**************************************************************************
 *                    Video related API implementation                    *
 **************************************************************************/
API_EXPORTED
dtk_htex dtk_create_video_tcp(int flags, const char *server, int port)
{
	struct pipeline_opt opt = {.type=VTCP, .str = server, .port = port};
	char stringid[255];

	if (port < 1 || !server)
		return NULL;

	sprintf(stringid, "TCP:%s:%d", server, port);
	return create_video_any(&opt, stringid, flags);
}


API_EXPORTED
dtk_htex dtk_create_video_udp(int flags, int port)
{
	struct pipeline_opt opt = {.type=VUDP, .port = port};
	char stringid[255];

	if (port < 1)
		return NULL;

	sprintf(stringid, "UDP:%d", port);
	return create_video_any(&opt, stringid, flags);
}


API_EXPORTED
dtk_htex dtk_create_video_file(int flags, const char *file)
{
	struct pipeline_opt opt = {.type=VFILE, .str=file};
	char stringid[255];

	if (!file)
		return NULL;

	sprintf(stringid, "FILE:%s", file);
	return create_video_any(&opt, stringid, flags);
}


API_EXPORTED
dtk_htex dtk_create_video_test(int flags)
{
	struct pipeline_opt opt = {.type=VTEST};
	return create_video_any(&opt, "TESTPIPE", flags);
}


API_EXPORTED
int dtk_video_getstate(dtk_htex video)
{
	struct dtk_pipeline* pl = video->aux;

	return pl->status;
}


API_EXPORTED
bool dtk_video_exec(dtk_htex video, int command)
{
	struct dtk_pipeline* pl = video->aux;

	switch (command) {
	case DTKV_CMD_STOP:
		stop_pipeline(pl);
		return true;

	case DTKV_CMD_PLAY:
		return run_pipeline(pl);

	case DTKV_CMD_PAUSE:
		return pause_pipeline(pl);

	default:
		return false;
	}
}
