/*********************************************************************
*               SEGGER MICROCONTROLLER GmbH & Co. KG                 *
*       Solutions for real time microcontroller applications         *
**********************************************************************
*                                                                    *
*       (c) 2015 - 2016  SEGGER Microcontroller GmbH & Co. KG        *
*                                                                    *
*       www.segger.com     Support: support@segger.com               *
*                                                                    *
**********************************************************************
*                                                                    *
*       SEGGER SystemView * Real-time application analysis           *
*                                                                    *
**********************************************************************
*                                                                    *
* All rights reserved.                                               *
*                                                                    *
* * This software may in its unmodified form be freely redistributed *
*   in source form.                                                  *
* * The source code may be modified, provided the source code        *
*   retains the above copyright notice, this list of conditions and  *
*   the following disclaimer.                                        *
* * Modified versions of this software in source or linkable form    *
*   may not be distributed without prior consent of SEGGER.          *
*                                                                    *
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS "AS IS" AND     *
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,  *
* THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A        *
* PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL               *
* SEGGER Microcontroller BE LIABLE FOR ANY DIRECT, INDIRECT,         *
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES           *
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS    *
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS            *
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,       *
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING          *
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS *
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.       *
*                                                                    *
**********************************************************************
*                                                                    *
*       SystemView version: V2.38                                    *
*                                                                    *
**********************************************************************
-------------------------- END-OF-HEADER -----------------------------
File    : SEGGER_SYSVIEW.h
Purpose : System visualization API.
Revision: $Rev: 3735 $
*/

#ifndef SEGGER_SYSVIEW_H
#define SEGGER_SYSVIEW_H

/*********************************************************************
*
*       #include Section
*
**********************************************************************
*/

#include "SEGGER.h"

