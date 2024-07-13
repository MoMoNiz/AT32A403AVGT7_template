/**
 * CANopen Object Dictionary interface
 *
 * @file        CO_ODinterface.h
 * @ingroup     CO_ODinterface
 * @author      Janez Paternoster
 * @copyright   2020 Janez Paternoster
 *
 * This file is part of CANopenNode, an opensource CANopen Stack.
 * Project home page is <https://github.com/CANopenNode/CANopenNode>.
 * For more information on CANopen see <http://www.can-cia.org/>.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef CO_OD_INTERFACE_H
#define CO_OD_INTERFACE_H

#include "301/CO_driver.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup CO_ODinterface OD interface
 * CANopen Object Dictionary interface.
 *
 * @ingroup CO_CANopen_301
 * @{
 * See @ref doc/objectDictionary.md
 */

#ifndef CO_OD_OWN_TYPES
/** OD_size_t 类型的变量包含以字节为单位的 OD 变量数据长度 */
typedef uint32_t OD_size_t;
/** Type (and size) of Object Dictionary attribute */
typedef uint8_t OD_attr_t;
#endif

#ifndef OD_FLAGS_PDO_SIZE
/** @ref OD_extension_t 中 flagsPDO 变量的大小，从 0 到 32。 */
#define OD_FLAGS_PDO_SIZE 4
#endif

#ifndef CO_PROGMEM
/** OD 对象的修改器。这是大量数据，通常在对象字典（OD.c 文件）中指定。 */
#define CO_PROGMEM const
#endif


/**
 * 常见的 DS301 对象字典条目。
 */
typedef enum {
    OD_H1000_DEV_TYPE           = 0x1000U,/**< Device type */
    OD_H1001_ERR_REG            = 0x1001U,/**< Error register */
    OD_H1002_MANUF_STATUS_REG   = 0x1002U,/**< Manufacturer status register */
    OD_H1003_PREDEF_ERR_FIELD   = 0x1003U,/**< Predefined error field */
    OD_H1004_RSV                = 0x1004U,/**< Reserved */
    OD_H1005_COBID_SYNC         = 0x1005U,/**< Sync message cob-id */
    OD_H1006_COMM_CYCL_PERIOD   = 0x1006U,/**< Communication cycle period */
    OD_H1007_SYNC_WINDOW_LEN    = 0x1007U,/**< Sync windows length */
    OD_H1008_MANUF_DEV_NAME     = 0x1008U,/**< Manufacturer device name */
    OD_H1009_MANUF_HW_VERSION   = 0x1009U,/**< Manufacturer hardware version */
    OD_H100A_MANUF_SW_VERSION   = 0x100AU,/**< Manufacturer software version */
    OD_H100B_RSV                = 0x100BU,/**< Reserved */
    OD_H100C_GUARD_TIME         = 0x100CU,/**< Guard time */
    OD_H100D_LIFETIME_FACTOR    = 0x100DU,/**< Life time factor */
    OD_H100E_RSV                = 0x100EU,/**< Reserved */
    OD_H100F_RSV                = 0x100FU,/**< Reserved */
    OD_H1010_STORE_PARAMETERS   = 0x1010U,/**< Store params in persistent mem.*/
    OD_H1011_RESTORE_DEFAULT    = 0x1011U,/**< Restore default parameters */
    OD_H1012_COBID_TIME         = 0x1012U,/**< Timestamp message cob-id */
    OD_H1013_HIGH_RES_TIMESTAMP = 0x1013U,/**< High resolution timestamp */
    OD_H1014_COBID_EMERGENCY    = 0x1014U,/**< Emergency message cob-id */
    OD_H1015_INHIBIT_TIME_EMCY  = 0x1015U,/**< Inhibit time emergency message */
    OD_H1016_CONSUMER_HB_TIME   = 0x1016U,/**< Consumer heartbeat time */
    OD_H1017_PRODUCER_HB_TIME   = 0x1017U,/**< Producer heartbeat time */
    OD_H1018_IDENTITY_OBJECT    = 0x1018U,/**< Identity object */
    OD_H1019_SYNC_CNT_OVERFLOW  = 0x1019U,/**< Sync counter overflow value */
    OD_H1020_VERIFY_CONFIG      = 0x1020U,/**< Verify configuration */
    OD_H1021_STORE_EDS          = 0x1021U,/**< Store EDS */
    OD_H1022_STORE_FORMAT       = 0x1022U,/**< Store format */
    OD_H1023_OS_CMD             = 0x1023U,/**< OS command */
    OD_H1024_OS_CMD_MODE        = 0x1024U,/**< OS command mode */
    OD_H1025_OS_DBG_INTERFACE   = 0x1025U,/**< OS debug interface */
    OD_H1026_OS_PROMPT          = 0x1026U,/**< OS prompt */
    OD_H1027_MODULE_LIST        = 0x1027U,/**< Module list */
    OD_H1028_EMCY_CONSUMER      = 0x1028U,/**< Emergency consumer object */
    OD_H1029_ERR_BEHAVIOR       = 0x1029U,/**< Error behaviour */
    OD_H1200_SDO_SERVER_1_PARAM = 0x1200U,/**< SDO server parameter */
    OD_H1280_SDO_CLIENT_1_PARAM = 0x1280U,/**< SDO client parameter */
    OD_H1300_GFC_PARAM          = 0x1300U,/**< Global fail-safe command param */
    OD_H1301_SRDO_1_PARAM       = 0x1301U,/**< SRDO communication parameter */
    OD_H1381_SRDO_1_MAPPING     = 0x1381U,/**< SRDO mapping parameter */
    OD_H13FE_SRDO_VALID         = 0x13FEU,/**< SRDO Configuration valid */
    OD_H13FF_SRDO_CHECKSUM      = 0x13FFU,/**< SRDO configuration checksum */
    OD_H1400_RXPDO_1_PARAM      = 0x1400U,/**< RXPDO communication parameter */
    OD_H1600_RXPDO_1_MAPPING    = 0x1600U,/**< RXPDO mapping parameters */
    OD_H1800_TXPDO_1_PARAM      = 0x1800U,/**< TXPDO communication parameter */
    OD_H1A00_TXPDO_1_MAPPING    = 0x1A00U,/**< TXPDO mapping parameters */
} OD_ObjDicId_30x_t;


