/******************************************************************************

	    COPYRIGHT (c) 2005, 2016 by Reprise Software, Inc.
	This software has been provided pursuant to a License Agreement
	containing restrictions on its use.  This software contains
	valuable trade secrets and proprietary information of 
	Reprise Software Inc and is protected by law.  It may not be 
	copied or distributed in any form or medium, disclosed to third 
	parties, reverse engineered or used in any manner not provided 
	for in said License Agreement except with the prior written 
	authorization from Reprise Software Inc.

 *****************************************************************************/
/*	
 *	Description: 	Test client for RLM-EZ
 *
 *	Parameters:	None
 *
 *	Return:		None
 *
 *	M. Christiano
 *	11/27/05
 *
 *	$Id: ezclient.c,v 1.12 2016/01/08 20:19:10 matt Exp $
 */

#include "rlmez.h"
#include <stdio.h>

int
main(int argc, char *argv[])
{
  int stat, days;
  char *product = "test1";
  char *ver = "1.0";
  char resp[100];
  char errstring[RLM_ERRSTRING_MAX];

	if (argc > 1) product = argv[1];
	stat = rlmez_checkout(product, ver, &days);
	if (stat)
	{
		printf("checkout of %s failed, status: %d: %s\n", product, stat,
					rlmez_errstring(stat, errstring));
		printf("do you want to install a 10-day-demo? : ");
		fgets(resp, RLM_MAX_LINE, stdin);
		if (*resp == 'y' || *resp == 'Y')
		{
			stat = rlmez_demo(product, ver, 10);
/*
 *			You don't want to do this in your real code,
 *			but if you re-build the kit and create a new
 *			public/private key pair, any old demo licenses
 *			will become unreadable.  So for the sake of the
 *			demo, we remove the demo file if it exists, and
 *			re-create it.
 */
			if ((stat == RLM_EH_DEMOEXISTS))
			{
				stat = rlmez_demox(product, ver);
				if (!stat)
				    stat = rlmez_demo(product, ver, 10);
				if (stat)
				{
					printf("ERROR: demo already exists\n");
					return(stat);
				}
			}
			else
			  printf("status from demo installation: %d: %s\n", 
				    stat, rlmez_errstring(stat, errstring));
		}
	}
	else
	{
		printf("Checkout of %s OK", product);
		if (days == 0)
			printf(", permanent license\n");
		else if (days > 0)
			printf(", days to expiration: %d\n", days-1);
	}
	return(0);
}
