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
 *	Description: 	RLM external definitions
 *
 *	M. Christiano
 *	11/19/05
 *
 */

#ifndef RLM_LICENSE_H_INCLUDED
#define RLM_LICENSE_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif


#define RLM_MAX_LINE	  1024	/* Maximum size of a LF line (after */
						/* all continuations */
#define RLM_ERRSTRING_MAX 512	/* Maximum length of a returned err string */
/*
 *	Function prototypes
 */
extern int 	rlmez_checkout(char *product, char *ver, int *days);
extern int	rlmez_demo(char *product, char *ver, int days);
extern int	rlmez_demox(char *product, char *ver);
extern char *	rlmez_errstring(int err, char *string_space);
extern char *	rlmez_license();

/*
 *	General Errors
 */
#define RLM_HANDLE_ERR_FIRST	 101
#define RLM_EH_NOHANDLE		-101	/* No handle supplied to call */
#define RLM_EH_READ_NOLICENSE	-102	/* Can't read license data */
#define RLM_EH_NET_INIT		-103	/* Network (msg_init()) error */
#define RLM_EH_NET_WERR		-104	/* Error writing to network */
#define RLM_EH_NET_RERR		-105	/* Error reading from network */
#define RLM_EH_NET_BADRESP 	-106	/* Unexpected response */
#define RLM_EH_BADHELLO		-107	/* HELLO message for wrong server */

#define RLM_EH_BADPRIVKEY	-108	/* Error in private key */
#define RLM_EH_SIGERROR		-109	/* Error signing authorization */
#define RLM_EH_INTERNAL		-110	/* Internal error */
	
#define RLM_EH_CONN_REFUSED 	-111	/* Connection refused at server */
#define RLM_EH_NOSERVER		-112	/* No server to connect to */
#define RLM_EH_BADHANDSHAKE 	-113	/* Bad communications handshake */
#define RLM_EH_CANTGETETHER 	-114	/* Can't get ethernet address */
					/* system call failures */
#define RLM_EH_MALLOC		-115
#define RLM_EH_BIND		-116	/* bind() error */
#define RLM_EH_SOCKET		-117	/* socket() error */

#define RLM_EH_BADPUBKEY	-118	/* Error in public key */
#define RLM_EH_AUTHFAIL		-119	/* Authentication failed */
#define RLM_EH_WRITE_LF		-120	/* Can't write temp license file */
#define RLM_EH_NO_REPORTLOG 	-121	/* No reportlog file on this server */
#define RLM_EH_DUP_ISV_HID	-122	/* ISV-defined hostid already registered */
#define RLM_EH_BADPARAM		-123	/* Bad parameter passed to RLM function */
/*
 *	BADPARAM sub-errors (in syserr)
 */
#define RLM_ACT_BP_NOLIC	-1
#define RLM_ACT_BP_NOURL	-2
#define RLM_ACT_BP_NLKEY	-3	/* Newline in activation key */
#define RLM_ACT_BP_NLXTRA	-4	/* Newline in extra parameter */
#define RLM_ACT_BP_TOOMUCH	-5	/* Too much data to send */
#define RLM_ACT_BP_ILLXTRA	-6	/* Illegal character in extra data */
#define RLM_CO_BADCOUNT		-7	/* bad count for checkout <= 0 */
#define RLM_DD_BADDATE		-8	/* exp date <= 0 for detached demo */
#define RLM_ILL_CHAR_HOSTID	-9	/* Illegal char in hostid */

#define RLM_EH_ROAMWRITEERR	-124	/* Roam File write error */
#define RLM_EH_ROAMREADERR	-125	/* Roam File read error */
#define RLM_EH_HANDLER_INSTALLED -126	/* heartbeat handler already installed */
#define RLM_EH_CANTCREATLOCK -127	/* Cannot create 'single' lockfile */
#define RLM_EH_CANTOPENLOCK -128	/* Cannot open 'single' lockfile */
#define RLM_EH_CANTSETLOCK -129		/* Cannot set lock on 'single' */
#define RLM_EH_BADRLMLIC -130		/* Bad/missing/expired RLM license */
#define RLM_EH_BADHOST	-131		/* Bad hostname in LF/port@host */
#define RLM_EH_CANTCONNECT_URL -132	/* Cannot connect to specified URL */
#define RLM_EH_OP_NOT_ALLOWED  -133	/* Operation not allowed on server */
#define RLM_EH_ACT_BADSTAT     -134	/* Bad status from activation binary */
#define RLM_EH_ACT_BADLICKEY   -135	/* Bad license key in 
							activation binary */
