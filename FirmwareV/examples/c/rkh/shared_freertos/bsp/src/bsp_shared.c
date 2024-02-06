/**
 * \cond
 *  --------------------------------------------------------------------------
 *
 *                                Framework RKH
 *                                -------------
 *
 * 	          State-machine framework for reactive embedded systems
 *
 * 	                    Copyright (C) 2010 Leandro Francucci.
 * 	        All rights reserved. Protected by international copyright laws.
 *
 *
 * 	RKH is free software: you can redistribute it and/or modify it under the
 * 	terms of the GNU General Public License as published by the Free Software
 * 	Foundation, either version 3 of the License, or (at your option) any
 * 	later version.
 *
 *  RKH is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 *  more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with RKH, see copying.txt file.
 *
 *  Contact information:
 *  RKH site: http://vortexmakes.com/que-es/
 *  RKH GitHub: https://github.com/vortexmakes/RKH
 *  RKH Sourceforge: https://sourceforge.net/projects/rkh-reactivesys/
 *  e-mail: lf@vortexmakes.com
 *  --------------------------------------------------------------------------
 *  File                     : bsp.c
 *	Last updated for version : v2.4.04
 *	By                       : DB
 *  --------------------------------------------------------------------------
 *  \endcond
 *
 * 	\file
 * 	\ingroup 	prt
 *
 * 	\brief 		BSP for FRDK64F120 using Freescale OSA from KSDK
 */

#include "rkh.h"
#include "bsp.h"
#include "server.h"
#include "client.h"
#include "shared.h"
#include "sapi.h"
//#include "fsl_debug_console.h"
//#include "switch.h"
#include "board.h"
//#include "boardext.h"
//#include "os_cfg_app.h"


typedef enum
{
    RGB_BLACK,
    RGB_WHITE,
    RGB_RED,
    RGB_LIME,
    RGB_BLUE,
    RGB_YELLOW,
    RGB_CYAN,
    RGB_MAGENTA
} RGB_COLOR_IDX;

#define SERIAL_TRACE				0

#define SIZEOF_EP0STO				32
#define SIZEOF_EP0_BLOCK			sizeof( RKH_EVT_T )
#define SIZEOF_EP1STO				16
#define SIZEOF_EP1_BLOCK			sizeof( ReqEvt )
#define SVR_NAME					"Server    -"
#define CLI_NAME					"Client"


DEBUG_PRINT_ENABLE

/* This macro is needed only if the module requires to check 	.. */
/* .. expressions that ought to be true as long as the program  .. */
/* .. is running. 												   */

RKH_THIS_MODULE


static rui32_t l_rnd;				/* random seed */
static rui8_t ep0sto[ SIZEOF_EP0STO ], ep1sto[ SIZEOF_EP1STO ];

#if defined( RKH_USE_TRC_SENDER )
static rui8_t l_isr_kbd;
#endif

typedef struct
{
    uint32_t r;
    uint32_t g;
    uint32_t b;
} RGB_COLOR_ST;

static RGB_COLOR_ST rgb_colors[] = {	{ 0, 0, 0 },
    { 1, 1, 1 },
    { 1, 0, 0 },
    { 0, 1, 0 },
    { 0, 0, 1 },
    { 1, 1, 0 },
    { 0, 1, 1 },
    { 1, 0, 1 }
};

void
set_rgb_led( RGB_COLOR_IDX idx )
{
    ( rgb_colors[idx].r == 1 ) ?  gpioWrite( LEDR, FALSE ) :
    gpioWrite( LEDR,TRUE );
    ( rgb_colors[idx].g == 1 ) ?  gpioWrite( LEDG, FALSE ) :
    gpioWrite( LEDG,TRUE );
    ( rgb_colors[idx].b == 1 ) ?  gpioWrite( LEDB, FALSE ) :
    gpioWrite( LEDB,TRUE );
}



/*
 * 	For serial trace feature.
 */

#if SERIAL_TRACE == 1
#define SERIAL_TRACE_OPEN()		rkhtrc_uart_init()
#define SERIAL_TRACE_CLOSE() 	rkhtrc_uart_deinit()
#define SERIAL_TRACE_SEND_BLOCK( buf_, len_ )	\
									rkhtrc_send_block( buf_, len_ )
