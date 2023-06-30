#include "swtimer.h"


#ifdef _USE_HW_SWTIMER

typedef struct
{

  uint8_t  Timer_En;             // 타이머 인에이블 신호
  uint8_t  Timer_Mode;           // 타이머 모드
  uint32_t Timer_Ctn;            // 현제의 타이머 값
  uint32_t Timer_Init;           // 타이머 초기화될때의 카운트 값
  void (*TmrFnct)(void *);       // 만료될때 실행될 함수
  void *TmrFnctArg;              // 함수로 전달할 인수들
} swtimer_t;



//-- Internal Variables
//
static volatile uint32_t sw_timer_counter      = 0;
static volatile uint16_t sw_timer_handle_index = 0;
static swtimer_t  swtimer_tbl[_HW_DEF_SW_TIMER_MAX];           // 타이머 배열 선언



//-- External Variables
//


//-- Internal Functions
//



//-- External Functions
//


bool swtimerInit(void)
{
  uint8_t i;
  static uint8_t excute = 0;


  if (excute == 1)
  {
    return false;  // 이미 한번 실행했다면 정지.
  }


  // 구조체 초기화
  for(i=0; i<_HW_DEF_SW_TIMER_MAX; i++)
  {
    swtimer_tbl[i].Timer_En   = OFF;
    swtimer_tbl[i].Timer_Ctn  = 0;
    swtimer_tbl[i].Timer_Init = 0;
    swtimer_tbl[i].TmrFnct    = NULL;
  }

  excute = 1;

  return true;
}

void swtimerISR(void)
{
  uint8_t i;


  sw_timer_counter++;


  for (i=0; i<_HW_DEF_SW_TIMER_MAX && i<sw_timer_handle_index; i++)     // 타이머 갯수만큼
  {
    if ( swtimer_tbl[i].Timer_En == ON)                         // 타이머가 활성화 됬니?
    {
      swtimer_tbl[i].Timer_Ctn--;                               // 타이머값 감소

      if (swtimer_tbl[i].Timer_Ctn == 0)                        // 타이머 오버플로어
      {
        if(swtimer_tbl[i].Timer_Mode == ONE_TIME)               // 한번만 실행하는거면
        {
          swtimer_tbl[i].Timer_En = OFF;                        // 타이머 OFF 한다.
        }

        swtimer_tbl[i].Timer_Ctn = swtimer_tbl[i].Timer_Init;   // 타이머 초기화

        (*swtimer_tbl[i].TmrFnct)(swtimer_tbl[i].TmrFnctArg);   // 함수 실행
      }
    }
  }

}

void swtimerSet(swtimer_handle_t TmrNum, uint32_t TmrData, uint8_t TmrMode, void (*Fnct)(void *),void *arg)
{
  if(TmrNum < 0) return;

  swtimer_tbl[TmrNum].Timer_Mode = TmrMode;    // 모드설정
  swtimer_tbl[TmrNum].TmrFnct    = Fnct;       // 실행할 함수
  swtimer_tbl[TmrNum].TmrFnctArg = arg;        // 매개변수
  swtimer_tbl[TmrNum].Timer_Ctn  = TmrData;
  swtimer_tbl[TmrNum].Timer_Init = TmrData;
}

void swtimerStart(swtimer_handle_t TmrNum)
{
  if(TmrNum < 0) return;

  swtimer_tbl[TmrNum].Timer_Ctn = swtimer_tbl[TmrNum].Timer_Init;
  swtimer_tbl[TmrNum].Timer_En  = ON;
}

void swtimerStop (swtimer_handle_t TmrNum)
{
  if(TmrNum < 0) return;

  swtimer_tbl[TmrNum].Timer_En = OFF;
}

void swtimerReset(swtimer_handle_t TmrNum)
{
  if(TmrNum < 0) return;

  swtimer_tbl[TmrNum].Timer_En   = OFF;
  swtimer_tbl[TmrNum].Timer_Ctn  = swtimer_tbl[TmrNum].Timer_Init;
}

swtimer_handle_t swtimerGetHandle(void)
{
  swtimer_handle_t TmrIndex = sw_timer_handle_index;

  if (TmrIndex < _HW_DEF_SW_TIMER_MAX)
    sw_timer_handle_index++;
  else
    TmrIndex = -1;

  return TmrIndex;
}

uint32_t swtimerGetCounter(void)
{
  return sw_timer_counter;
}

#endif
