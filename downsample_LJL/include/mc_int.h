/* -*-mode:c++-*- ***********************************************************
 *                                                                          *
 * Mentor Graphics Corp C++ Data Type Packages                                   *
 *                                                                          *
 * Release Version : 23.0                                                   *
 * Release Date    : Fri Mar  8 21:05:48 PST 2019                           *
 * Release Type    : Production Release                                     *
 *                                                                          *
 * Copyright © 2003-2011, Mentor Graphics Corp.                  *
 *                                                                          *
 * All Rights Reserved.                                                     *
 *                                                                          *
 ****************************************************************************
 *                                                                          *
 *   IMPORTANT - USE OF THIS SOFTWARE IS SUBJECT TO LICENSE RESTRICTIONS    *
 *                                                                          *
 *   CAREFULLY READ THIS LICENSE AGREEMENT BEFORE USING THE SOFTWARE        *
 *                                                                          *
 *             YOU MAY USE THIS SOFTWARE AS STATED BELOW,                   *
 *                                                                          *
 *           BUT YOU MAY NOT ALTER OR DISTRIBUTE THE SOFTWARE               *
 *           ------------------------------------------------               *
 *                                                                          *
 ****************************************************************************
 *                                                                          *
 *   This license is a legal Agreement concerning the use of Software       *
 *   between you, the end user, either individually or as an authorized     *
 *   representative of the company acquiring the license, and Mentor        *
 *   Graphics Corp.       YOUR USE OF THE SOFTWARE INDICATES                *
 *   YOUR COMPLETE AND UNCONDITIONAL ACCEPTANCE OF THE TERMS AND CONDITIONS *
 *   SET FORTH IN THIS AGREEMENT. IF YOU  DON'T AGREE TO THE FOLLOWING      *
 *   TERMS AND CONDITIONS, DON'T USE THE SOFTWARE, REMOVE IT FROM YOUR      *
 *   SYSTEM, AND DESTROY ALL COPIES.                                        *
 *                                                                          *
 ****************************************************************************
 *                                                                          *
 *     END-USER LICENSE AGREEMENT                                           *
 *                                                                          *
 *     GRANT OF LICENSE: YOU MAY USE THE SOFTWARE, BUT YOU MAY NOT          *
 *     ALTER OR DISTRIBUTE THE SOFTWARE. The software you are installing,   *
 *     downloading, or otherwise acquired, with this Agreement,             *
 *     including source code, binaries code, updates, modifications,        *
 *     revisions, copies, documentation and design data (collectively       *
 *     the "Software") is copyrighted and owned by Mentor Graphics.         *
 *     Mentor Graphics grants to you, a nontransferable, nonexclusive,      *
 *     limited copyright license to use the Software, but you may not       *
 *     alter or distribute the Software. Use of the Software consists       *
 *     solely of reproduction, performance, and display. Altering           *
 *     the Software (which is prohibited) comprises modifying,              *
 *     changing, editing, adding to, or deleting from, the Software. If     *
 *     the Software is source code, you may compile the Software. You       *
 *     may append the Software to other code, so long as the Software       *
 *     is not altered. Mentor Graphics retains all rights not expressly     *
 *     granted by this Agreement.                                           *
 *                                                                          *
 *     RESTRICTIONS ON USE: YOU MAY USE THE SOFTWARE, BUT YOU MAY NOT       *
 *     ALTER OR DISTRIBUTE THE SOFTWARE. Each copy of the Software you      *
 *     create must include all notices and legends embedded in Software,    *
 *     including this License Agreement. The terms of this Agreement,       *
 *     including without limitation, the licensing and assignment           *
 *     provisions, shall be binding upon your heirs, successors in          *
 *     interest and assigns. The provisions of this section shall           *
 *     survive the termination or expiration of this Agreement.             *
 *                                                                          *
 *     NO WARRANTY: Mentor Graphics expressly disclaims all warranty        *
 *     for the Software. TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE      *
 *     LAW, THE SOFTWARE AND ANY RELATED DOCUMENTATION IS PROVIDED "AS      *
 *     IS" AND WITH ALL FAULTS, AND WITHOUT WARRANTIES OR CONDITIONS OF     *
 *     ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING, WITHOUT              *
 *     LIMITATION, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS       *
 *     FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT. THE ENTIRE RISK      *
 *     OF USING THE SOFTWARE REMAINS WITH YOU.                              *
 *                                                                          *
 *     LIMITATION OF LIABILITY: EXCEPT WHERE THIS EXCLUSION OR              *
 *     RESTRICTION OF LIABILITY WOULD BE VOID OR INEFFECTIVE UNDER          *
 *     APPLICABLE LAW, IN NO EVENT WILL MENTOR GRAPHICS CORP.  OR ITS       *
 *     LICENSORS BE LIABLE FOR INDIRECT, SPECIAL, INCIDENTAL, OR            *
 *     CONSEQUENTIAL DAMAGES (INCLUDING LOST PROFITS OR SAVINGS)            *
 *     WHETHER BASED ON CONTRACT, TORT OR ANY OTHER LEGAL THEORY, EVEN      *
 *     IF MENTOR GRAPHICS CORP.  OR ITS LICENSORS HAVE BEEN ADVISED OF THE  *
 *     POSSIBILITY OF SUCH DAMAGES. IN NO EVENT SHALL MENTOR GRAPHICS CORP  *
 *     OR ITS LICENSORS' LIABILITY UNDER THIS AGREEMENT EXCEED THE          *
 *     AMOUNT PAID BY YOU FOR THE SOFTWARE OR SERVICE GIVING RISE TO        *
 *     THE CLAIM. IN THE CASE WHERE NO AMOUNT WAS PAID, MENTOR GRAPHICS CORP*
 *     AND ITS LICENSORS SHALL HAVE NO LIABILITY FOR ANY DAMAGES            *
 *     WHATSOEVER.                                                          *
 *                                                                          *
 *     LIFE ENDANGERING ACTIVITIES: NEITHER MENTOR GRAPHICS CORP. NOR ITS  *
 *     LICENSORS SHALL BE LIABLE FOR ANY DAMAGES RESULTING FROM, OR IN      *
 *     CONNECTION WITH, THE USE OF SOFTWARE IN ANY APPLICATION WHERE        *
 *     THE FAILURE OR INACCURACY OF THE SOFTWARE MIGHT RESULT IN DEATH      *
 *     OR PERSONAL INJURY. YOU AGREE TO INDEMNIFY AND HOLD HARMLESS         *
 *     MENTOR GRAPHICS CORP. FROM ALL CLAIMS, LOSSES, COSTS, DAMAGES,       *
 *     EXPENSES, AND LIABILITY, INCLUDING ATTORNEYS' FEES, ARISING OUT      *
 *     OF OR IN CONNECTION WITH YOUR USE OF SOFTWARE WHERE FAILURE OR       *
 *     INACCURACY OF THE SOFTWARE MAY, OR IS ALLEGED TO, HAVE CAUSED        *
 *     DEATH OR PERSONAL INJURY.                                            *
 *                                                                          *
 *     TERM: This Agreement terminates immediately if you exceed the        *
 *     scope of the license granted or fail to comply with the              *
 *     provisions of this License Agreement.  Upon termination or           *
 *     expiration, you agree to cease all use of the Software and           *
 *     delete all copies of the Software.                                   *
 *                                                                          *
 *                                                                          *
 *     EXPORT: Exportation and diversion of software is subject to          *
 *     regulation by local laws and the United States Government. You       *
 *     agree that you will not export any Software or direct product of     *
 *     the Software without first obtaining all necessary approval from     *
 *     appropriate local and United States government agencies.             *
 *                                                                          *
 *     CONTROLLING LAW AND JURISDICTION: THIS AGREEMENT SHALL BE            *
 *     GOVERNED BY AND CONSTRUED UNDER THE LAWS OF THE STATE OF OREGON,     *
 *     U.S.A., IF YOU ARE LOCATED IN NORTH OR SOUTH AMERICA, AND THE        *
 *     LAWS OF IRELAND IF YOU ARE LOCATED OUTSIDE OF NORTH AND SOUTH        *
 *     AMERICA. All disputes arising out of or in relation to this          *
 *     Agreement shall be submitted to the exclusive jurisdiction of        *
 *     Dublin, Ireland when the laws of Ireland apply, or Multnomah         *
 *     County, Oregon when the laws of Oregon apply. This section shall     *
 *     not restrict Mentor Graphics' right to bring an action against       *
 *     you in the jurisdiction where your place of business is located.     *
 *     The United Nations Convention on Contracts for the International     *
 *     Sale of Goods does not apply to this Agreement.                      *
 *                                                                          *
 *     MISCELLANEOUS:  If any provision of this Agreement is held by a      *
 *     court of competent jurisdiction to be void, invalid,                 *
 *     unenforceable or illegal, such provision shall be severed from       *
 *     this Agreement and the remaining provisions will remain in full      *
 *     force and effect. This Agreement contains the parties' entire        *
 *     understanding relating to its subject matter and supersedes all      *
 *     prior or contemporaneous agreements, including but not limited       *
 *     to any purchase order terms and conditions, except valid license     *
 *     agreements related to the subject matter of this Agreement           *
 *     (which are physically signed by you and an authorized agent of       *
 *     Mentor Graphics) either referenced in the purchase order or          *
 *     otherwise governing this subject matter. This Agreement may only     *
 *     be modified in writing by authorized representatives of the          *
 *     parties. Waiver of terms or excuse of breach must be in writing      *
 *     and shall not constitute subsequent consent, waiver or excuse.       *
 *                                                                          *
 ***************************************************************************/


