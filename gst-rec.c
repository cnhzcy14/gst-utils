#include <gst/gst.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    GstElement *pipeline;
    GstBus *bus;
    GstClock *clock;
    GstMessage *msg;
    const GstStructure *msmsg;
    GstClockTime base_time;
    // std::fstream file;
    FILE *file;
    int opt;
    const char *csv_location;
    const char *lunch_str;

    /* Initialize GStreamer */
    gst_init(&argc, &argv);

    while ((opt = getopt(argc, argv, "l:f:")) > 0)
    {
        switch (opt)
        {
        case 'l':
            lunch_str = strdup(optarg);
            break;
        case 'f':
            csv_location = strdup(optarg);
            break;
        default:
            g_print("usage: -l pipeline -f csv location\n");
            g_print("example pipelines:\n");
            g_print("'v4l2src device=/dev/video2 ! video/x-raw,width=640,height=480,framerate=30/1 ! pngenc compression-level=0 multifilesink post-messages=true location=%%08d.png\n");
            g_print("'v4l2src device=/dev/video0 ! video/x-raw,format=NV12,width=480,height=640,framerate=25/1 ! queue2 ! videoconvert ! video/x-raw,format=GRAY8,framerate=25/1 ! queue2 ! pngenc compression-level=0 ! multifilesink post-messages=true location=/userdata/test/%%08d.png sync=true'\n");
            return 0;
        }
    }

    file = fopen(csv_location, "w");
    if (file == NULL)
    {
        printf("Error! Could not open file\n");
        exit(-1); // must include stdlib.h
    }
    setlinebuf(file);

    /* Build the pipeline */
    pipeline = gst_parse_launch(lunch_str, NULL);

    /* Start playing */
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    /* Wait until error or EOS */

    base_time = gst_element_get_base_time(pipeline);
    bus = gst_element_get_bus(pipeline);
    while (1)
    {
        msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
                                         (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS | GST_MESSAGE_ELEMENT));
        /* Parse message */
        if (msg != NULL)
        {
            GError *err;
            gchar *debug_info;

            switch (GST_MESSAGE_TYPE(msg))
            {
            case GST_MESSAGE_ERROR:
                gst_message_parse_error(msg, &err, &debug_info);
                g_printerr("Error received from element %s: %s\n",
                           GST_OBJECT_NAME(msg->src), err->message);
                g_printerr("Debugging information: %s\n",
                           debug_info ? debug_info : "none");
                g_clear_error(&err);
                g_free(debug_info);
                break;
            case GST_MESSAGE_EOS:
                g_print("End-Of-Stream reached.\n");
                break;
            case GST_MESSAGE_ELEMENT:
                msmsg = gst_message_get_structure(msg);
                if (gst_structure_has_name(msmsg, "GstMultiFileSink"))
                {
                    guint64 timestamp;
                    int idx;

                    gst_structure_get_uint64(msmsg, "timestamp", &timestamp);
                    gst_structure_get_int(msmsg, "index", &idx);

                    fprintf(file, "%ld,%08d.png\n", timestamp + base_time, idx);
                    GST_INFO("ID: %d; Timestamp: %ld\n", idx, timestamp + base_time);
                }
                break;
            default:
                /* We should not reach here because we only asked for ERRORs and EOS */
                g_printerr("Unexpected message received.\n");
                break;
            }
            gst_message_unref(msg);
        }
    }

    fclose(file);
    /* Free resources */
    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    
    return 0;
}