#ifdef __cplusplus
extern "C" {
#endif


/*********************************************************************
*
*       Defines, fixed
*
**********************************************************************
*/

#define SEGGER_SYSVIEW_VERSION        21000

#define SEGGER_SYSVIEW_INFO_SIZE      9   // Minimum size, which has to be reserved for a packet. 1-2 byte of message type, 0-2  byte of payload length, 1-5 bytes of timestamp.
#define SEGGER_SYSVIEW_QUANTA_U32     5   // Maximum number of bytes to encode a U32, should be reserved for each 32-bit value in a packet.

#define SEGGER_SYSVIEW_LOG            (0u)
#define SEGGER_SYSVIEW_WARNING        (1u)
#define SEGGER_SYSVIEW_ERROR          (2u)
#define SEGGER_SYSVIEW_FLAG_APPEND    (1u << 6)

#define SEGGER_SYSVIEW_PREPARE_PACKET(p)  (p) + 4
//
// SystemView events. First 32 IDs from 0 .. 31 are reserved for these
//
#define   SEGGER_SYSVIEW_EVENT_ID_NOP                0  // Dummy packet.
#define   SEGGER_SYSVIEW_EVENT_ID_OVERFLOW           1
#define   SEGGER_SYSVIEW_EVENT_ID_ISR_ENTER          2
#define   SEGGER_SYSVIEW_EVENT_ID_ISR_EXIT           3
#define   SEGGER_SYSVIEW_EVENT_ID_TASK_START_EXEC    4
#define   SEGGER_SYSVIEW_EVENT_ID_TASK_STOP_EXEC     5
#define   SEGGER_SYSVIEW_EVENT_ID_TASK_START_READY   6
#define   SEGGER_SYSVIEW_EVENT_ID_TASK_STOP_READY    7
#define   SEGGER_SYSVIEW_EVENT_ID_TASK_CREATE        8
#define   SEGGER_SYSVIEW_EVENT_ID_TASK_INFO          9
#define   SEGGER_SYSVIEW_EVENT_ID_TRACE_START       10
#define   SEGGER_SYSVIEW_EVENT_ID_TRACE_STOP        11
#define   SEGGER_SYSVIEW_EVENT_ID_SYSTIME_CYCLES    12
#define   SEGGER_SYSVIEW_EVENT_ID_SYSTIME_US        13
#define   SEGGER_SYSVIEW_EVENT_ID_SYSDESC           14
#define   SEGGER_SYSVIEW_EVENT_ID_USER_START        15
#define   SEGGER_SYSVIEW_EVENT_ID_USER_STOP         16
#define   SEGGER_SYSVIEW_EVENT_ID_IDLE              17
#define   SEGGER_SYSVIEW_EVENT_ID_ISR_TO_SCHEDULER  18
#define   SEGGER_SYSVIEW_EVENT_ID_TIMER_ENTER       19
#define   SEGGER_SYSVIEW_EVENT_ID_TIMER_EXIT        20
#define   SEGGER_SYSVIEW_EVENT_ID_STACK_INFO        21
#define   SEGGER_SYSVIEW_EVENT_ID_MODULEDESC        22

#define   SEGGER_SYSVIEW_EVENT_ID_INIT              24
#define   SEGGER_SYSVIEW_EVENT_ID_NAME_RESOURCE     25
#define   SEGGER_SYSVIEW_EVENT_ID_PRINT_FORMATTED   26
#define   SEGGER_SYSVIEW_EVENT_ID_NUMMODULES        27
#define   SEGGER_SYSVIEW_EVENT_ID_END_CALL          28
#define   SEGGER_SYSVIEW_EVENT_ID_TASK_TERMINATE    29

#define   SEGGER_SYSVIEW_EVENT_ID_EX                31
//
// Event masks to disable/enable events
//
#define   SEGGER_SYSVIEW_EVENT_MASK_NOP               (1 << SEGGER_SYSVIEW_EVENT_ID_NOP)
#define   SEGGER_SYSVIEW_EVENT_MASK_OVERFLOW          (1 << SEGGER_SYSVIEW_EVENT_ID_OVERFLOW)
#define   SEGGER_SYSVIEW_EVENT_MASK_ISR_ENTER         (1 << SEGGER_SYSVIEW_EVENT_ID_ISR_ENTER)
#define   SEGGER_SYSVIEW_EVENT_MASK_ISR_EXIT          (1 << SEGGER_SYSVIEW_EVENT_ID_ISR_EXIT)
#define   SEGGER_SYSVIEW_EVENT_MASK_TASK_START_EXEC   (1 << SEGGER_SYSVIEW_EVENT_ID_TASK_START_EXEC)
#define   SEGGER_SYSVIEW_EVENT_MASK_TASK_STOP_EXEC    (1 << SEGGER_SYSVIEW_EVENT_ID_TASK_STOP_EXEC)
#define   SEGGER_SYSVIEW_EVENT_MASK_TASK_START_READY  (1 << SEGGER_SYSVIEW_EVENT_ID_TASK_START_READY)
#define   SEGGER_SYSVIEW_EVENT_MASK_TASK_STOP_READY   (1 << SEGGER_SYSVIEW_EVENT_ID_TASK_STOP_READY)
#define   SEGGER_SYSVIEW_EVENT_MASK_TASK_CREATE       (1 << SEGGER_SYSVIEW_EVENT_ID_TASK_CREATE)
#define   SEGGER_SYSVIEW_EVENT_MASK_TASK_INFO         (1 << SEGGER_SYSVIEW_EVENT_ID_TASK_INFO)
#define   SEGGER_SYSVIEW_EVENT_MASK_TRACE_START       (1 << SEGGER_SYSVIEW_EVENT_ID_TRACE_START)
#define   SEGGER_SYSVIEW_EVENT_MASK_TRACE_STOP        (1 << SEGGER_SYSVIEW_EVENT_ID_TRACE_STOP)
#define   SEGGER_SYSVIEW_EVENT_MASK_SYSTIME_CYCLES    (1 << SEGGER_SYSVIEW_EVENT_ID_SYSTIME_CYCLES)
#define   SEGGER_SYSVIEW_EVENT_MASK_SYSTIME_US        (1 << SEGGER_SYSVIEW_EVENT_ID_SYSTIME_US)
#define   SEGGER_SYSVIEW_EVENT_MASK_SYSDESC           (1 << SEGGER_SYSVIEW_EVENT_ID_SYSDESC)
#define   SEGGER_SYSVIEW_EVENT_MASK_USER_START        (1 << SEGGER_SYSVIEW_EVENT_ID_USER_START)
#define   SEGGER_SYSVIEW_EVENT_MASK_USER_STOP         (1 << SEGGER_SYSVIEW_EVENT_ID_USER_STOP)
#define   SEGGER_SYSVIEW_EVENT_MASK_IDLE              (1 << SEGGER_SYSVIEW_EVENT_ID_IDLE)
#define   SEGGER_SYSVIEW_EVENT_MASK_ISR_TO_SCHEDULER  (1 << SEGGER_SYSVIEW_EVENT_ID_ISR_TO_SCHEDULER)
#define   SEGGER_SYSVIEW_EVENT_MASK_TIMER_ENTER       (1 << SEGGER_SYSVIEW_EVENT_ID_TIMER_ENTER)
#define   SEGGER_SYSVIEW_EVENT_MASK_TIMER_EXIT        (1 << SEGGER_SYSVIEW_EVENT_ID_TIMER_EXIT)
#define   SEGGER_SYSVIEW_EVENT_MASK_STACK_INFO        (1 << SEGGER_SYSVIEW_EVENT_ID_STACK_INFO)
#define   SEGGER_SYSVIEW_EVENT_MASK_MODULEDESC        (1 << SEGGER_SYSVIEW_EVENT_ID_MODULEDESC)

#define   SEGGER_SYSVIEW_EVENT_MASK_INIT              (1 << SEGGER_SYSVIEW_EVENT_ID_INIT)
#define   SEGGER_SYSVIEW_EVENT_MASK_NAME_RESOURCE     (1 << SEGGER_SYSVIEW_EVENT_ID_NAME_RESOURCE)
#define   SEGGER_SYSVIEW_EVENT_MASK_PRINT_FORMATTED   (1 << SEGGER_SYSVIEW_EVENT_ID_PRINT_FORMATTED)
#define   SEGGER_SYSVIEW_EVENT_MASK_NUMMODULES        (1 << SEGGER_SYSVIEW_EVENT_ID_NUMMODULES)
#define   SEGGER_SYSVIEW_EVENT_MASK_END_CALL          (1 << SEGGER_SYSVIEW_EVENT_ID_END_CALL)
#define   SEGGER_SYSVIEW_EVENT_MASK_TASK_TERMINATE    (1 << SEGGER_SYSVIEW_EVENT_ID_TASK_TERMINATE)

#define   SEGGER_SYSVIEW_EVENT_MASK_EX                (1 << SEGGER_SYSVIEW_EVENT_ID_EX)

#define   SEGGER_SYSVIEW_EVENT_MASK_ALL_INTERRUPTS    ( SEGGER_SYSVIEW_EVENT_MASK_ISR_ENTER           \
                                                      | SEGGER_SYSVIEW_EVENT_MASK_ISR_EXIT            \
                                                      | SEGGER_SYSVIEW_EVENT_MASK_ISR_TO_SCHEDULER)