#else
#define SERIAL_TRACE_OPEN()						(void)0
#define SERIAL_TRACE_CLOSE()					(void)0
#define SERIAL_TRACE_SEND( d )					(void)0
#define SERIAL_TRACE_SEND_BLOCK( buf_, len_ )	(void)0
#endif

#ifdef RKH_DEBUG
#define reset_now()		__asm volatile	("	bkpt 0x00FF\n" )
#else
#define reset_now()		cpu_reset()
#endif

void bsp_publish( const RKH_EVT_T *e )
{
    rInt cn;

    RKH_SMA_POST_FIFO( server, e, &l_isr_kbd );			/* to server */

    for( cn = 0; cn < NUM_CLIENTS; ++cn )				/* to clients */
    {
        RKH_SMA_POST_FIFO( CLI( cn ), e, &l_isr_kbd );
    }
}




void rkh_hook_timetick( void )
{
    switch_tick();
}


void rkh_hook_start( void )
{
    rkh_fwk_registerEvtPool( ep0sto, SIZEOF_EP0STO, SIZEOF_EP0_BLOCK  );
    rkh_fwk_registerEvtPool( ep1sto, SIZEOF_EP1STO, SIZEOF_EP1_BLOCK  );
}


void rkh_hook_exit( void )
{
    RKH_TRC_FLUSH();
}


void rkh_assert( RKHROM char * const file, int line )
{
    ( void )line;

    RKH_DIS_INTERRUPT();
    RKH_TR_FWK_ASSERT( ( RKHROM char * )file, __LINE__ );
    rkh_fwk_exit();
    //	reset_now();
}


static void print_banner( void )
{
    printf(	"\"Shared\" example\n\n" );
    printf(	"RKH version      = %s\n", RKH_RELEASE );
    printf(	"Port version     = %s\n", rkhport_get_version() );
    printf(	"Port description = %s\n\n", rkhport_get_desc() );
    printf(	"Description: \n\n" );
    printf(	"This application deals with the shared resource problem \n" );
    printf(	"in active object systems. Showing one of the biggest \n" );
    printf(	"benefit of using active objects: resource encapsulation. \n" );
    printf(	"The encapsulation naturally designates the owner of the \n" );
    printf(	"resource as the ultimate arbiter in resolving any contention \n" );
    printf(	"and potential conflicts for the resource. \n" );
    printf(	"The SHD application is relatively simple and can be tested \n" );
    printf(	"only with a couple of LEDs on your target board. \n" );
    printf(	"Still, SHD contains five (5) concurrent active objects \n" );
    printf(	"that exchange events via direct event posting mechanism. \n" );
    printf(	"The application uses four timers, as well as dynamic  \n" );
    printf(	"and static events. \n" );
    printf(	"On the other hand, this application could be used in either \n" );
    printf(	"preemptive or cooperative enviroment. \n" );
    printf(	"Aditionally, the SHD could be used to verify a new RKH port. \n" );
    printf(	"\n\n\n" );

    printf( "1.- Each Client have your own color, Client 1-4:\n" );
    printf( "	   	RED, GREEN, BLUE, YELLOW\n" );
    printf( "2.- Press SW2 to PAUSE.\n" );
    printf( "3.- Paused state is shown with a WHITE in RGB led\n." );
}


#if RKH_CFG_TRC_EN == RKH_ENABLED

void rkh_trc_open( void )
{
    rkh_trc_init();

    SERIAL_TRACE_OPEN();
    RKH_TRC_SEND_CFG( RKH_CFG_FWK_TICK_RATE_HZ );
}


void rkh_trc_close( void )
{
    SERIAL_TRACE_CLOSE();
}


RKH_TS_T
rkh_trc_getts( void )
{
    return ( RKH_TS_T ) xTaskGetTickCount();
}