#define RLM_EH_BAD_HTTP	       -136	/* Bad HTTP transaction */
#define RLM_EH_DEMOEXISTS      -137	/* Demo already created */
#define RLM_EH_DEMOWRITEERR    -138	/* Demo file write error */
#define RLM_EH_NO_RLM_PLATFORM -140	/* RLM unlicensed on this platform */
#define RLM_EH_EVAL_EXPIRED    -141	/* RLM eval expired */
#define RLM_EH_SERVER_REJECT   -142	/* Server rejected by client */
#define RLM_EH_UNLICENSED      -143	/* Unlicensed RLM option */
#define RLM_EH_SEMAPHORE_FAILURE -144	/* Semaphore initialization failure */
#define RLM_EH_ACT_OLDSERVER	-145	/* Activation server too old */
#define RLM_EH_BAD_LIC_LINE	-146	/* Invalid license line in LF */
#define RLM_EH_BAD_SERVER_HOSTID -147	/* Invalid hostid on SERVER line */

#define RLM_EH_MAX_ERRS 47

/*
 *	rlm_checkout() errors - returned by rlm_license_stat(license)
 */
#define RLM_EL_NOPRODUCT 	-1	/* No authorization for product */
#define RLM_EL_NOTME		-2	/* Authorization is for another ISV */
#define RLM_EL_EXPIRED		-3	/* Authorization has expired */
#define RLM_EL_NOTTHISHOST 	-4	/* Wrong host for authorization */
#define RLM_EL_BADKEY		-5	/* Bad key in authorization */
#define RLM_EL_BADVER		-6	/* Requested version not supported */
#define RLM_EL_BADDATE		-7	/* bad date format - not permanent or 
								dd-mm-yy */
#define RLM_EL_TOOMANY		-8	/* checkout request for too many 
								licenses */
#define RLM_EL_NOAUTH		-9	/* No license auth supplied to call */
#define RLM_EL_ON_EXC_ALL 	-10	/* On excludeall list */
#define RLM_EL_ON_EXC		-11	/* On feature exclude list */
#define RLM_EL_NOT_INC_ALL 	-12	/* Not on the includeall list */
#define RLM_EL_NOT_INC		-13	/* Not on the feature include list */
#define RLM_EL_OVER_MAX		-14	/* Request would go over license MAX */
#define RLM_EL_REMOVED		-15	/* License (rlm)removed by server */
#define RLM_EL_SERVER_BADRESP 	-16	/* Unexpected response from server */
#define RLM_EL_COMM_ERROR	-17	/* Error communicating with server */
#define RLM_EL_NO_SERV_SUPP	-18	/* License server doesn't support this */
#define RLM_EL_NOHANDLE   	-19	/* No license handle */
#define RLM_EL_SERVER_DOWN  	-20	/* Server closed connection */
#define RLM_EL_NO_HEARTBEAT 	-21	/* No heartbeat response received */
#define RLM_EL_ALLINUSE		-22	/* All licenses in use */
#define RLM_EL_NOHOSTID		-23	/* No hostid on uncounted license */
#define RLM_EL_TIMEDOUT		-24	/* License timed out by server */
#define RLM_EL_INQUEUE		-25	/* In queue for license */
#define RLM_EL_SYNTAX		-26	/* License syntax error */
#define RLM_EL_ROAM_TOOLONG	-27	/* Roam time exceeds maximum */
#define RLM_EL_NO_SERV_HANDLE	-28	/* Server does not know this handle */
#define RLM_EL_ON_EXC_ROAM 	-29	/* On roam exclude list */
#define RLM_EL_NOT_INC_ROAM	-30	/* Not on roam include list */
#define RLM_EL_TOOMANY_ROAMING	-31	/* Too many licenses roaming */
#define RLM_EL_WILL_EXPIRE	-32	/* License expires before roam ends */
#define RLM_EL_ROAMFILEERR	-33	/* Problem with roam file */
#define RLM_EL_RLM_ROAM_ERR	-34	/* Cannot checkout rlm_roam license */
#define RLM_EL_WRONG_PLATFORM	-35	/* Wrong platform for client */
#define RLM_EL_WRONG_TZ		-36	/* Wrong timezone for client */
#define RLM_EL_NOT_STARTED	-37	/* License start date not reached */
#define RLM_EL_CANT_GET_DATE	-38	/* time() failure */
#define RLM_EL_OVERSOFT		-39	/* Over license soft limit */
#define RLM_EL_WINDBACK		-40	/* Clock setback detected */
#define RLM_EL_BADPARAM		-41	/* Bad parameter to checkout() call */
#define RLM_EL_NOROAM_FAILOVER	-42	/* Roam operations not permitted on
					   failover servers */
