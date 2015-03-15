/******************************************************************************

            COPYRIGHT (c) 2005, 2014 by Reprise Software, Inc.
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
 *      Description:    Test Java client for RLM-EZ
 *
 *      Parameters:     None
 *
 *      Return:         None
 *
 *      Bob Mearns
 *      1/14
 *
 *      $Id: EZClient.java,v 1.1 2014/02/03 21:18:23 matt Exp $
 */

import java.io.*;
import com.reprisesoftware.rlmez.RlmEZ;

public class EZClient {
	private static final String version = "1.0";

	private static void perror(String msg, int code) {
		System.out.println(
			msg + ": " + RlmEZ.errString(code) + " (" + code + ")");
	}

	private static boolean yesNo() {
		boolean yes = false;
		String resp = null;
		BufferedReader br = new 
			BufferedReader(new InputStreamReader(System.in));
		try {
			resp = br.readLine().toLowerCase();
		} catch (IOException e) {}
		if (resp.charAt(0) == 'y')
			yes = true;
		return yes;
	}

	private static void doDemo() {
		System.out.println("A demo license already exists.");
		System.out.println("You would not do this in your");
		System.out.println("production code, but for the");
		System.out.println("purposes of demoing RLM-EZ, do you");
		System.out.println("want to remove that demo and");
		System.out.print("reinstall it? ");
		boolean yes = yesNo();
		if (yes) {
			int stat = RlmEZ.deleteDemo(version);
			if (stat == 0) {
			    System.out.println("Existing demo deleted");
			    stat = RlmEZ.createDemo(version, 10);
			    if (stat == 0)
				System.out.println(
					"demo license reinstalled");
			    else
				perror("createDemo failed", stat);
			}
			else
				perror("deleteDemo failed", stat);
		}
	}

	public static void main(String argv[]) {
		int stat;
		boolean yes;
		int days = RlmEZ.checkout(version);
		stat = days;
		if (days >= 0) {
			System.out.println("Checkout OK");
			if (days == 0)
				System.out.println("Permanent license");
			else
				System.out.println(days-1 + " days to expiration");
		}
		else {
			if (stat == RlmEZ.RLM_EH_EVAL_EXPIRED)
			{
				System.out.println(
					"Your RLM-ez evaluation has expired.");
			}
			else {
				perror("checkout error", stat);
				System.out.print(
					"Do you want to install a 10-day demo? ");
				yes = yesNo();
				if (yes) {
					stat = RlmEZ.createDemo(version, 10);
					if (stat == RlmEZ.RLM_EH_DEMOEXISTS) {
						doDemo();
					}
				}
			}
		}
	}
}
