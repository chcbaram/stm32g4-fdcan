#include "audio.h"
#include "audio_def.h"
#include "cmd/driver/cmd_udp.h"


#define AUDIO_CMD_INFO              0x0020
#define AUDIO_CMD_BEGIN             0x0021
#define AUDIO_CMD_END               0x0022
#define AUDIO_CMD_READY             0x0023
#define AUDIO_CMD_WRITE             0x0024
#define AUDIO_CMD_WRITE_NO_RESP     0x0025



#define USE_WRITE_RESP              0



typedef struct
{
  uint8_t  hw_type;
  char     file_name[128];
  uint32_t sample_rate;
  uint32_t file_size;
  uint8_t  file_type;
} audio_begin_t;

static uint16_t audioCmdBegin(audio_begin_t *p_data, uint32_t timeout);
static uint16_t audioCmdEnd(uint32_t timeout);
static uint16_t audioCmdReady(uint32_t *p_data, uint32_t timeout);
#if USE_WRITE_RESP == 1
static uint16_t audioCmdWrite(void *p_data, uint32_t length, uint32_t timeout);
#else
static uint16_t audioCmdWriteNoResp(void *p_data, uint32_t length);
#endif
static char *getFileNameFromPath(char *path );
static int32_t getFileSize(char *file_name);

static bool is_init = false;
static cmd_t cmd;
static cmd_driver_t cmd_driver;
static audio_begin_t audio_begin;





void audioMain(arg_option_t *args)
{
  logPrintf("\n");
  logPrintf("audioMain()\n");



  char *file_name;
  char *file_path;
  uint8_t *file_buf;
  int32_t file_size;
  int32_t file_index;
  FILE *fp;
  wavfile_header_t header;
  int32_t  volume = 100;


  if ((args->arg_bits & ARG_OPTION_PORT) == 0)
  {
    const char ip_str[32] = "255.255.255.255";
    strncpy(args->port_str, ip_str, 128);
    args->arg_bits |= ARG_OPTION_PORT;
    logPrintf("-p %s\n", args->port_str);
  }

  cmdUdpInitDriver(&cmd_driver, args->port_str, 5000);
  cmdInit(&cmd, &cmd_driver);
  cmdOpen(&cmd);  

  uartOpen(_USE_UART_CLI, 115200);

  file_path = args->file_str;
  file_name = getFileNameFromPath(args->file_str);
  file_size = getFileSize(file_path);

  logPrintf("FileName      : %s\n", file_name);
  logPrintf("FileSize      : %d KB\n", file_size/1024);
  logPrintf("\n");

  file_buf = (uint8_t *)malloc(file_size);
  if (file_buf == NULL)
  {
    logPrintf("malloc fail %d\n", file_size);
    return;
  }
  logPrintf("malloc()\n");

  //-- File Open
  //
  fp = fopen(file_path, "r");
  if (fp == NULL)
  {
    free(file_buf);
    logPrintf("fopen fail : %s\n", file_path);
    return;
  }  


  file_index = 0;
  fread(file_buf, 1, file_size, fp);
  fclose(fp);

  memcpy(&header, &file_buf[file_index], sizeof(wavfile_header_t));

  logPrintf("ChunkSize     : %d\n", header.ChunkSize);
  logPrintf("Format        : %c%c%c%c\n", header.Format[0], header.Format[1], header.Format[2], header.Format[3]);
  logPrintf("Subchunk1Size : %d\n", header.Subchunk1Size);
  logPrintf("AudioFormat   : %d\n", header.AudioFormat);
  logPrintf("NumChannels   : %d\n", header.NumChannels);
  logPrintf("SampleRate    : %d\n", header.SampleRate);
  logPrintf("ByteRate      : %d\n", header.ByteRate);
  logPrintf("BlockAlign    : %d\n", header.BlockAlign);
  logPrintf("BitsPerSample : %d\n", header.BitsPerSample);
  logPrintf("Subchunk2Size : %d\n", header.Subchunk2Size);
  logPrintf("\n");


  int16_t buf_frame[4096];

  file_index += 1024;

  uint32_t ready_cnt;
  uint16_t err_ret;
  bool is_begin = true;
  bool is_end = false;


  // audioBegin()
  //
  audio_begin.hw_type = args->type;
  strncpy(audio_begin.file_name, file_name, 128);
  audio_begin.file_size = file_size;
  audio_begin.sample_rate = header.SampleRate;
  
  err_ret = audioCmdBegin(&audio_begin, 100);
  if (err_ret != CMD_OK)
  {
    logPrintf("audioCmdBegin()\n  err 0x%04X\n", err_ret);
    is_begin = false;
  }


  while(uartAvailable(_DEF_UART1) == 0 && is_begin && is_end == false)
  {
    int len;
    bool ready_ret;
    int w_len;

    ready_ret = false;
    ready_cnt = 0;
    for (int i=0; i<3; i++)
    {
      if (audioCmdReady(&ready_cnt, 100) == CMD_OK)
      {
        ready_ret = true;
        break;
      }
    }
    if (ready_ret == false)
    {
      logPrintf("audioCmdReady() Fail\n");
      break;
    }


    w_len = 0;
    while(w_len < (int)ready_cnt)
    {
      int r_len;

      r_len = constrain((ready_cnt-w_len), 0, 700);
      r_len = r_len / 2;

      w_len += (r_len * 2);

      len = constrain((file_size-file_index), 0, r_len * 2 * header.NumChannels);
      if (len == 0)
      {
        logPrintf("File End\n");
        is_end = true;
        break;
      }
      memcpy(buf_frame, &file_buf[file_index], len);
      file_index += len;

      float percent;

      percent = (float)file_index * 100. / (float)file_size;

      logPrintf("Play          : %3.2f%%, %d   \r", percent, ready_cnt);

      int16_t buf_data[r_len*2 + 2];

      buf_data[0] = (file_index >>  0) & 0xFFFF;
      buf_data[1] = (file_index >> 16) & 0xFFFF;

      for (int i=0; i<r_len; i++)
      {
        if (header.NumChannels == 2)
        {
          buf_data[i*2 + 0 + 2] = buf_frame[i*2 + 0] * volume / 100;;
          buf_data[i*2 + 1 + 2] = buf_frame[i*2 + 1] * volume / 100;;
        }
        else
        {
          buf_data[i*2 + 0 + 2] = buf_frame[i] * volume / 100;;
          buf_data[i*2 + 1 + 2] = buf_frame[i] * volume / 100;;
        }
      }

      #if USE_WRITE_RESP == 1
      err_ret = audioCmdWrite(buf_data, r_len * 2 * 2 + 4, 50);
      #else
      err_ret = audioCmdWriteNoResp(buf_data, r_len * 2 * 2 + 4);
      #endif
      if (err_ret != CMD_OK)
      {
        logPrintf("audioCmdWrite() Fail\n");
        // break;
      }
    }
  }

  audioCmdEnd(100);

  free(file_buf);
  logPrintf("free()\n");

  is_init = true;
  return;
}

