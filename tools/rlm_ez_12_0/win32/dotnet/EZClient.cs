using System;
using System.Runtime.InteropServices;

namespace Reprise
{
    class RLMEZclient
    {
	[DllImport("rlmez1203.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
	public static extern int rlmez_checkout(String product, String version, 
		ref int days);

	[DllImport("rlmez1203.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
	public static extern int rlmez_demo(String product, String version, int days);

	[DllImport("rlmez1203.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
	public static extern int rlmez_demox(String product, String version);

	[DllImport("rlmez1203.dll", CharSet = CharSet.Ansi, CallingConvention = CallingConvention.Cdecl)]
	public static extern IntPtr rlmez_errstring(int err, byte[] sarray);

	private static String marshalToString(IntPtr ip)
	{
		return Marshal.PtrToStringAnsi(ip);
	}
	
	private static string version = "1.0";

	private static void perror(String msg, int code) {
		byte[] errbytes = new byte[1000];
		IntPtr errmsg = rlmez_errstring(code, errbytes);
		String errstring = marshalToString(errmsg);
		Console.WriteLine(msg + ": " + errstring + " (" + code + ")");
	}

	private static bool yesNo() {
		bool yes = false;
		string resp = Console.ReadLine();
		if (resp[0] == 'y')
			yes = true;
		return yes;
	}

	private static void doDemo(String product) {
		Console.WriteLine("A demo license already exists.");
		Console.WriteLine("You would not do this in your");
		Console.WriteLine("production code, but for the");
		Console.WriteLine("purposes of demoing RLM-EZ, do you");
		Console.WriteLine("want to remove that demo and");
		Console.WriteLine("reinstall it? ");
		bool yes = yesNo();
		if (yes) {
			int stat = rlmez_demox(product, version);
			if (stat == 0) {
			    Console.WriteLine("Existing demo deleted");
			    stat = rlmez_demo(product, version, 10);
			    if (stat == 0)
				Console.WriteLine("demo license reinstalled");
			    else
				perror("rlmez_demo failed", stat);
			}
			else
				perror("rlmez_demox failed", stat);
		}
	}

	static void Main(string[] argv) {
		int stat, days = 0;
		bool yes;
		String product;
		String defproduct = "test1";
		if (argv.Length > 0)
			product = argv[0];
		else
			product = defproduct;
		stat = rlmez_checkout(product, version, ref days);
		if (days >= 0) {
			Console.WriteLine("Checkout OK");
			if (days == 0)
				Console.WriteLine("Permanent license");
			else
				Console.WriteLine(days-1 + " days to expiration");
		}
		else {
			if (stat == RLM_EH_EVAL_EXPIRED)
			{
				Console.WriteLine(
					"Your RLM-ez evaluation has expired.");
			}
			else {
				perror("checkout error", stat);
				Console.WriteLine(
					"Do you want to install a 10-day demo? ");
				yes = yesNo();
				if (yes) {
					stat = rlmez_demo(product, version, 10);
					if (stat == RLM_EH_DEMOEXISTS) {
						doDemo(product);
					}
				}
			}
		}
	}


public const int RLM_EH_NOHANDLE         = -101;    /* No handle supplied to call */
public const int RLM_EH_READ_NOLICENSE   = -102;    /* Can't read license data */
public const int RLM_EH_NET_INIT         = -103;    /* Network (msg_init()) error */
public const int RLM_EH_NET_WERR         = -104;    /* Error writing to network */
public const int RLM_EH_NET_RERR         = -105;    /* Error reading from network */
public const int RLM_EH_NET_BADRESP      = -106;    /* Unexpected response */
public const int RLM_EH_BADHELLO         = -107;    /* HELLO message for wrong server */

public const int RLM_EH_BADPRIVKEY       = -108;    /* Error in private key */
public const int RLM_EH_SIGERROR         = -109;    /* Error signing authorization */
public const int RLM_EH_INTERNAL         = -110;    /* Internal error */

public const int RLM_EH_CONN_REFUSED     = -111;    /* Connection refused at server */
public const int RLM_EH_NOSERVER         = -112;    /* No server to connect to */
public const int RLM_EH_BADHANDSHAKE     = -113;    /* Bad communications handshake */
public const int RLM_EH_CANTGETETHER     = -114;    /* Can't get ethernet address */
                                        /* system call failures */
public const int RLM_EH_MALLOC           = -115;
public const int RLM_EH_BIND             = -116;    /* bind() error */
public const int RLM_EH_SOCKET           = -117;    /* socket() error */

public const int RLM_EH_BADPUBKEY        = -118;    /* Error in public key */
public const int RLM_EH_AUTHFAIL         = -119;    /* Authentication failed */
public const int RLM_EH_WRITE_LF         = -120;    /* Can't write temp license file */
public const int RLM_EH_NO_REPORTLOG     = -121;    /* No reportlog file on this server */
public const int RLM_EH_DUP_ISV_HID      = -122;    /* ISV-defined hostid already registered */
public const int RLM_EH_BADPARAM         = -123;    /* Bad parameter passed to RLM function */
/*
 *      BADPARAM sub-errors (in syserr)
 */
public const int RLM_ACT_BP_NOLIC        = -1;
public const int RLM_ACT_BP_NOURL        = -2;
public const int RLM_ACT_BP_NLKEY        = -3;      /* Newline in activation key */
public const int RLM_ACT_BP_NLXTRA       = -4;      /* Newline in extra parameter */
public const int RLM_ACT_BP_TOOMUCH      = -5;      /* Too much data to send */
public const int RLM_ACT_BP_ILLXTRA      = -6;      /* Illegal character in extra data */
public const int RLM_CO_BADCOUNT         = -7;      /* bad count for checkout <= 0 */
public const int RLM_DD_BADDATE          = -8;      /* exp date <= 0 for detached demo */
public const int RLM_ILL_CHAR_HOSTID     = -9;      /* Illegal char in hostid */

public const int RLM_EH_ROAMWRITEERR     = -124;    /* Roam File write error */
public const int RLM_EH_ROAMREADERR      = -125;    /* Roam File read error */
public const int RLM_EH_HANDLER_INSTALLED = -126;   /* heartbeat handler already installed */
public const int RLM_EH_CANTCREATLOCK = -127;       /* Cannot create 'single' lockfile */
public const int RLM_EH_CANTOPENLOCK = -128;        /* Cannot open 'single' lockfile */
public const int RLM_EH_CANTSETLOCK = -129;         /* Cannot set lock on 'single' */
public const int RLM_EH_BADRLMLIC = -130;           /* Bad/missing/expired RLM license */
public const int RLM_EH_BADHOST  = -131;            /* Bad hostname in LF/port@host */
public const int RLM_EH_CANTCONNECT_URL = -132;     /* Cannot connect to specified URL */
public const int RLM_EH_OP_NOT_ALLOWED  = -133;     /* Operation not allowed on server */
public const int RLM_EH_ACT_BADSTAT     = -134;     /* Bad status from activation binary */
public const int RLM_EH_ACT_BADLICKEY   = -135;     /* Bad license key in 
                                                        activation binary */
public const int RLM_EH_BAD_HTTP        = -136;     /* Bad HTTP transaction */
public const int RLM_EH_DEMOEXISTS      = -137;     /* Demo already created */
public const int RLM_EH_DEMOWRITEERR    = -138;     /* Demo file write error */
public const int RLM_EH_NO_RLM_PLATFORM = -140;     /* RLM unlicensed on this platform */
public const int RLM_EH_EVAL_EXPIRED    = -141;     /* RLM eval expired */
public const int RLM_EH_SERVER_REJECT   = -142;     /* Server rejected by client */
public const int RLM_EH_UNLICENSED      = -143;     /* Unlicensed RLM option */
public const int RLM_EH_SEMAPHORE_FAILURE = -144;   /* Semaphore initialization failure */
public const int RLM_EH_ACT_OLDSERVER    = -145;    /* Activation server too old */
public const int RLM_EH_BAD_LIC_LINE     = -146;    /* Invalid license line in LF */
public const int RLM_EH_BAD_SERVER_HOSTID = -147;   /* Invalid hostid on SERVER line */


/*
 *      rlm_checkout() errors - returned by rlm_license_stat(license)
 */
public const int RLM_EL_NOPRODUCT        = -1;      /* No authorization for product */
public const int RLM_EL_NOTME            = -2;      /* Authorization is for another ISV */
public const int RLM_EL_EXPIRED          = -3;      /* Authorization has expired */
public const int RLM_EL_NOTTHISHOST      = -4;      /* Wrong host for authorization */
public const int RLM_EL_BADKEY           = -5;      /* Bad key in authorization */
public const int RLM_EL_BADVER           = -6;      /* Requested version not supported */
public const int RLM_EL_BADDATE          = -7;      /* bad date format - not permanent or 
                                                                dd-mm-yy */
public const int RLM_EL_TOOMANY          = -8;      /* checkout request for too many 
                                                                licenses */
public const int RLM_EL_NOAUTH           = -9;      /* No license auth supplied to call */
public const int RLM_EL_ON_EXC_ALL       = -10;     /* On excludeall list */
public const int RLM_EL_ON_EXC           = -11;     /* On feature exclude list */
public const int RLM_EL_NOT_INC_ALL      = -12;     /* Not on the includeall list */
public const int RLM_EL_NOT_INC          = -13;     /* Not on the feature include list */
public const int RLM_EL_OVER_MAX         = -14;     /* Request would go over license MAX */
public const int RLM_EL_REMOVED          = -15;     /* License (rlm)removed by server */
public const int RLM_EL_SERVER_BADRESP   = -16;     /* Unexpected response from server */
public const int RLM_EL_COMM_ERROR       = -17;     /* Error communicating with server */
public const int RLM_EL_NO_SERV_SUPP     = -18;     /* License server doesn't support this */
public const int RLM_EL_NOHANDLE         = -19;     /* No license handle */
public const int RLM_EL_SERVER_DOWN      = -20;     /* Server closed connection */
public const int RLM_EL_NO_HEARTBEAT     = -21;     /* No heartbeat response received */
public const int RLM_EL_ALLINUSE         = -22;     /* All licenses in use */
public const int RLM_EL_NOHOSTID         = -23;     /* No hostid on uncounted license */
public const int RLM_EL_TIMEDOUT         = -24;     /* License timed out by server */
public const int RLM_EL_INQUEUE          = -25;     /* In queue for license */
public const int RLM_EL_SYNTAX           = -26;     /* License syntax error */
public const int RLM_EL_ROAM_TOOLONG     = -27;     /* Roam time exceeds maximum */
public const int RLM_EL_NO_SERV_HANDLE   = -28;     /* Server does not know this handle */
public const int RLM_EL_ON_EXC_ROAM      = -29;     /* On roam exclude list */
public const int RLM_EL_NOT_INC_ROAM     = -30;     /* Not on roam include list */
public const int RLM_EL_TOOMANY_ROAMING  = -31;     /* Too many licenses roaming */
public const int RLM_EL_WILL_EXPIRE      = -32;     /* License expires before roam ends */
public const int RLM_EL_ROAMFILEERR      = -33;     /* Problem with roam file */
public const int RLM_EL_RLM_ROAM_ERR     = -34;     /* Cannot checkout rlm_roam license */
public const int RLM_EL_WRONG_PLATFORM   = -35;     /* Wrong platform for client */
public const int RLM_EL_WRONG_TZ         = -36;     /* Wrong timezone for client */
public const int RLM_EL_NOT_STARTED      = -37;     /* License start date not reached */
public const int RLM_EL_CANT_GET_DATE    = -38;     /* time() failure */
public const int RLM_EL_OVERSOFT         = -39;     /* Over license soft limit */
public const int RLM_EL_WINDBACK         = -40;     /* Clock setback detected */
public const int RLM_EL_BADPARAM         = -41;     /* Bad parameter to checkout() call */
public const int RLM_EL_NOROAM_FAILOVER  = -42;     /* Roam operations not permitted on
                                           failover servers */
public const int RLM_EL_BADHOST          = -43;     /* Bad hostname in LF/port@host */
public const int RLM_EL_APP_INACTIVE     = -44;     /* Application is inactive */
public const int RLM_EL_NOT_NAMED_USER   = -45;     /* Not a named user */
public const int RLM_EL_TS_DISABLED      = -46;     /* Terminal Server disabled */
public const int RLM_EL_VM_DISABLED      = -47;     /* Virtual Machines disabled */
public const int RLM_EL_PORTABLE_REMOVED = -48;     /* Portable hostid removed */
public const int RLM_EL_DEMOEXP          = -49;     /* Demo license has expired */
public const int RLM_EL_FAILED_BACK_UP   = -50;     /* Failed host back up */
public const int RLM_EL_SERVER_LOST_XFER = -51;     /* Server lost it's transferred license */
public const int RLM_EL_BAD_PASSWORD     = -52;     /* Incorrect password for product */
                                        /* Note: RLM_EL_BAD_PASSWORD is an
                                           internal error and won't ever be 
                                           returned to the client - if the 
                                           license password is bad, the client
                                           will receive RLM_EL_NO_SERV_SUPP */

    }
}