/**
 * OD 子对象的属性（位掩码）。
 */
typedef enum {
    ODA_SDO_R = 0x01, /**< SDO server may read from the variable */
    ODA_SDO_W = 0x02, /**< SDO server may write to the variable */
    ODA_SDO_RW = 0x03, /**< SDO server may read from or write to the variable */
    ODA_TPDO = 0x04, /**< Variable is mappable into TPDO (can be read) */
    ODA_RPDO = 0x08, /**< Variable is mappable into RPDO (can be written) */
    ODA_TRPDO = 0x0C, /**< Variable is mappable into TPDO or RPDO */
    ODA_TSRDO = 0x10, /**< Variable is mappable into transmitting SRDO */
    ODA_RSRDO = 0x20, /**< Variable is mappable into receiving SRDO */
    ODA_TRSRDO = 0x30, /**< Variable is mappable into tx or rx SRDO */
    ODA_MB = 0x40, /**< Variable is multi-byte ((u)int16_t to (u)int64_t) */
    ODA_STR = 0x80 /**< Shorter value, than specified variable size, may be
    written to the variable. SDO write will fill remaining memory with zeroes.
    Attribute is used for VISIBLE_STRING and UNICODE_STRING. */
} OD_attributes_t;


/**
 * OD 访问功能的返回代码。
 *
 * @ref OD_getSDOabCode() 可用于获取相应的 SDO 终止代码。
 */
typedef enum {
/* !!!! WARNING !!!!
 * 如果更改这些值，还需更改 OD_getSDOabCode() 函数！
 */
    /** Read/write is only partial, make more calls */
    ODR_PARTIAL = -1,
    /** SDO abort 0x00000000 - Read/write successfully finished */
    ODR_OK = 0,
    /** SDO abort 0x05040005 - Out of memory */
    ODR_OUT_OF_MEM = 1,
    /** SDO abort 0x06010000 - Unsupported access to an object */
    ODR_UNSUPP_ACCESS = 2,
    /** SDO abort 0x06010001 - Attempt to read a write only object */
    ODR_WRITEONLY = 3,
    /** SDO abort 0x06010002 - Attempt to write a read only object */
    ODR_READONLY = 4,
    /** SDO abort 0x06020000 - Object does not exist in the object dict. */
    ODR_IDX_NOT_EXIST = 5,
    /** SDO abort 0x06040041 - Object cannot be mapped to the PDO */
    ODR_NO_MAP = 6,
    /** SDO abort 0x06040042 - PDO length exceeded */
    ODR_MAP_LEN = 7,
    /** SDO abort 0x06040043 - General parameter incompatibility reasons */
    ODR_PAR_INCOMPAT = 8,
    /** SDO abort 0x06040047 - General internal incompatibility in device */
    ODR_DEV_INCOMPAT = 9,
    /** SDO abort 0x06060000 - Access failed due to hardware error */
    ODR_HW = 10,
    /** SDO abort 0x06070010 - Data type does not match */
    ODR_TYPE_MISMATCH = 11,
    /** SDO abort 0x06070012 - Data type does not match, length too high */
    ODR_DATA_LONG = 12,
    /** SDO abort 0x06070013 - Data type does not match, length too short */
    ODR_DATA_SHORT = 13,
    /** SDO abort 0x06090011 - Sub index does not exist */
    ODR_SUB_NOT_EXIST = 14,
    /** SDO abort 0x06090030 - Invalid value for parameter (download only) */
    ODR_INVALID_VALUE = 15,
    /** SDO abort 0x06090031 - Value range of parameter written too high */
    ODR_VALUE_HIGH = 16,
    /** SDO abort 0x06090032 - Value range of parameter written too low */
    ODR_VALUE_LOW = 17,
    /** SDO abort 0x06090036 - Maximum value is less than minimum value */
    ODR_MAX_LESS_MIN = 18,
    /** SDO abort 0x060A0023 - Resource not available: SDO connection */
    ODR_NO_RESOURCE = 19,
    /** SDO abort 0x08000000 - General error */
    ODR_GENERAL = 20,
    /** SDO abort 0x08000020 - Data cannot be transferred or stored to app */
    ODR_DATA_TRANSF = 21,
    /** SDO abort 0x08000021 - Data can't be transferred (local control) */
    ODR_DATA_LOC_CTRL = 22,
    /** SDO abort 0x08000022 - Data can't be transf. (present device state) */
    ODR_DATA_DEV_STATE = 23,
    /** SDO abort 0x08000023 - Object dictionary not present */
    ODR_OD_MISSING = 24,
    /** SDO abort 0x08000024 - No data available */
    ODR_NO_DATA = 25,
    /** Last element, number of responses */
    ODR_COUNT = 26
} ODR_t;


