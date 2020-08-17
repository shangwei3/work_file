void main_loop (void)
{
	char *s;
	int bootdelay;

	u_boot_hush_start ();

	s = getenv ("bootdelay");//bootdelay="5" 或 NULL
	bootdelay = s ? (int)simple_strtol(s, NULL, 10) : CONFIG_BOOTDELAY;

	debug ("### main_loop entered: bootdelay=%d\n\n", bootdelay);


	s = getenv ("bootcmd");
	debug ("### main_loop: bootcmd=\"%s\"\n", s ? s : "<UNDEFINED>");


	//到计时，判断用户有没有输入，如果在倒计时时间内没有输入,则执行bootcmd指定的命令
	if (bootdelay >= 0 && s && !abortboot (bootdelay)) {
		parse_string_outer(s, FLAG_PARSE_SEMICOLON |
				FLAG_EXIT_FROM_LOOP);
	}

	/*
	 * Main Loop for Monitor Command Processing
	 */	
	parse_file_outer();
	/* This point is never reached */
	for (;;);
}


static __inline__ int abortboot(int bootdelay)
{
	int abort = 0;

	printf("Hit any key to stop autoboot: %2d ", bootdelay);

	/*
	 * Check if key already pressed
	 * Don't check if bootdelay < 0
	 */
	if (bootdelay >= 0) {
		if (tstc()) {	/* we got a key press	*/
			(void) getc();  /* consume input	*/
			puts ("\b\b\b 0");
			abort = 1;	/* don't auto boot	*/
		}
	}

	while ((bootdelay > 0) && (!abort)) {
		int i;

		--bootdelay;
		/* delay 100 * 10ms */
		for (i=0; !abort && i<100; ++i) {
			if (tstc()) {	/* we got a key press	*/
				abort  = 1;	/* don't auto boot	*/
				bootdelay = 0;	/* no more delay	*/
				(void) getc();  /* consume input	*/
				break;
			}
			udelay(10000);
		}

		printf("\b\b\b%2d ", bootdelay);
	}

	putc('\n');

	return abort;
}