#ifndef __MC_INT_H
#define __MC_INT_H

#ifndef __MC_TYPES_DONT_WARN
#if defined(__EDG__) || defined(__GNUC__)
#warning Deprecated include mc_int.h, replace with ac_int.h 
#endif
#endif

#ifdef MC_IGNORE_BUILTINS
#define AC_IGNORE_BUILTINS 1 
#endif

#include <ac_int.h>

#define MC_ASSERT AC_ASSERT
#define mc_assert ac_assert

#define mc_int ac_int
#define mc_bitref ac_bitref

#define mc_base_mode ac_base_mode
#define MC_BIN AC_BIN
#define MC_DEC AC_DEC
#define MC_HEX AC_HEX
#define MC_OCT AC_OCT

#define mc_special_val ac_special_val
#define MC_VAL_DC AC_VAL_DC
#define MC_VAL_0 AC_VAL_0

#define mc_fixed ac_fixed
#define to_mc_int to_ac_int

typedef ac_q_mode mc_q_mode;
typedef ac_o_mode mc_o_mode;
#define MC_TRN AC_TRN
#define MC_RND AC_RND
#define MC_TRN_ZERO AC_TRN_ZERO
#define MC_RND_ZERO AC_RND_ZERO
#define MC_RND_INF AC_RND_INF
#define MC_RND_MIN_INF AC_RND_MIN_INF
#define MC_RND_CONV AC_RND_CONV
#define MC_WRAP AC_WRAP
#define MC_SAT AC_SAT
#define MC_SAT_ZERO AC_SAT_ZERO
#define MC_SAT_SYM AC_SAT_SYM

#endif