/**
 * IO 流结构，用于读/写访问 OD 变量 , @ref OD_IO_t 的一部分
 */
typedef struct {
    /** Pointer to original data object, defined by Object Dictionary. Default
     * read/write functions operate on it. If memory for data object is not
     * specified by Object Dictionary, then dataOrig is NULL.
     */
    void *dataOrig;
    /** Pointer to object, passed by @ref OD_extension_init(). Can be used
     * inside read / write functions from IO extension.
     */
    void *object;
    /** Data length in bytes or 0, if length is not specified */
    OD_size_t dataLength;
    /** In case of large data, dataOffset indicates position of already
     * transferred data */
    OD_size_t dataOffset;
    /** Attribute bit-field of the OD sub-object, see @ref OD_attributes_t */
    OD_attr_t attribute;
    /** Index of the OD object, informative */
    uint16_t index;
    /** Sub index of the OD sub-object, informative */
    uint8_t subIndex;
} OD_stream_t;


/**
 * 用于输入/输出 OD 变量的结构。它通过 @ref OD_getSub() 函数初始化。
 * 访问 OD 变量的原理是通过对流操作的读/写函数，类似于标准读/写。
 */
typedef struct {
    /** Object Dictionary stream object, passed to read or write */
    OD_stream_t stream;
    /**
     * Function pointer for reading value from specified variable from Object
     * Dictionary. If OD variable is larger than buf, then this function must
     * be called several times. After completed successful read function returns
     * 'ODR_OK'. If read is partial, it returns 'ODR_PARTIAL'. In case of errors
     * function returns code similar to SDO abort code.
     *
     * Read can be restarted with @ref OD_rwRestart() function.
     *
     * At the moment, when Object Dictionary is initialized, every variable has
     * assigned the same "read" function. This default function simply copies
     * data from Object Dictionary variable. Application can bind its own "read"
     * function for specific object. In that case application is able to
     * calculate data for reading from own internal state at the moment of
     * "read" function call. Own "read" function on OD object can be initialized
     * with @ref OD_extension_init() function.
     *
     * "read" function must always copy all own data to buf, except if "buf" is
     * not large enough. ("*returnCode" must not return 'ODR_PARTIAL', if there
     * is still space in "buf".)
     *
     * @warning Do not use @ref CO_LOCK_OD() and @ref CO_UNLOCK_OD() macros
     * inside the read() function. See also @ref CO_critical_sections.
     *
     * @param stream Object Dictionary stream object.
     * @param buf Pointer to external buffer, where to data will be copied.
     * @param count Size of the external buffer in bytes.
     * @param [out] countRead If return value is "ODR_OK" or "ODR_PARTIAL",
     * then number of bytes successfully read must be returned here.
     *
     * @return Value from @ref ODR_t, "ODR_OK" in case of success.
     */
    ODR_t (*read)(OD_stream_t *stream, void *buf,
                  OD_size_t count, OD_size_t *countRead);
    /**
     * Function pointer for writing value into specified variable inside Object
     * Dictionary. If OD variable is larger than buf, then this function must
     * be called several times. After completed successful write function
     * returns 'ODR_OK'. If write is partial, it returns 'ODR_PARTIAL'. In case
     * of errors function returns code similar to SDO abort code.
     *
     * Write can be restarted with @ref OD_rwRestart() function.
     *
     * At the moment, when Object Dictionary is initialized, every variable has
     * assigned the same "write" function, which simply copies data to Object
     * Dictionary variable. Application can bind its own "write" function,
     * similar as it can bind "read" function.
     *
     * "write" function must always copy all available data from buf. If OD
     * variable expect more data, then "*returnCode" must return 'ODR_PARTIAL'.
     *
     * @warning Do not use @ref CO_LOCK_OD() and @ref CO_UNLOCK_OD() macros
     * inside the write() function. See also @ref CO_critical_sections.
     *
     * @param stream Object Dictionary stream object.
     * @param buf Pointer to external buffer, from where data will be copied.
     * @param count Size of the external buffer in bytes.
     * @param [out] countWritten If return value is "ODR_OK" or "ODR_PARTIAL",
     * then number of bytes successfully written must be returned here.
     *
     * @return Value from @ref ODR_t, "ODR_OK" in case of success.
     */
    ODR_t (*write)(OD_stream_t *stream, const void *buf,
                   OD_size_t count, OD_size_t *countWritten);
} OD_IO_t;