void rkh_trc_flush( void )
{
    rui8_t *blk;
    TRCQTY_T nbytes;
    RKH_SR_ALLOC();

    FOREVER
    {
        nbytes = ( TRCQTY_T )1024;

        RKH_ENTER_CRITICAL_();
        blk = rkh_trc_get_block( &nbytes );
        RKH_EXIT_CRITICAL_();

        if( ( blk != ( rui8_t * )0 ) )
        {
            SERIAL_TRACE_SEND_BLOCK( blk, nbytes );
        }
        else
        {
            break;
        }
    }
}

#endif


rui32_t bsp_rand( void )
{
    /*
     * A very cheap pseudo-random-number generator.
     * "Super-Duper" Linear Congruential Generator (LCG)
     * LCG(2^32, 3*7*11*13*23, 0, seed) [MS]
     */
    l_rnd = l_rnd * ( 3*7*11*13*23 );
    return l_rnd >> 8;
}


void bsp_srand( rui32_t seed )
{
    l_rnd = seed;
}

static RGB_COLOR_IDX bsp_led_colors[] =
{
    /* Server paused */
    RGB_WHITE,

    /*
     * Client1 1-4 according
     * to clino arg
     */
    RGB_RED,
    RGB_LIME,
    RGB_BLUE,
    RGB_YELLOW
};



void bsp_cli_wait_req( rui8_t clino, RKH_TNT_T req_time )
{
    printf( "Client[%d] - Waiting for send request to server (%d seg)\n",
            CLI_ID( clino ), req_time );
}


void bsp_cli_req( rui8_t clino )
{
    printf( "Client[%d] - Send request to server...\n", CLI_ID( clino ) );
}

void bsp_cli_using( rui8_t clino, RKH_TNT_T using_time )
{
    printf( "Client[%d] - Using server for %d [seg]\n", CLI_ID( clino ), using_time );
    set_rgb_led( bsp_led_colors[clino] );
}


void bsp_cli_paused( rui8_t clino )
{
    printf( "Client[%d] - Paused\n", CLI_ID( clino ) );
    set_rgb_led( RGB_WHITE );
    gpioWrite( bsp_led_colors[0],TRUE );
}


void bsp_cli_resumed( rui8_t clino )
{
    printf( "Client[%d] - Resumed\n", CLI_ID( clino ) );
}


void bsp_cli_done( rui8_t clino )
{
    printf( "Client[%d] - Done\n", CLI_ID( clino ) );
    set_rgb_led( RGB_BLACK );
}


void bsp_svr_recall( rui8_t clino )
{
    printf( "%s Recall a deferred request from client[%d]\n",
            SVR_NAME, CLI_ID( clino ) );
}


void bsp_svr_paused( rui32_t ntot, rui32_t *ncr )
{
    rInt cn;
    rui32_t *pNcr;

    printf( "Server paused | " );
    printf( "ntot = %d |", ntot );

    for( pNcr = ncr, cn = 0; cn < NUM_CLIENTS; ++cn, ++pNcr )
    {
        printf( " cli%d=%d |", cn, *pNcr );
    }

    printf( '\n' );
}


void bsp_svr_start( rui8_t clino )
{
    ( void )0;
}


void bsp_svr_end( void )
{
}


void bsp_svr_resume( void )
{
}

void bsp_init( int argc, char *argv[]  )
{
    rInt cn;

    ( void )argc;
    ( void )argv;

    boardConfig();
    debugPrintConfigUart( UART_USB, 115200 );

    bsp_srand( 1234 );

    print_banner();

    rkh_fwk_init();

    RKH_FILTER_OFF_SMA( server );

    for( cn = 0; cn < NUM_CLIENTS; ++cn )
    {
        RKH_FILTER_OFF_SMA( CLI( cn ) );
    }

    RKH_FILTER_OFF_EVENT( RKH_TE_SMA_FIFO );
    RKH_FILTER_OFF_EVENT( RKH_TE_SMA_LIFO );
    RKH_FILTER_OFF_EVENT( RKH_TE_SM_DCH );
    RKH_FILTER_OFF_EVENT( RKH_TE_SM_STATE );

    /*RKH_FILTER_OFF_ALL_SIGNALS();*/
    RKH_FILTER_OFF_SIGNAL( REQ );
    RKH_FILTER_OFF_SIGNAL( START );

    RKH_TRC_OPEN();
}