#define   SEGGER_SYSVIEW_EVENT_MASK_ALL_TASKS         ( SEGGER_SYSVIEW_EVENT_MASK_TASK_START_EXEC     \
                                                      | SEGGER_SYSVIEW_EVENT_MASK_TASK_STOP_EXEC      \
                                                      | SEGGER_SYSVIEW_EVENT_MASK_TASK_START_READY    \
                                                      | SEGGER_SYSVIEW_EVENT_MASK_TASK_STOP_READY     \
                                                      | SEGGER_SYSVIEW_EVENT_MASK_TASK_CREATE         \
                                                      | SEGGER_SYSVIEW_EVENT_MASK_TASK_INFO           \
                                                      | SEGGER_SYSVIEW_EVENT_MASK_STACK_INFO          \
                                                      | SEGGER_SYSVIEW_EVENT_MASK_TASK_TERMINATE)

/*********************************************************************
*
*       Structures
*
**********************************************************************
*/

typedef struct {
  U32          TaskID;
  const char*  sName;
  U32          Prio;
  U32          StackBase;
  U32          StackSize;
} SEGGER_SYSVIEW_TASKINFO;

typedef struct SEGGER_SYSVIEW_MODULE SEGGER_SYSVIEW_MODULE;

struct SEGGER_SYSVIEW_MODULE {
  const char*                   sModule;
        U32                     NumEvents;
        U32                     EventOffset;
        void                    (*pfSendModuleDesc)(void);
        SEGGER_SYSVIEW_MODULE*  pNext;
};