/**
 * OD 对象的扩展名，可由应用程序在初始化阶段
 * 使用 @ref OD_extension_init() 函数指定。
 */
typedef struct {
    /** 读写操作对象，@ref OD_stream_t 的一部分 */
    void *object;
    /** 应用程序指定的读取功能指针。如果为空，则读取功能将被禁用。
     * @ref OD_readOriginal 可用于保留原始读取函数。有关函数说明，请参阅 @ref OD_IO_t。*/
    ODR_t (*read)(OD_stream_t *stream, void *buf,
                  OD_size_t count, OD_size_t *countRead);
    /** 应用程序指定的写入函数指针。如果为空，则将禁用写入功能。
     * @ref OD_writeOriginal 可用于保留原始写入函数。有关函数说明，请参阅 @ref OD_IO_t。*/
    ODR_t (*write)(OD_stream_t *stream, const void *buf,
                   OD_size_t count, OD_size_t *countWritten);
#if OD_FLAGS_PDO_SIZE > 0
    /**PDO 标志位字段为每个 OD 变量提供一个位，这些变量存在于 OD 对象的特定子索引中。
     * 如果应用程序清除了该位，且 OD 变量被映射到事件驱动的 TPDO，则将发送 TPDO。
     *
     * @ref OD_FLAGS_PDO_SIZE 的值范围为 0 至 32 字节，相当于 0 至 256 个可用位。
     * 例如，如果 @ref OD_FLAGS_PDO_SIZE 的值为 4，那么子索引不超过 31 的 OD 变量将具有 TPDO 请求功能。
     * 另请参阅 @ref OD_requestTPDO 和 @ref OD_TPDOtransmitted。 */
    uint8_t flagsPDO[OD_FLAGS_PDO_SIZE];
#endif
} OD_extension_t;


/**
 * 一个 OD 对象的对象字典条目。
 *
 * OD 条目以数组（列表）形式收集在 OD_t 中。
 * 每个 OD 条目都包含有关 OD 对象的基本信息（索引和子条目数）、
 * 指向 odObject 的指针（包含有关 var、数组或记录条目的附加信息）以及指向扩展名的指针（可由应用程序配置）。
 */
typedef struct {
    /** 对象字典索引 */
    uint16_t index;
    /** 所有子条目数，包括子索引 0 的子条目数 */
    uint8_t subEntriesCount;
    /** odObject 的类型，由 @ref OD_objectTypes_t 枚举器表示。*/
    uint8_t odObjectType;
    /** OD 对象的类型由 odObjectType 表示，@ref OD_getSub()从中获取信息。*/
    CO_PROGMEM void *odObject;
    /** Extension to OD, specified by application */
    OD_extension_t *extension;
} OD_entry_t;


/**
 * 对象字典
 */
typedef struct {
    /** 列表中的元素数，不包括最后一个元素（空白）。 */
    uint16_t size;
    /** OD 条目列表（目录），按索引排序 */
    OD_entry_t *list;
} OD_t;


/**
 * 从原始 OD 位置读取数值
 *
 * 该函数可在 @ref OD_extension_init() 指定的读/写函数中使用。
 * 它直接从对象字典指定的内存位置读取数据。
 * 如果没有在 OD 条目上使用 IO 扩展，则 @ref OD_getSub() 返回的 io->read 与此函数等价。
 * 另请参阅 @ref OD_IO_t。
 */
ODR_t OD_readOriginal(OD_stream_t *stream, void *buf,
                      OD_size_t count, OD_size_t *countRead);


/**
 * 将数值写入原始 OD 位置
 *
 * 该函数可在 @ref OD_extension_init() 指定的读/写函数中使用。
 * 它直接从对象字典指定的内存位置读取数据。
 * 如果没有在 OD 条目上使用 IO 扩展，则 @ref OD_getSub() 返回的 io->read 与此函数等价。
 * 另请参阅 @ref OD_IO_t。
 */
