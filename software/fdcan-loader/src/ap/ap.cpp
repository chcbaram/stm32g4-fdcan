/*
 * ap.c
 *
 *  Created on: Dec 6, 2020
 *      Author: baram
 */


#include "ap.h"
#include "boot/boot.h"
#include "audio/audio.h"


enum
{
  MODE_CLI,
  MODE_DOWN
};



arg_option_t arg_option;


void apShowHelp(void);
bool apGetOption(int argc, char *argv[]);
void apDownMode(void);
int32_t getFileSize(char *file_name);
int32_t getFileVersion(char *file_name, firm_ver_t *p_ver);
uint32_t getFileCrc(char *file_name);





void apInit(void)
{
  logPrintf("FDCAN-LOADER V230715R2\n\n");

  arg_option.mode = MODE_DOWN;
  arg_option.is_udp = false;
  arg_option.is_audio = false;
}

void apMain(int argc, char *argv[])
{
  bool ret;


  ret = apGetOption(argc, argv);
  if (ret != true)
  {
    apExit();
    return;
  }

  if (arg_option.is_audio)
  {
    audioMain(&arg_option);
  }
  else
  {
    apDownMode();
  }

  apExit();
}

bool apGetOption(int argc, char *argv[])
{
  int opt;
  bool ret = true;
  int argc_i = 0;

  arg_option.run_fw      = true;
  arg_option.arg_bits    = 0;
  arg_option.port_baud   = 19200;
  arg_option.tx_block_len = 256;


  while((opt = getopt(argc, argv, "m:t:hcp:b:f:a:rv:l")) != -1)
  {
    switch(opt)
    {
      case 'm':
        if (strncmp(argv[optind-1], "udp", 3) == 0)
        {
          arg_option.is_udp = true;
          arg_option.tx_block_len = 1024;
          logPrintf("-m udp\n");
        }
        else if (strncmp(argv[optind-1], "audio", 5) == 0)
        {
          arg_option.is_audio = true;
          logPrintf("-m audio\n");
        }
        else
        {
          logPrintf("-m uart\n");
        }
        break;

      case 'h':
        apShowHelp();
        ret = false;
        break;

      case 'c':
        arg_option.mode = MODE_CLI;
        break;

      case 't':
        if (strncmp(argv[optind-1], "sai", 3) == 0)
        {
          arg_option.type = 1;
          logPrintf("-t sai\n");
        }
        else
        {
          arg_option.type = 0;
          logPrintf("-t i2s\n");
        }        
        break;

      case 'p':
        arg_option.arg_bits |= ARG_OPTION_PORT;
        strncpy(arg_option.port_str, optarg, 128);
        logPrintf("-p %s\n", arg_option.port_str);
        break;

      case 'b':
        arg_option.arg_bits |= ARG_OPTION_BAUD;
        arg_option.port_baud = (uint32_t)strtoul((const char * )optarg, (char **)NULL, (int) 0);
        logPrintf("-b %d\n", arg_option.port_baud);
        break;

      case 'f':
        arg_option.arg_bits |= ARG_OPTION_FILE;
        strncpy(arg_option.file_str, optarg, 128);
        logPrintf("-f %s\n", arg_option.file_str);
        break;

      case 'r':
        arg_option.run_fw = true;
        logPrintf("-r 1\n");
        break;

      case '?':
        logPrintf("Unknown\n");
        break;
    }

    argc_i++;
  }

  if (arg_option.is_udp == true)
  {
    if ((arg_option.arg_bits & ARG_OPTION_PORT) == 0)
    {
      const char ip_str[32] = "255.255.255.255";
      strncpy(arg_option.port_str, ip_str, 128);
      arg_option.arg_bits |= ARG_OPTION_PORT;
      logPrintf("-p %s\n", arg_option.port_str);
    }
  }

  if (argc == 1)
  {
    apShowHelp();
    ret = false;
  }
  return ret;
}

void apShowHelp(void)
{
  logPrintf("fdcan-loader [udp] -p com1 -f fw.bin\n");
  logPrintf("            -h : help\n");
  logPrintf("            -m udp   : udp \n");
  logPrintf("            -p com1  : com port\n");
  logPrintf("            -b 19200 : baud\n");
  logPrintf("            -f fw.bin: firmware\n");
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

int32_t getFileVersion(char *file_name, firm_ver_t *p_ver)
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
    fseek( fp, BOOT_SIZE_VER, SEEK_SET );
    ret = fread(p_ver, 1, sizeof(firm_ver_t), fp);
    fclose(fp);
  }

  return ret;
}

