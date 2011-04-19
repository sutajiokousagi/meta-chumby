/*
 * cp_interface.h
 *
 * Aaron "Caustik" Robinson
 * (c) Copyright Chumby Industries, 2007
 * All rights reserved
 *
 * This API defines an interface for accessing the Crypto Processor.
 *
 * CPI := Crypto Processor Interface (this module)
 */

#ifndef CP_INTERFACE_H
#define CP_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/*! \name forward declarations */
/*! \{ */
struct _cpi_info_t;
struct _cpi_t;
/*! \} */

/*!

 Create an instance of the Crypto Processor Interface.

  @param p_cpi_info (INP) - CPI Info used to initialize CPI instance
  @param pp_cpi (OUT) - Pointer to CPI instance
  @return CPI_OK for success, otherwise CPI_ error code

*/

int cpi_create(struct _cpi_info_t *p_cpi_info, struct _cpi_t **pp_cpi);

/*!

 Closes an instance of the Crypto Processor Interface.

  @param p_cpi (INP) - CPI instance
  @return CPI_OK for success, otherwise CPI_ error code

 */

int cpi_close(struct _cpi_t *p_cpi);

/*!

 Initialize an instance of the Crypto Processor Interface. Must be called before
 any other API calls, aside from cpi_create and cpi_close.

  @param p_cpi (INP) - CPI instance
  @param serial_device_path (INP) - Path to proper serial device (e.g. "/dev/ttyS2")
  @return CPI_OK for success, otherwise CPI_ error code

 */

int cpi_init(struct _cpi_t *p_cpi, char *serial_device_path);

/*!

 Process the specified XML command, and return the result in XML.

  @param p_cpi (INP) - CPI instance
  @param inp_xml_str (INP) - XML input string (<= CPI_MAX_RESULT_SIZE bytes, including null terminator)
  @param out_xml_str (OUT) - XML output string (>= CPI_MAX_RESULT_SIZE bytes, including null terminator)
  @return CPI_OK for success, otherwise CPI_ error code

 */

int cpi_process_xml(struct _cpi_t *p_cpi, char *inp_xml_str, char *out_xml_str);

/*!

 Get the putative ID of the specified key.

  @param p_cpi (INP) - CPI instance
  @param cpi_key_id (INP) - CPI Key ID
  @param str (OUT) - ASCII buffer at least CPI_PUTATIVE_ID_SIZE bytes in size. Format
                     is ASCII GUID format (e.g. "01234567-89AB-CDEF-0123-456789ABCDEF")
  @return CPI_OK for success, otherwise CPI_ error code

 */

int cpi_get_putative_id(struct _cpi_t *p_cpi, uint16_t cpi_key_id, char *str);

/*!

 Get the public key.

  @param p_cpi (INP) - CPI instance
  @param cpi_key_id (INP) - CPI Key ID
  @param str (OUT) - ASCII buffer at least CPI_MAX_RESULT_SIZE bytes in size. Format
                     is RFC2440 section 5.5.2-compliant version 3 public key subkey packet
  @return CPI_OK for success, otherwise CPI_ error code

 */

int cpi_get_public_key(struct _cpi_t *p_cpi, uint16_t cpi_key_id, char *str);

/*!

 Get the raw version data of the CP.

  @param p_cpi (INP) - CPI instance
  @param raw_vers (OUT) - Buffer at least CPI_VERSION_SIZE bytes in size.
  @return CPI_OK for success, otherwise CPI_ error code

 */

int cpi_get_version_data(struct _cpi_t *p_cpi, uint8_t *raw_vers);

/*!

 Get the version number of the CP.

  @param p_cpi (INP) - CPI instance
  @param p_vers_major (OUT) - Version "major" component.
  @param p_vers_minor (OUT) - Version "minor" component.
  @param p_vers_fix (OUT) - Version "fix" component.
  @return CPI_OK for success, otherwise CPI_ error code

 */

int cpi_get_version_number(struct _cpi_t *p_cpi, uint16_t *p_vers_major, uint16_t *p_vers_minor, uint16_t *p_vers_fix);

/*!

 Trigger power down of system using the CP.

  @param p_cpi (INP) - CPI instance
  @param alarm_time (INP) - Time, in seconds, to trigger alarm (relative to cpi_get_current_time).
  @return CPI_OK for success, otherwise CPI_ error code

 */

int cpi_set_alarm_time(struct _cpi_t *p_cpi, uint32_t alarm_time);

/*!

 Trigger power down of system using the CP.

  @param p_cpi (INP) - CPI instance
  @return CPI_OK for success, otherwise CPI_ error code

 */

int cpi_trigger_power_down(struct _cpi_t *p_cpi);

/*!

 Trigger reset of system using the CP.

  @param p_cpi (INP) - CPI instance
  @return CPI_OK for success, otherwise CPI_ error code

 */

int cpi_trigger_reset(struct _cpi_t *p_cpi);

/*!

 Retrieve current time, in seconds, since last CP reboot.

  @param p_cpi (INP) - CPI instance
  @param p_cur_time (OUT) - Time, in seconds, since last CP reboot.
  @return CPI_OK for success, otherwise CPI_ error code

 */

int cpi_get_current_time(struct _cpi_t *p_cpi, uint32_t *p_cur_time);