ODR_t OD_writeOriginal(OD_stream_t *stream, const void *buf,
                       OD_size_t count, OD_size_t *countWritten);


/**
 * 在对象字典中查找 OD 条目
 *
 * @param od 对象字典
 * @param index CANopen 对象字典中对象的索引
 *
 * @return OD 条目的指针，如果未找到，则为 NULL
 */
OD_entry_t *OD_find(OD_t *od, uint16_t index);


/**
 * 在 OD_find 返回的 OD 条目中查找具有指定子索引的子对象。
 * 函数用子对象数据填充 io 结构。
 *
 * @warning
 * 读取和写入函数可能会被不同的线程调用，
 * 因此必须遵守自定义函数中的关键部分，参见 @ref CO_critical_sections。
 *
 * @param entry 由 @ref OD_find() 返回的 OD 条目。
 * @param subIndex OD 对象中变量的子索引。
 * @param [out] io 成功后将填充结构。
 * @param odOrig 如果为 "true"，则将忽略输入时的潜在 IO 扩展，
 * 并返回原始 OD 位置的数据输入权限
 *
 * @return Value from @ref ODR_t, "ODR_OK" in case of success.
 */
ODR_t OD_getSub(const OD_entry_t *entry, uint8_t subIndex,
                OD_IO_t *io, bool_t odOrig);


/**
 * 从 OD 条目返回索引
 *
 * @param entry OD entry returned by @ref OD_find().
 *
 * @return OD index
 */
static inline uint16_t OD_getIndex(const OD_entry_t *entry) {
    return (entry != NULL) ? entry->index : 0;
}


/**
 * 检查 OD 变量是否可映射到 PDO 或 SRDO。
 *
 * 如果 OD 变量是可映射的，则可能需要保护主线函数的读/写访问。
 * 参见 @ref CO_critical_sections。
 *
 * @param stream Object Dictionary stream object.
 *
 * @return true, if OD variable is mappable.
 */
static inline bool_t OD_mappable(OD_stream_t *stream) {
    return (stream != NULL)
         ? (stream->attribute & (ODA_TRPDO | ODA_TRSRDO)) != 0 : false;
}


/**
 * 重启对 OD 变量的读取或写入操作
 *
 * 如果数据流已由 @ref OD_getSub() 初始化，则无需调用此函数。
 * 如果之前的读取或写入已成功完成，也无需调用此函数。
 *
 * @param stream Object Dictionary stream object.
 */
static inline void OD_rwRestart(OD_stream_t *stream) {
    if (stream != NULL) { stream->dataOffset = 0; }
}


/**
 * 获取 OD 条目的 TPDO 请求标志。
 *
 * flagsPDO 可用于 @ref OD_requestTPDO() 或 @ref OD_TPDOtransmitted()。
 *
 * @param entry 由 @ref OD_find() 返回的 OD 条目。
 *
 * @return 指向 flagsPDO 的指针
 */
static inline uint8_t *OD_getFlagsPDO(OD_entry_t *entry) {
#if OD_FLAGS_PDO_SIZE > 0
    if (entry != NULL && entry->extension != NULL) {
        return &entry->extension->flagsPDO[0];
    }
#endif
    return 0;
}


/**
 * 请求 TPDO，OD 变量映射到该 TPDO
 *
 * 该函数清除与特定 OD 索引和子索引上的 OD 变量相对应的 flagPDO 位。
 * 要使该功能生效，必须在 OD 变量上启用 @ref OD_extension_t。
 * 如果 OD 变量被映射到任何具有事件驱动传输功能的 TPDO，那么 TPDO 将在此函数调用后被传输。
 * 如果 OD 变量被映射到多个具有事件驱动传输功能的 TPDO，则只会传输第一个匹配的 TPDO。
 *
 * 如果 TPDO 通信参数（传输类型）设置为 0、254 或 255，则会启用 TPDO 事件驱动传输。
 * 对于其他传输类型（同步），flagPDO 位将被忽略。
 *
 * @param flagsPDO 由 @ref OD_getFlagsPDO 返回的 TPDO 请求标志。
 * @param subIndex OD 变量的子索引。
 */
static inline void OD_requestTPDO(uint8_t *flagsPDO, uint8_t subIndex) {
#if OD_FLAGS_PDO_SIZE > 0
    if (flagsPDO != NULL && subIndex < (OD_FLAGS_PDO_SIZE * 8)) {
        /* clear subIndex-th bit */
        uint8_t mask = ~(1 << (subIndex & 0x07));
        flagsPDO[subIndex >> 3] &= mask;
    }
#endif
}


/**
 * 检查所请求的 TPDO 是否已传输
 *
 * @param flagsPDO 由 @ref OD_getFlagsPDO 返回的 TPDO 请求标志。
 * @param subIndex OD 变量的子索引。
 *
 * @return 如果事件驱动的 TPDO（由 flagsPDO 和 subIndex 表示）与 OD 变量有映射关系，
 * 且自上次传输后传输过，则返回 true。
 * @ref OD_requestTPDO 调用。
 * 如果还没有 @ref OD_requestTPDO调用，且 TPDO 被其他事件传送，函数也返回 true。
 */
