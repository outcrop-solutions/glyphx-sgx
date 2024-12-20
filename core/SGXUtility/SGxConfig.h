///
/// SynGlyphX Holdings Incorporated ("COMPANY") CONFIDENTIAL
/// Copyright (c) 2013-2015 SynGlyphX Holdings Incorporated, All Rights Reserved.
///
/// NOTICE:  All information contained herein is, and remains the property of COMPANY. The intellectual and technical concepts contained
/// herein are proprietary to COMPANY and may be covered by U.S. and Foreign Patents, patents in process, and are protected by trade secret or copyright law.
/// Dissemination of this information or reproduction of this material is strictly forbidden unless prior written permission is obtained
/// from COMPANY.  Access to the source code contained herein is hereby forbidden to anyone except current COMPANY employees, managers or contractors who have executed 
/// Confidentiality and Non-disclosure agreements explicitly covering such access.
///
/// The copyright notice above does not evidence any actual or intended publication or disclosure  of  this source code, which includes  
/// information that is confidential and/or proprietary, and is a trade secret, of  COMPANY.   ANY REPRODUCTION, MODIFICATION, DISTRIBUTION, PUBLIC  PERFORMANCE, 
/// OR PUBLIC DISPLAY OF OR THROUGH USE  OF THIS  SOURCE CODE  WITHOUT  THE EXPRESS WRITTEN CONSENT OF COMPANY IS STRICTLY PROHIBITED, AND IN VIOLATION OF APPLICABLE 
/// LAWS AND INTERNATIONAL TREATIES.  THE RECEIPT OR POSSESSION OF  THIS SOURCE CODE AND/OR RELATED INFORMATION DOES NOT CONVEY OR IMPLY ANY RIGHTS  
/// TO REPRODUCE, DISCLOSE OR DISTRIBUTE ITS CONTENTS, OR TO MANUFACTURE, USE, OR SELL ANYTHING THAT IT  MAY DESCRIBE, IN WHOLE OR IN PART.                
///

#pragma once
//global warning disable
#if defined(_MSC_VER)
#pragma warning(disable: 4251)
#pragma warning(disable: 4996) 
#define _SCL_SECURE_NO_WARNINGS 
//4996 - "Function call with parameters that may be unsafe" used in Boost
//4251 - "class needs to have dll - interface.." we will need to deal with this eventually
#endif

#if defined(_MSC_VER)
#define SGX_PRAGMA_WARNING_PUSH __pragma(warning(push))
#define SGX_PRAGMA_WARNING_POP	__pragma(warning(pop))
#define SGX_PRAGMA_WARNING_DISABLE(x) __pragma(warning(disable:x))
#else
#define SGX_PRAGMA_WARNING_PUSH 
#define SGX_PRAGMA_WARNING_POP	
#define SGX_PRAGMA_WARNING_DISABLE(x) 
#endif