uint16_t audioCmdBegin(audio_begin_t *p_data, uint32_t timeout)
{
  uint16_t ret = CMD_OK;
  cmd_t *p_cmd = &cmd;


  cmdSendCmdRxResp(p_cmd, AUDIO_CMD_BEGIN, (uint8_t *)p_data, sizeof(audio_begin_t), timeout);
  ret = p_cmd->packet.err_code;

  return ret;
}

uint16_t audioCmdEnd(uint32_t timeout)
{
  uint16_t ret = CMD_OK;
  cmd_t *p_cmd = &cmd;


  cmdSendCmdRxResp(p_cmd, AUDIO_CMD_END, NULL, 0, timeout);
  ret = p_cmd->packet.err_code;

  return ret;
}

uint16_t audioCmdReady(uint32_t *p_data, uint32_t timeout)
{
  uint16_t ret = CMD_OK;
  cmd_t *p_cmd = &cmd;

  *p_data = 0;

  if (cmdSendCmdRxResp(p_cmd, AUDIO_CMD_READY, NULL, 0, timeout) == true)
  {
    cmd_packet_t *p_packet = &p_cmd->packet;

    if (p_packet->err_code == CMD_OK)
    {
      memcpy(p_data, p_cmd->packet.data, 4);
    }
  } 
  ret = p_cmd->packet.err_code;

  return ret;
}

uint16_t audioCmdWrite(void *p_data, uint32_t length, uint32_t timeout)
{
  uint16_t ret = CMD_OK;
  cmd_t *p_cmd = &cmd;


  if (cmdSendCmdRxResp(p_cmd, AUDIO_CMD_WRITE, (uint8_t *)p_data, length, timeout) == true)
  {
    cmd_packet_t *p_packet = &p_cmd->packet;

    if (p_packet->err_code == CMD_OK)
    {
    }
  } 
  ret = p_cmd->packet.err_code;

  return ret;
}

uint16_t audioCmdWriteNoResp(void *p_data, uint32_t length)
{
  uint16_t ret = CMD_OK;
  cmd_t *p_cmd = &cmd;

  cmdSendCmd(p_cmd, AUDIO_CMD_WRITE_NO_RESP, (uint8_t *)p_data, length);

  return ret;
}

void audioDeInit(void)
{
  if (is_init)
  {
    cmdClose(&cmd);
  }
}

char *getFileNameFromPath(char *path )
{
  if( path == NULL )
      return NULL;

  char  *pFileName = path;
  for (char * pCur = path; *pCur != '\0'; pCur++)
  {
    if( *pCur == '/' || *pCur == '\\' )
      pFileName = pCur+1;
  }
  
  return pFileName;
}

int32_t getFileSize(char *file_name)
{
  int32_t ret = -1;
  FILE *fp;

  if ((fp = fopen( file_name, "rb")) == NULL)
  {
    fprintf( stderr, "Unable to open %s\n", file_name );
    return -1;
  }
  else
  {
    fseek( fp, 0, SEEK_END );
    ret = ftell( fp );
    fclose(fp);
  }

  return ret;
}