#define RLM_EL_BADHOST		-43	/* Bad hostname in LF/port@host */
#define RLM_EL_APP_INACTIVE	-44	/* Application is inactive */
#define RLM_EL_NOT_NAMED_USER	-45	/* Not a named user */
#define RLM_EL_TS_DISABLED	-46	/* Terminal Server disabled */
#define RLM_EL_VM_DISABLED	-47	/* Virtual Machines disabled */
#define RLM_EL_PORTABLE_REMOVED	-48	/* Portable hostid removed */
#define RLM_EL_DEMOEXP		-49	/* Demo license has expired */
#define RLM_EL_FAILED_BACK_UP	-50	/* Failed host back up */
#define RLM_EL_SERVER_LOST_XFER	-51	/* Server lost it's transferred license */
#define RLM_EL_BAD_PASSWORD	-52	/* Incorrect password for product */
					/* Note: RLM_EL_BAD_PASSWORD is an
					   internal error and won't ever be 
					   returned to the client - if the 
					   license password is bad, the client
					   will receive RLM_EL_NO_SERV_SUPP */

#define RLM_EL_MAX_ERRS 52

/*
 *	Activation errors
 */

#define RLM_ACT_ERR_FIRST	 1001
#define RLM_ACT_BADPARAM	-1001	/* Unused - RLM_EH_BADPARAM instead */
#define RLM_ACT_NO_KEY		-1002	
#define RLM_ACT_NO_PROD		-1003	/* No product definition */
#define RLM_ACT_CANT_WRITE_KEYS -1004	/* Can't write keyf table */
#define RLM_ACT_KEY_USED 	-1005	/* Activation key already used*/
#define RLM_ACT_BAD_HOSTID	-1006	/* Missing hostid */
#define RLM_ACT_BAD_HOSTID_TYPE	-1007	/* Invalid hostid type */
#define RLM_ACT_BAD_HTTP	-1008	/* Bad HTTP transaction - 
							UNUSED after v3.0BL4 */
#define RLM_ACT_CANTLOCK	-1009	/* Can't lock activation DB */
#define RLM_ACT_CANTREAD_DB	-1010	/* Can't read activation DB */
#define RLM_ACT_CANT_WRITE_FULFILL -1011 /* Can't write licf table */
#define RLM_ACT_CLIENT_TIME_BAD -1012	/* Clock incorrect on client */
#define RLM_ACT_BAD_REDIRECT	-1013	/* Bad redirect from server */
#define RLM_ACT_TOOMANY_HOSTID_CHANGES	-1014	/* Too many hostid changes */
					/*  for Refresh activation */
#define RLM_ACT_BLACKLISTED	-1015	/* Domain on blacklist */
#define RLM_ACT_NOT_WHITELISTED	-1016	/* Domain not on whitelist */
#define RLM_ACT_KEY_EXPIRED	-1017	/* Activation key expired */
#define RLM_ACT_NO_PERMISSION	-1018	/* HTTP request denied */
#define RLM_ACT_SERVER_ERROR	-1019	/* HTTP internal server error */
#define RLM_ACT_BAD_GENERATOR	-1020	/* Bad/missing generator file */
#define RLM_ACT_NO_KEY_MATCH	-1021	/* No matching key in DB */
#define RLM_ACT_NO_AUTH_SUPPLIED -1022	/* No proxy auth supplied */
#define RLM_ACT_PROXY_AUTH_FAILED -1023	/* proxy auth failed */
#define RLM_ACT_NO_BASIC_AUTH	-1024	/* No basic authentication */
#define RLM_ACT_GEN_UNLICENSED   -1025	/* Activation generator unlicensed */
#define RLM_ACT_DB_READERR	-1026	/* Activtion DB read error (MySQL) */
#define RLM_ACT_GEN_PARAM_ERR	-1027	/* Generating license - bad parameter */
#define RLM_ACT_ERR_LAST	27

#ifdef __cplusplus
}
#endif

#endif /* RLM_LICENSE_H_INCLUDED */