/*!

 Retrieve the current owner key index.

  @param p_cpi (INP) - CPI instance
  @param p_oki (OUT) - Current owner key index.
  @return CPI_OK for success, otherwise CPI_ error code

 */

int cpi_get_owner_key_index(struct _cpi_t *p_cpi, uint32_t *p_oki);

/*!

 Get the serial number of the CP.

  @param p_cpi (INP) - CPI instance
  @param raw_serial (OUT) - Buffer at least CPI_SERIAL_NUMBER_SIZE bytes in size.
  @return CPI_OK for success, otherwise CPI_ error code

 */

int cpi_get_serial_number(struct _cpi_t *p_cpi, uint8_t *raw_serial);

/*!

 Get the hardware version data of the attached core unit.

  @param p_cpi (INP) - CPI instance
  @param raw_hard_vers (OUT) - Buffer at least CPI_HARDWARE_VERSION_SIZE bytes in size.
  @return CPI_OK for success, otherwise CPI_ error code

 */

int cpi_get_hardware_version_data(struct _cpi_t *p_cpi, uint8_t *raw_hard_vers);

/*!

 Issue a challenge to the CP.

  @param p_cpi (INP) - CPI instance
  @param cpi_key_id (INP) - CPI Key ID
  @param rand_data (INP) - Raw random binary data CPI_RNDX_SIZE bytes in size.
  @param result1 (OUT) - Buffer at least CPI_RESULT1_SIZE bytes in size, which holds result 1.
  @param result2 (OUT) - Buffer at least CPI_RESULT2_SIZE bytes in size, which holds result 2.
  @return CPI_OK for success, otherwise CPI_ error code

 */

  int cpi_issue_challenge(struct _cpi_t *p_cpi, uint16_t cpi_key_id, uint8_t *rand_data, uint8_t *result1, uint8_t *result2, uint8_t *result3);

/*! 

  @brief CPI instance

  This structure represents an instance of the CPI.

*/

typedef struct _cpi_t
{
    /*! serial device file handle */
    int serial_file;
    /*! initialization flag */
    int is_initialized;
    /*! temporary buffer */
    char *tmp_buff; 
    /*! temporary buffer, for XML parsing */
    char *tmp_buff_xml;
}
cpi_t;

/*! 

  @brief CPI information 

  This structure should be passed to cpi_create() in order to initialize
  a CPI instance. 

*/

typedef struct _cpi_info_t
{
    int dummy; /*!< temporary placeholder */
}
cpi_info_t;

/*! \name CPI sizes, in bytes */
/*! \{ */
#define CPI_MAX_RESULT_SIZE         0x1000  /*!< 4096 bytes, @todo finalize this max */
#define CPI_PUTATIVE_ID_SIZE        0x0025  /*!< 37 bytes, includes null terminator */
#define CPI_VERSION_SIZE            0x0006  /*!< 6 bytes, raw binary data */
#define CPI_SERIAL_NUMBER_SIZE      0x0010  /*!< 16 bytes, raw binary data */
#define CPI_HARDWARE_VERSION_SIZE   0x0010  /*!< 16 bytes, raw binary data */
#define CPI_HPIDX_SIZE              0x0014  /*!< 20 bytes, raw binary data */
#define CPI_RNDX_SIZE               0x0010  /*!< 16 bytes, raw binary data */
#define CPI_RESULT1_SIZE            0x0114  /*!< 276 bytes, raw binary data, holds encrypted owner key, "Rm", and 4 byte version data */
#define CPI_RESULT1_ENC_OK_SIZE     0x0100  /*!< 256 bytes, raw binary data, holds encrypted owner key result */
#define CPI_RESULT1_RAND_SIZE       0x0010  /*!< 16 bytes, raw binary data, holds "Rm" random data */
#define CPI_RESULT1_VERS_SIZE       0x0004  /*!< 4 bytes, raw binary data, holds 4 byte version data */
#define CPI_RESULT2_SIZE            0x0080  /*!< 128 bytes, raw binary data, holds signature result */
#define CPI_RESULT3_SIZE            (416)   /*!< 416 bytes, raw binary data, holds hash result, rounded to nearest block length */
/*! \} */

/*! \name CPI return codes */
/*! \{ */
#define CPI_OK                  0x0000  /*!< Success! */
#define CPI_FAIL                0x0001  /*!< Generic failure */
#define CPI_NOTIMPL             0x0002  /*!< Functionality not implemented */
#define CPI_INVALID_PARAM       0x0003  /*!< Invalid parameter */
#define CPI_OUT_OF_MEMORY       0x0004  /*!< Out of memory */
#define CPI_ACCESS_DENIED       0x0005  /*!< Access denied */
#define CPI_INVALID_CALL        0x0006  /*!< Invalid call */
/*! \} */

/*! \name CPI return code lookup table, for convienence */
/*! \{ */
char *CPI_RETURN_CODE_LOOKUP[0x07];
/*! \} */

/*! \name CPI return code helper functions */
/*! \{ */
/*! Detect success error code */
#define CPI_SUCCESS(x) (x == CPI_OK)
/*! Detect failure error code */
#define CPI_FAILED(x) (x != CPI_OK)
/*! \} */

#ifdef __cplusplus
}
#endif

#endif