static inline bool_t OD_TPDOtransmitted(uint8_t *flagsPDO, uint8_t subIndex) {
#if OD_FLAGS_PDO_SIZE > 0
    if (flagsPDO != NULL && subIndex < (OD_FLAGS_PDO_SIZE * 8)) {
        /* return true, if subIndex-th bit is set */
        uint8_t mask = 1 << (subIndex & 0x07);
        if ((flagsPDO[subIndex >> 3] & mask) != 0) {
            return true;
        }
    }
#endif
    return false;
}


/**
 * 从 returnCode 获取 SDO 终止代码
 *
 * @param returnCode Returned from some OD access functions
 *
 * @return Corresponding @ref CO_SDO_abortCode_t
 */
uint32_t OD_getSDOabCode(ODR_t returnCode);


/**
 * 用自己的读/写功能和/或标志扩展 OD 对象PDO
 *
 * 该函数为应用程序提供了非常强大的工具：
 * 在 OD 对象上定义自己的 IO 访问。
 * 结构和属性与原始 OD 对象中定义的相同，
 * 但数据是通过自定义函数调用直接从应用程序指定的对象读取（或直接写入）的。
 *
 * 在此函数指定扩展之前，OD 变量从原始 OD 位置访问。
 * 在此函数指定扩展后，OD 变量通过扩展指定的读/写函数访问。
 * (当 @ref OD_getSub() 的 "odOrig "参数设置为 true 时除外）。
 *
 * 当需要为特定条目启用 flagsPDO 时，也必须使用该函数。
 *
 * @warning
 * 对象字典存储只直接作用于 OD 变量。
 * 它不能访问此处指定的读取功能。
 * 因此，如果需要保存扩展的 OD 对象，可以在自定义写入函数中使用 @ref OD_writeOriginal。
 *
 * @warning
 * 读取和写入函数可能会被不同的线程调用，
 * 因此必须遵守自定义函数中的关键部分，参见 @ref CO_critical_sections。
 *
 * @param entry 由 @ref OD_find() 返回的 OD 条目。
 * @param extension 扩展对象，必须从外部初始化。
 * 扩展对象必须永久存在。如果为空，扩展将被删除。
 *
 * @return 成功时显示 "ODR_OK"，如果 OD 对象不存在，则显示 "ODR_IDX_NOT_EXIST"。
 */
static inline ODR_t OD_extension_init(OD_entry_t *entry,
                                      OD_extension_t *extension)
{
    if (entry == NULL) { return ODR_IDX_NOT_EXIST; }
    entry->extension = extension;
    return ODR_OK;
}


/**
 * @defgroup CO_ODgetSetters 获取器和设置器
 * @{
 *
 * 用于访问不同类型对象字典变量的 Getter 和 setter 辅助函数。
 */
/**
 * 从对象字典中获取变量
 *
 * @param entry 由 @ref OD_find() 返回的 OD 条目。
 * @param subIndex OD 对象中变量的子索引。
 * @param [out] val 值将写在这里。
 * @param len 要从 OD 获取的值的大小。
 * @param odOrig 如果为 "true"，则将忽略输入时的潜在 IO 扩展，并返回原始 OD 位置中的数据。
 *
 * @return 来自 @ref ODR_t 的值，成功时为 "ODR_OK"。
 * 如果对象字典中不存在变量或变量长度不正确或其他原因，则出错。
 */
ODR_t OD_get_value(const OD_entry_t *entry, uint8_t subIndex,
                   void *val, OD_size_t len, bool_t odOrig);

/** Get int8_t variable from Object Dictionary, see @ref OD_get_value */
static inline ODR_t OD_get_i8(const OD_entry_t *entry, uint8_t subIndex,
                              int8_t *val, bool_t odOrig)
{
    return OD_get_value(entry, subIndex, val, sizeof(*val), odOrig);
}

/** Get int16_t variable from Object Dictionary, see @ref OD_get_value */
static inline ODR_t OD_get_i16(const OD_entry_t *entry, uint8_t subIndex,
                               int16_t *val, bool_t odOrig)
{
    return OD_get_value(entry, subIndex, val, sizeof(*val), odOrig);
}

/** Get int32_t variable from Object Dictionary, see @ref OD_get_value */
static inline ODR_t OD_get_i32(const OD_entry_t *entry, uint8_t subIndex,
                               int32_t *val, bool_t odOrig)
{
    return OD_get_value(entry, subIndex, val, sizeof(*val), odOrig);
}