typedef void (SEGGER_SYSVIEW_SEND_SYS_DESC_FUNC)(void);

/*********************************************************************
*
*       API functions
*
**********************************************************************
*/

typedef struct {
  U64  (*pfGetTime)      (void);
  void (*pfSendTaskList) (void);
} SEGGER_SYSVIEW_OS_API;

/*********************************************************************
*
*       Control and initialization functions
*/
void SEGGER_SYSVIEW_Init                          (U32 SysFreq, U32 CPUFreq, const SEGGER_SYSVIEW_OS_API *pOSAPI, SEGGER_SYSVIEW_SEND_SYS_DESC_FUNC pfSendSysDesc);
void SEGGER_SYSVIEW_SetRAMBase                    (U32 RAMBaseAddress);
void SEGGER_SYSVIEW_Start                         (void);
void SEGGER_SYSVIEW_Stop                          (void);
void SEGGER_SYSVIEW_GetSysDesc                    (void);
void SEGGER_SYSVIEW_SendTaskList                  (void);
void SEGGER_SYSVIEW_SendTaskInfo                  (const SEGGER_SYSVIEW_TASKINFO* pInfo);
void SEGGER_SYSVIEW_SendSysDesc                   (const char* sSysDesc);

/*********************************************************************
*
*       Event recording functions
*/
void SEGGER_SYSVIEW_RecordVoid                    (unsigned EventId);
void SEGGER_SYSVIEW_RecordU32                     (unsigned EventId, U32 Para0);
void SEGGER_SYSVIEW_RecordU32x2                   (unsigned EventId, U32 Para0, U32 Para1);
void SEGGER_SYSVIEW_RecordU32x3                   (unsigned EventId, U32 Para0, U32 Para1, U32 Para2);
void SEGGER_SYSVIEW_RecordU32x4                   (unsigned EventId, U32 Para0, U32 Para1, U32 Para2, U32 Para3);
void SEGGER_SYSVIEW_RecordU32x5                   (unsigned EventId, U32 Para0, U32 Para1, U32 Para2, U32 Para3, U32 Para4);
void SEGGER_SYSVIEW_RecordU32x6                   (unsigned EventId, U32 Para0, U32 Para1, U32 Para2, U32 Para3, U32 Para4, U32 Para5);
void SEGGER_SYSVIEW_RecordU32x7                   (unsigned EventId, U32 Para0, U32 Para1, U32 Para2, U32 Para3, U32 Para4, U32 Para5, U32 Para6);
void SEGGER_SYSVIEW_RecordU32x8                   (unsigned EventId, U32 Para0, U32 Para1, U32 Para2, U32 Para3, U32 Para4, U32 Para5, U32 Para6, U32 Para7);
void SEGGER_SYSVIEW_RecordU32x9                   (unsigned EventId, U32 Para0, U32 Para1, U32 Para2, U32 Para3, U32 Para4, U32 Para5, U32 Para6, U32 Para7, U32 Para8);
void SEGGER_SYSVIEW_RecordU32x10                  (unsigned EventId, U32 Para0, U32 Para1, U32 Para2, U32 Para3, U32 Para4, U32 Para5, U32 Para6, U32 Para7, U32 Para8, U32 Para9);
void SEGGER_SYSVIEW_RecordString                  (unsigned EventId, const char* pString);
void SEGGER_SYSVIEW_RecordSystime                 (void);
void SEGGER_SYSVIEW_RecordEnterISR                (void);
void SEGGER_SYSVIEW_RecordExitISR                 (void);
void SEGGER_SYSVIEW_RecordExitISRToScheduler      (void);
void SEGGER_SYSVIEW_RecordEnterTimer              (U32 TimerId);
void SEGGER_SYSVIEW_RecordExitTimer               (void);
void SEGGER_SYSVIEW_RecordEndCall                 (unsigned EventID);
void SEGGER_SYSVIEW_RecordEndCallU32              (unsigned EventID, U32 Para0);