uint32_t getFileCrc(char *file_name)
{
  uint32_t ret = 0;
  FILE *fp;

  if ((fp = fopen( file_name, "rb")) == NULL)
  {
    fprintf( stderr, "Unable to open %s\n", file_name );
    return -1;
  }
  else
  {
    uint8_t buffer[1024];
    int len;
    uint16_t calc_crc = 0;

    while(1)
    {
      len = fread(buffer, 1, 1024, fp);
      if (len > 0)
      {
        for (int i=0; i<len; i++)
        {
          utilUpdateCrc(&calc_crc, buffer[i]);
        }
      }
      else
      {
        break;
      }
    }
    ret = calc_crc;

    fclose(fp);
  }

  return ret;
}

void apDownMode(void)
{
  uint32_t arg_check = ARG_OPTION_PORT | ARG_OPTION_FILE;

  logPrintf("\n");

  if ((arg_option.arg_bits & arg_check) != arg_check)
  {
    if ((arg_option.arg_bits & ARG_OPTION_PORT) == 0)
    {
      logPrintf("-p port empty\n");
    }
    if ((arg_option.arg_bits & ARG_OPTION_FILE) == 0)
    {
      logPrintf("-f file empty\n");
    }

    return;
  }

  logPrintf("[ Download Begin.. ]\n\n");


  uint8_t err_code;
  char *file_name;
  uint32_t addr;
  int32_t file_len;
  uint32_t baud;

  boot_info_t boot_info;
  boot_version_t boot_ver;
  boot_begin_t boot_begin;

  firm_ver_t firm_ver;
  firm_tag_t firm_tag;
  uint32_t pre_time;
  bool ret;

  file_name = arg_option.file_str;
  addr = 0;
  baud = arg_option.port_baud;

  file_len = getFileSize(file_name);
  if (file_len < 0)
  {
    apExit();
  }

  logPrintf("## File Open \n");
  logPrintf("##\n");  
  logPrintf("file_name  : %s \n", file_name);
  logPrintf("file_len   : %d Bytes\n", file_len);

  if (file_len <= 0)
  {
    logPrintf("File not available\n");
    apExit();
  }

  firm_tag.magic_number = TAG_MAGIC_NUMBER;
  firm_tag.fw_addr = BOOT_SIZE_TAG;
  firm_tag.fw_crc = 0;
  firm_tag.fw_size = file_len;
  firm_tag.fw_crc = getFileCrc(file_name);

  logPrintf("file_crc   : 0x%04X\n", firm_tag.fw_crc);

  if (getFileVersion(file_name, &firm_ver) < 0)
  {
    apExit();
  }
  if (firm_ver.magic_number != VERSION_MAGIC_NUMBER)
  {
    firm_ver.version_str[0] = 0;
    firm_ver.name_str[0] = 0;
  }
  logPrintf("firm ver   : %s\n", firm_ver.version_str);    
  logPrintf("firm name  : %s\n", firm_ver.name_str);  
  logPrintf("firm addr  : 0x%X\n", firm_ver.firm_addr);



  strncpy(boot_begin.fw_name, file_name, 64);
  boot_begin.fw_size = file_len;

  FILE *fp;

  if ((fp = fopen(file_name, "rb")) == NULL)
  {
    logPrintf("Unable to open %s\n", file_name);
    apExit();
  }


  if (arg_option.is_udp == true)
    ret = bootInitUdp(arg_option.port_str, 5000);
  else
    ret = bootInit(_USE_UART_CMD, arg_option.port_str, baud);
  if (ret != true)
  {
    logPrintf("bootInit() Fail\n");
    apExit();
  }

  while(1)
  {
    if (file_len <= 0) break;


    logPrintf("\n");
    

    // Read Info
    //
    logPrintf("## Read Info \n");
    logPrintf("##\n");
    for (int i=0; i<3; i++)
    {
      err_code = bootCmdReadInfo(&boot_info, 500);
      if (err_code == CMD_OK)
        break;
    }
    if (err_code == CMD_OK)
    {
      if (boot_info.mode == 0)
        logPrintf("bootloader mode\n");
      else
        logPrintf("firmware mode\n");

      if (arg_option.is_udp)
      {
        bootGetDriver()->ioctl(0, NULL, 0);
      }
    }
    else
    {
      logPrintf("Fail : 0x%04X\n", err_code);
      break;
    }
    logPrintf("\n");


    // Begin
    //
    logPrintf("## Begin \n");
    logPrintf("##\n");
    pre_time = millis();
    err_code = bootCmdFirmBegin(&boot_begin, 500);
    if (err_code != CMD_OK)
    {
      logPrintf("bootCmdFirmBegin() : fail 0x%04X\n", err_code);   
      break; 
    }
    logPrintf("OK %d ms\n\n", millis()-pre_time);


    // Read Version
    //
    logPrintf("## Read Version \n");
    logPrintf("##\n");
    pre_time = millis();
    err_code = bootCmdReadVersion(&boot_ver, 500);
    if (err_code == CMD_OK)
    {
      logPrintf("boot   name: %s\n", boot_ver.boot.name_str);    
      logPrintf("firm   name: %s\n", boot_ver.firm.name_str); 
      logPrintf("boot   ver : %s\n", boot_ver.boot.version_str);    
      logPrintf("firm   ver : %s\n", boot_ver.firm.version_str); 
    }
    else
    {
      logPrintf("bootCmdReadVersion() : fail 0x%04X\n", err_code);   
      break; 
    }
    logPrintf("OK %d ms\n\n", millis()-pre_time);


    addr = 0;

    // 1. Flash Erase
    //
    logPrintf("firm erase : ");
    pre_time = millis();
    err_code = bootCmdFirmErase(addr, BOOT_SIZE_TAG + file_len, 5000);
    if (err_code != CMD_OK)
    {
      logPrintf("Fail, 0x%04X\n", err_code);      
      break;
    }
    logPrintf("OK, %d ms\n", millis()-pre_time);    

    addr = BOOT_SIZE_TAG;

    // 2. Flash Write
    //
    uint32_t tx_block_size = arg_option.tx_block_len;
    uint8_t  tx_buf[arg_option.tx_block_len];
    uint32_t tx_len;
    int32_t  len_to_send;
    bool write_done = false;
    
    tx_len = 0;    
    pre_time = millis();
    while(tx_len < file_len)
    {
      len_to_send = fread(tx_buf, 1, tx_block_size, fp);
      if (len_to_send <= 0)
      {
        break;
      }

      err_code = bootCmdFirmWrite(addr + tx_len, tx_buf, len_to_send, 500);
      if (err_code != CMD_OK)
      {
        logPrintf("firm write Fail \n");        
        logPrintf("           err  : 0x%04X\n", err_code);        
        logPrintf("           addr : 0x%04X\n", addr + tx_len);        
        break;
      }
      else
      {
        logPrintf("firm write : %d%%\r", ((tx_len+len_to_send)*100/file_len));
      }

      tx_len += len_to_send;    

      if (tx_len == file_len)
      {
        logPrintf("firm write : OK, %d ms\n", millis()-pre_time);
        write_done = true;
        break;
      }      
    }

    logPrintf("\n");

    if (write_done == true)
    {      
      err_code = bootCmdFirmWrite(0, (uint8_t *)&firm_tag, sizeof(firm_tag), 500);
      if (err_code == CMD_OK)
      {
        logPrintf("tag  write : OK\n");
      }
      else
      {
        logPrintf("tag  write : Fail, %d\n", err_code);
        break;
      }

      pre_time = millis();
      err_code = bootCmdFirmVerify(1000);
      if (err_code != CMD_OK)
      {
        logPrintf("tag  verify: Fail\n");
        logPrintf("      err  : 0x%04X\n", err_code);        
        break;
      }
      else
      {
        logPrintf("tag  verify: OK, %dms\n", millis()-pre_time);
      }

      err_code = bootCmdFirmEnd(500);

      pre_time = millis();
      err_code = bootCmdFirmUpdate(5000);
      if (err_code == CMD_OK)
      {
        logPrintf("firm update: OK, %dms\n", millis()-pre_time);
      }
      else
      {
        logPrintf("firm update: Fail\n");
        logPrintf("      err  : 0x%04X\n", err_code);        
        break;
      }
    }
    else
    {
      err_code = bootCmdFirmEnd(500);
    }

    break;
  }

  fclose(fp);

  logPrintf("\n");
  logPrintf("[ Download End.. ]");
}


void apExit(void)
{
  printf("\n");
  bootDeInit();
  audioDeInit();

  for (int i=0; i<UART_MAX_CH; i++)
  {
    uartClose(i);
  }
  printf("\nexit\n");
  exit(0);
}