/** Get int64_t variable from Object Dictionary, see @ref OD_get_value */
static inline ODR_t OD_get_i64(const OD_entry_t *entry, uint8_t subIndex,
                               int64_t *val, bool_t odOrig)
{
    return OD_get_value(entry, subIndex, val, sizeof(*val), odOrig);
}

/** Get uint8_t variable from Object Dictionary, see @ref OD_get_value */
static inline ODR_t OD_get_u8(const OD_entry_t *entry, uint8_t subIndex,
                              uint8_t *val, bool_t odOrig)
{
    return OD_get_value(entry, subIndex, val, sizeof(*val), odOrig);
}

/** Get uint16_t variable from Object Dictionary, see @ref OD_get_value */
static inline ODR_t OD_get_u16(const OD_entry_t *entry, uint8_t subIndex,
                               uint16_t *val, bool_t odOrig)
{
    return OD_get_value(entry, subIndex, val, sizeof(*val), odOrig);
}

/** Get uint32_t variable from Object Dictionary, see @ref OD_get_value */
static inline ODR_t OD_get_u32(const OD_entry_t *entry, uint8_t subIndex,
                               uint32_t *val, bool_t odOrig)
{
    return OD_get_value(entry, subIndex, val, sizeof(*val), odOrig);
}

/** Get uint64_t variable from Object Dictionary, see @ref OD_get_value */
static inline ODR_t OD_get_u64(const OD_entry_t *entry, uint8_t subIndex,
                               uint64_t *val, bool_t odOrig)
{
    return OD_get_value(entry, subIndex, val, sizeof(*val), odOrig);
}

/** Get float32_t variable from Object Dictionary, see @ref OD_get_value */
static inline ODR_t OD_get_f32(const OD_entry_t *entry, uint8_t subIndex,
                               float32_t *val, bool_t odOrig)
{
    return OD_get_value(entry, subIndex, val, sizeof(*val), odOrig);
}

/** Get float64_t variable from Object Dictionary, see @ref OD_get_value */
static inline ODR_t OD_get_f64(const OD_entry_t *entry, uint8_t subIndex,
                               float64_t *val, bool_t odOrig)
{
    return OD_get_value(entry, subIndex, val, sizeof(*val), odOrig);
}

/**
 * 在对象字典中设置变量
 *
 * @param entry OD entry returned by @ref OD_find().
 * @param subIndex Sub-index of the variable from the OD object.
 * @param val Pointer to value to write.
 * @param len Size of value to write.
 * @param odOrig If true, then potential IO extension on entry will be
 * ignored and data in the original OD location will be written.
 *
 * @return Value from @ref ODR_t, "ODR_OK" in case of success. Error, if
 * variable does not exist in object dictionary or it does not have the correct
 * length or other reason.
 */
ODR_t OD_set_value(const OD_entry_t *entry, uint8_t subIndex, void *val,
                   OD_size_t len, bool_t odOrig);

/** Set int8_t variable in Object Dictionary, see @ref OD_set_value */
static inline ODR_t OD_set_i8(const OD_entry_t *entry, uint8_t subIndex,
                              int8_t val, bool_t odOrig)
{
    return OD_set_value(entry, subIndex, &val, sizeof(val), odOrig);
}

/** Set int16_t variable in Object Dictionary, see @ref OD_set_value */
static inline ODR_t OD_set_i16(const OD_entry_t *entry, uint8_t subIndex,
                               int16_t val, bool_t odOrig)
{
    return OD_set_value(entry, subIndex, &val, sizeof(val), odOrig);
}

/** Set int32_t variable in Object Dictionary, see @ref OD_set_value */
static inline ODR_t OD_set_i32(const OD_entry_t *entry, uint8_t subIndex,
                               int32_t val, bool_t odOrig)
{
    return OD_set_value(entry, subIndex, &val, sizeof(val), odOrig);
}

/** Set int32_t variable in Object Dictionary, see @ref OD_set_value */
static inline ODR_t OD_set_i64(const OD_entry_t *entry, uint8_t subIndex,
                               int64_t val, bool_t odOrig)
{
    return OD_set_value(entry, subIndex, &val, sizeof(val), odOrig);
}

/** Set uint8_t variable in Object Dictionary, see @ref OD_set_value */
static inline ODR_t OD_set_u8(const OD_entry_t *entry, uint8_t subIndex,
                              uint8_t val, bool_t odOrig)
{
    return OD_set_value(entry, subIndex, &val, sizeof(val), odOrig);
}

/** Set uint16_t variable in Object Dictionary, see @ref OD_set_value */
static inline ODR_t OD_set_u16(const OD_entry_t *entry, uint8_t subIndex,
                               uint16_t val, bool_t odOrig)
{
    return OD_set_value(entry, subIndex, &val, sizeof(val), odOrig);
}

/** Set uint32_t variable in Object Dictionary, see @ref OD_set_value */
static inline ODR_t OD_set_u32(const OD_entry_t *entry, uint8_t subIndex,
                               uint32_t val, bool_t odOrig)
{
    return OD_set_value(entry, subIndex, &val, sizeof(val), odOrig);
}

