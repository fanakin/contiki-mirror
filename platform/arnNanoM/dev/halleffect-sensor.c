
#include "36064s.h"
#include "dev/halleffect-sensor.h"
#ifdef CONTIKI_TARGET_ARNNANOM
#include "printf.h" /* For printf() tiny*/
#else
#include <stdio.h> /* For printf() */
#endif

#define RESET_OV_TIMER1		TZ1.TSR.BIT.OVF=0
#define SET_TCR_TIMER1		TZ1.TCR.BIT.TPSC=0   /* Count rising edge, Tcnt is not cleared, Clock */
#define EN_OV_INT_TIMER1	TZ1.TIER.BIT.OVIE=1
#define DIS_OV_INT_TIMER1	TZ1.TIER.BIT.OVIE=0
#define IS_OV_INT_TIMER1_EN	(TZ1.TIER.BIT.OVIE ==1)
#define IS_OV_INT_TIMER1_FL	(TZ1.TSR.BIT.OVF ==1)

//         /* HALL registro C timer 1 input */
	#define MATCH_HALL		TZ1.GRC
	#define TIMER_HALL		TZ1.TCNT
	#define EN_HALL_INT		TZ1.TIER.BIT.IMIEC=1
	#define DIS_HALL_INT		TZ1.TIER.BIT.IMIEC=0
	#define RESET_HALL_I		TZ1.TSR.BIT.IMFC=0
	#define NOFRONT_HALL		TZ1.TIORC.BIT.IOC = 0
	#define RISE_HALL		TZ1.TIORC.BIT.IOC = 4
	#define FALL_HALL		TZ1.TIORC.BIT.IOC = 5
	#define IS_HALL_INT_EN		(TZ1.TIER.BIT.IMIEC ==1)
	#define IS_HALL_INT_FL		(TZ1.TSR.BIT.IMFC ==1)

#define START_ALL_TIMER	TZ.TSTR.BYTE  |= 0x3;		/* Start ALL TIMER   */


const struct sensors_sensor halleffect_sensor;
static struct timer debouncetimer;
static int status(int type);
static unsigned int counter;

/*---------------------------------------------------------------------------*/
void _TZ_ITMZ1_ (void)
{
  RESET_HALL_I;
  if(timer_expired(&debouncetimer)) {
    timer_set(&debouncetimer, CLOCK_SECOND / 4);
    sensors_changed(&halleffect_sensor);
    counter++;
  }
}
/*---------------------------------------------------------------------------*/
static int
value(int type)
{
  return counter;
}
/*---------------------------------------------------------------------------*/
static int
configure(int type, int c)
{
  switch (type) {
  case SENSORS_ACTIVE:
    if (c) {
      if(!status(SENSORS_ACTIVE)) {
	timer_set(&debouncetimer, 0);
	counter = 0;
	TZ.TOER.BIT.EC1 = 1; // abilito il pin come timer
	FALL_HALL;
	RESET_HALL_I;
	EN_HALL_INT;
	SET_TCR_TIMER1;
	START_ALL_TIMER;
      }
    } else {
      DIS_HALL_INT;
    }
    return 1;
  case SENSORS_HW_INIT:
    timer_set(&debouncetimer, 0);
    counter = 0;
    TZ.TOER.BIT.EC1 = 1; // abilito il pin come timer
    FALL_HALL;
    RESET_HALL_I;
    EN_HALL_INT;
    SET_TCR_TIMER1;
    START_ALL_TIMER;
    printf(HALLEFFECT_SENSOR" Activated.\r\n");
    return 1;
  default: return 0;
  }
  return 0;
}
/*---------------------------------------------------------------------------*/
int
status(int type)
{
  switch (type) {
  case SENSORS_ACTIVE:
  case SENSORS_READY:
    return IS_HALL_INT_EN;
  }
  return 0;
}
/*---------------------------------------------------------------------------*/
SENSORS_SENSOR(halleffect_sensor, HALLEFFECT_SENSOR,
	       value, configure, status);