void SEGGER_SYSVIEW_OnIdle                        (void);
void SEGGER_SYSVIEW_OnTaskCreate                  (unsigned TaskId);
void SEGGER_SYSVIEW_OnTaskTerminate               (unsigned TaskId);
void SEGGER_SYSVIEW_OnTaskStartExec               (unsigned TaskId);
void SEGGER_SYSVIEW_OnTaskStopExec                (void);
void SEGGER_SYSVIEW_OnTaskStartReady              (unsigned TaskId);
void SEGGER_SYSVIEW_OnTaskStopReady               (unsigned TaskId, unsigned Cause);
void SEGGER_SYSVIEW_OnUserStart                   (unsigned UserId);       // Start of user defined event (such as a subroutine to profile)
void SEGGER_SYSVIEW_OnUserStop                    (unsigned UserId);       // Start of user defined event

void SEGGER_SYSVIEW_NameResource                  (U32 ResourceId, const char* sName);

int  SEGGER_SYSVIEW_SendPacket                    (U8* pPacket, U8* pPayloadEnd, unsigned EventId);

/*********************************************************************
*
*       Event parameter encoding functions
*/
U8*  SEGGER_SYSVIEW_EncodeU32                     (U8* pPayload, unsigned Value);
U8*  SEGGER_SYSVIEW_EncodeData                    (U8* pPayload, const char* pSrc, unsigned Len);
U8*  SEGGER_SYSVIEW_EncodeString                  (U8* pPayload, const char* s, unsigned MaxLen);
U8*  SEGGER_SYSVIEW_EncodeId                      (U8* pPayload, unsigned Id);
U32  SEGGER_SYSVIEW_ShrinkId                      (U32 Id);


/*********************************************************************
*
*       Middleware module registration
*/
void SEGGER_SYSVIEW_RegisterModule                (SEGGER_SYSVIEW_MODULE* pModule);
void SEGGER_SYSVIEW_RecordModuleDescription       (const SEGGER_SYSVIEW_MODULE* pModule, const char* sDescription);
void SEGGER_SYSVIEW_SendModule                    (U8 ModuleId);
void SEGGER_SYSVIEW_SendModuleDescription         (void);
void SEGGER_SYSVIEW_SendNumModules                (void);

/*********************************************************************
*
*       printf-Style functions
*/
#ifndef SEGGER_SYSVIEW_EXCLUDE_PRINTF // Define in project to avoid warnings about variable parameter list
void SEGGER_SYSVIEW_PrintfHostEx                  (const char* s, U32 Options, ...);
void SEGGER_SYSVIEW_PrintfTargetEx                (const char* s, U32 Options, ...);
void SEGGER_SYSVIEW_PrintfHost                    (const char* s, ...);
void SEGGER_SYSVIEW_PrintfTarget                  (const char* s, ...);
void SEGGER_SYSVIEW_WarnfHost                     (const char* s, ...);
void SEGGER_SYSVIEW_WarnfTarget                   (const char* s, ...);
void SEGGER_SYSVIEW_ErrorfHost                    (const char* s, ...);
void SEGGER_SYSVIEW_ErrorfTarget                  (const char* s, ...);
#endif

void SEGGER_SYSVIEW_Print                         (const char* s);
void SEGGER_SYSVIEW_Warn                          (const char* s);
void SEGGER_SYSVIEW_Error                         (const char* s);

/*********************************************************************
*
*       Run-time configuration functions
*/
void SEGGER_SYSVIEW_EnableEvents                  (U32 EnableMask);
void SEGGER_SYSVIEW_DisableEvents                 (U32 DisableMask);

/*********************************************************************
*
*       Application-provided functions
*/
void SEGGER_SYSVIEW_Conf                          (void);
U32  SEGGER_SYSVIEW_X_GetTimestamp                (void);
U32  SEGGER_SYSVIEW_X_GetInterruptId              (void);

#ifdef __cplusplus
}
#endif

#endif

/*************************** End of file ****************************/