/** Set uint64_t variable in Object Dictionary, see @ref OD_set_value */
static inline ODR_t OD_set_u64(const OD_entry_t *entry, uint8_t subIndex,
                               uint64_t val, bool_t odOrig)
{
    return OD_set_value(entry, subIndex, &val, sizeof(val), odOrig);
}

/** Set float32_t variable in Object Dictionary, see @ref OD_set_value */
static inline ODR_t OD_set_f32(const OD_entry_t *entry, uint8_t subIndex,
                               float32_t val, bool_t odOrig)
{
    return OD_set_value(entry, subIndex, &val, sizeof(val), odOrig);
}

/** Set float64_t variable in Object Dictionary, see @ref OD_set_value */
static inline ODR_t OD_set_f64(const OD_entry_t *entry, uint8_t subIndex,
                               float64_t val, bool_t odOrig)
{
    return OD_set_value(entry, subIndex, &val, sizeof(val), odOrig);
}

/**
 * 从对象字典中获取存放数据变量的内存指针
 *
 * 函数总是返回指向原始 OD 位置数据的 "dataOrig "指针。
 * 如果在 OD 条目上启用了 IO 扩展，则需注意。还要注意 "dataOrig "可能与数据类型不对齐。
 *
 * @param entry OD entry returned by @ref OD_find().
 * @param subIndex Sub-index of the variable from the OD object.
 * @param len Required length of the variable. If len is different than zero,
 * then actual length of the variable must match len or error is returned.
 * @param [out] err Error reason is written here in case of error (allow NULL).
 *
 * @return 对象字典中变量的指针，出错时为 NULL。
 */
void *OD_getPtr(const OD_entry_t *entry, uint8_t subIndex, OD_size_t len,
                ODR_t *err);
/** @} */ /* CO_ODgetSetters */


#if defined OD_DEFINITION || defined CO_DOXYGEN
/**
 * @defgroup CO_ODdefinition OD definition objects
 * @{
 *
 * Types and functions used only for definition of Object Dictionary
 */
/**
 * Types for OD object.
 */
typedef enum {
    /** This type corresponds to CANopen Object Dictionary object with object
     * code equal to VAR. OD object is type of @ref OD_obj_var_t and represents
     * single variable of any type (any length), located on sub-index 0. Other
     * sub-indexes are not used. */
    ODT_VAR = 0x01,
    /** This type corresponds to CANopen Object Dictionary object with object
     * code equal to ARRAY. OD object is type of @ref OD_obj_array_t and
     * represents array of variables with the same type, located on sub-indexes
     * above 0. Sub-index 0 is of type uint8_t and usually represents length of
     * the array. */
    ODT_ARR = 0x02,
    /** This type corresponds to CANopen Object Dictionary object with object
     * code equal to RECORD. This type of OD object represents structure of
     * the variables. Each variable from the structure can have own type and
     * own attribute. OD object is an array of elements of type
     * @ref OD_obj_var_t. Variable at sub-index 0 is of type uint8_t and usually
     * represents number of sub-elements in the structure. */
    ODT_REC = 0x03,
    /** Mask for basic type */
    ODT_TYPE_MASK = 0x0F,
} OD_objectTypes_t;

/**
 * Object for single OD variable, used for "VAR" type OD objects
 */
typedef struct {
    void *dataOrig; /**< Pointer to data */
    OD_attr_t attribute; /**< Attribute bitfield, see @ref OD_attributes_t */
    OD_size_t dataLength; /**< Data length in bytes */
} OD_obj_var_t;

/**
 * Object for OD array of variables, used for "ARRAY" type OD objects
 */
typedef struct {
    uint8_t *dataOrig0; /**< Pointer to data for sub-index 0 */
    void *dataOrig; /**< Pointer to array of data */
    OD_attr_t attribute0; /**< Attribute bitfield for sub-index 0, see
                               @ref OD_attributes_t */
    OD_attr_t attribute; /**< Attribute bitfield for array elements */
    OD_size_t dataElementLength; /**< Data length of array elements in bytes */
    OD_size_t dataElementSizeof; /**< Sizeof one array element in bytes */
} OD_obj_array_t;

/**
 * Object for OD sub-elements, used in "RECORD" type OD objects
 */
typedef struct {
    void *dataOrig; /**< Pointer to data */
    uint8_t subIndex; /**< Sub index of element. */
    OD_attr_t attribute; /**< Attribute bitfield, see @ref OD_attributes_t */
    OD_size_t dataLength; /**< Data length in bytes */
} OD_obj_record_t;

/** @} */ /* CO_ODdefinition */

#endif /* defined OD_DEFINITION */

/** @} */ /* CO_ODinterface */

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* CO_OD_INTERFACE_H */
