#ifndef _MATCHMODE_TIMING_RANGES_H_
#define _MATCHMODE_TIMING_RANGES_H_

enum timing_status {
	STATUS_UNKNOWN,		/* Daemon just started up */
	STATUS_OK,		/* Running in a supported mode */
	STATUS_DISCONNECTED,	/* No sink connected */
	STATUS_INVALID,		/* Source is running in an unsupported mode, or disconnected */
	STATUS_NOSOURCE,        /* No source connected */
};

struct timing_info {
	unsigned int hactive;
	unsigned int vactive;
	int htotal;
	unsigned int vtotal_pix;
	unsigned int vtotal_lines;
	unsigned int h_fp;
	unsigned int h_bp;
	unsigned int v_fp_pix;
	unsigned int v_fp_lines;
	unsigned int v_bp_pix;
	unsigned int v_bp_lines;
	unsigned int hsync_width;
	unsigned int vsync_width_pix;
	unsigned int vsync_width_lines;
	unsigned int refclock_count;
	unsigned int lcd_de_latency_lines;
	unsigned int lcd_vsync_latency_lines;
	double pixclk_in_MHz;
	double ns_per_pix;
	enum timing_status status;
};

struct timing_range {
	struct timing_info lower;
	struct timing_info upper;
	struct timing_info actual;
};

static struct timing_range mode_1 = {
	.actual = {
		.hactive = 640,
		.vactive = 480,

		.htotal = 800,
		.vtotal_lines = 525,

		.h_bp = 16,
		.h_fp = 48,
		.hsync_width = 96,

		.v_fp_lines = 10,
		.v_bp_lines = 33,
		.vsync_width_lines = 2,

		.pixclk_in_MHz = 25.175,

		.status = STATUS_OK,
	},
};

static struct timing_range mode_2 = {
	.actual = {
		.hactive = 720,
		.vactive = 480,

		.htotal = 858,
		.vtotal_lines = 525,

		.h_fp = 46,
		.h_bp = 16,
		.hsync_width = 76,

		.v_fp_lines = 9,
		.v_bp_lines = 30,
		.vsync_width_lines = 6,

		.pixclk_in_MHz = 27.0,

		.status = STATUS_OK,
	},
};

static struct timing_range mode_4 = {
	.actual = {
		.hactive = 1280,
		.vactive = 720,

		.htotal = 1650,
		.vtotal_lines = 750,

		.h_fp = 110,
		.h_bp = 220,
		.hsync_width = 40,

		.v_fp_lines = 20,
		.v_bp_lines = 5,
		.vsync_width_lines = 5,

		.pixclk_in_MHz = 74.176,

		.status = STATUS_OK,
	},
};

static struct timing_range mode_32 = {
	.actual = {
		.hactive = 1920,
		.vactive = 1080,

		.htotal = 2750,
		.vtotal_lines = 1125,

		.h_fp = 638,
		.h_bp = 148,
		.hsync_width = 44,

		.v_fp_lines = 4,
		.v_bp_lines = 36,
		.vsync_width_lines = 5,

		.pixclk_in_MHz = 74.176,

		.status = STATUS_OK,
	},
};

static struct timing_range *timings[] = {
	&mode_1,
	&mode_2,
	&mode_4,
	&mode_32,
	NULL,
};


#endif /* _MATCHMODE_TIMING_RANGES_H_ */
