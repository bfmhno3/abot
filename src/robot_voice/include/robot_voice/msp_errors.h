#ifndef __MSP_ERRORS_H__
#define __MSP_ERRORS_H__

#define MSP_HTTP_ERROR(x) ((x) + MSP_ERROR_HTTP_BASE)

enum {
  MSP_SUCCESS = 0,
  MSP_ERROR_FAIL = -1,
  MSP_ERROR_EXCEPTION = -2,

  /* General errors 10100(0x2774) */
  MSP_ERROR_GENERAL = 10100,        /* 0x2774 */
  MSP_ERROR_OUT_OF_MEMORY = 10101,  /* 0x2775 */
  MSP_ERROR_FILE_NOT_FOUND = 10102, /* 0x2776 */
  MSP_ERROR_NOT_SUPPORT = 10103,    /* 0x2777 */
  MSP_ERROR_NOT_IMPLEMENT = 10104,  /* 0x2778 */
  MSP_ERROR_ACCESS = 10105,         /* 0x2779 */
  MSP_ERROR_INVALID_PARA = 10106,
  /* 0x277A */ /* ȱ�ٲ��� */
  MSP_ERROR_INVALID_PARA_VALUE = 10107,
  /* 0x277B */                      /* ��Ч����ֵ */
  MSP_ERROR_INVALID_HANDLE = 10108, /* 0x277C */
  MSP_ERROR_INVALID_DATA = 10109,   /* 0x277D */
  MSP_ERROR_NO_LICENSE = 10110,
  /* 0x277E */ /* ������Ȩ���� */
  MSP_ERROR_NOT_INIT = 10111,
  /* 0x277F */                   /* ����δ��ʼ��,������������� */
  MSP_ERROR_NULL_HANDLE = 10112, /* 0x2780 */
  MSP_ERROR_OVERFLOW = 10113,
  /* 0x2781 */ /* ���û���ģ����������(10��), */
               /* ֻ�����ڲ���ʱ��һ���û����в���ע�� */
  MSP_ERROR_TIME_OUT = 10114,
  /* 0x2782 */                 /* ��ʱ */
  MSP_ERROR_OPEN_FILE = 10115, /* 0x2783 */
  MSP_ERROR_NOT_FOUND = 10116,
  /* 0x2784 */                        /* ���ݿ���ģ�Ͳ����� */
  MSP_ERROR_NO_ENOUGH_BUFFER = 10117, /* 0x2785 */
  MSP_ERROR_NO_DATA = 10118,
  /* 0x2786 */                        /* �ӿͻ��˶���Ƶ�������λ�ȡ���ʱ������ */
  MSP_ERROR_NO_MORE_DATA = 10119,     /* 0x2787 */
  MSP_ERROR_NO_RESPONSE_DATA = 10120, /* 0x2788 */
  MSP_ERROR_ALREADY_EXIST = 10121,
  /* 0x2789 */                           /* ���ݿ���ģ���Ѵ��� */
  MSP_ERROR_LOAD_MODULE = 10122,         /* 0x278A */
  MSP_ERROR_BUSY = 10123,                /* 0x278B */
  MSP_ERROR_INVALID_CONFIG = 10124,      /* 0x278C */
  MSP_ERROR_VERSION_CHECK = 10125,       /* 0x278D */
  MSP_ERROR_CANCELED = 10126,            /* 0x278E */
  MSP_ERROR_INVALID_MEDIA_TYPE = 10127,  /* 0x278F */
  MSP_ERROR_CONFIG_INITIALIZE = 10128,   /* 0x2790 */
  MSP_ERROR_CREATE_HANDLE = 10129,       /* 0x2791 */
  MSP_ERROR_CODING_LIB_NOT_LOAD = 10130, /* 0x2792 */
  MSP_ERROR_USER_CANCELLED = 10131,      /* 0x2793 */
  MSP_ERROR_INVALID_OPERATION = 10132,   /* 0x2794 */
  MSP_ERROR_MESSAGE_NOT_COMPLETE = 10133,
  /* 0x2795 */              /* flash */
  MSP_ERROR_NO_EID = 10134, /* 0x2795 */
  MSP_ERROE_OVER_REQ = 10135,
  /* 0x2797 */ /* client Redundancy request */
  MSP_ERROR_USER_ACTIVE_ABORT = 10136,
  /* 0x2798 */                        /* user abort */
  MSP_ERROR_BUSY_GRMBUILDING = 10137, /* 0x2799 */
  MSP_ERROR_BUSY_LEXUPDATING = 10138, /* 0x279A */
  MSP_ERROR_SESSION_RESET = 10139,
  /* 0x279B */ /* msc������ֹ�Ự��׼���ش� */
  MSP_ERROR_BOS_TIMEOUT = 10140,
  /* 0x279C */ /* VADǰ�˵㳬ʱ */
  MSP_ERROR_STREAM_FILTER = 10141,
  /* 0X279D */ /* AIUI��ǰStream������ */

  /* Error codes of network 10200(0x27D8)*/
  MSP_ERROR_NET_GENERAL = 10200, /* 0x27D8 */
  MSP_ERROR_NET_OPENSOCK = 10201,
  /* 0x27D9 */ /* Open socket */
  MSP_ERROR_NET_CONNECTSOCK = 10202,
  /* 0x27DA */ /* Connect socket */
  MSP_ERROR_NET_ACCEPTSOCK = 10203,
  /* 0x27DB */ /* Accept socket */
  MSP_ERROR_NET_SENDSOCK = 10204,
  /* 0x27DC */ /* Send socket data */
  MSP_ERROR_NET_RECVSOCK = 10205,
  /* 0x27DD */ /* Recv socket data */
  MSP_ERROR_NET_INVALIDSOCK = 10206,
  /* 0x27DE */ /* Invalid socket handle */
  MSP_ERROR_NET_BADADDRESS = 10207,
  /* 0x27EF */ /* Bad network address */
  MSP_ERROR_NET_BINDSEQUENCE = 10208,
  /* 0x27E0 */ /* Bind after listen/connect */
  MSP_ERROR_NET_NOTOPENSOCK = 10209,
  /* 0x27E1 */ /* Socket is not opened */
  MSP_ERROR_NET_NOTBIND = 10210,
  /* 0x27E2 */ /* Socket is not bind to an address */
  MSP_ERROR_NET_NOTLISTEN = 10211,
  /* 0x27E3 */ /* Socket is not listening */
  MSP_ERROR_NET_CONNECTCLOSE = 10212,
  /* 0x27E4 */ /* The other side of connection is closed */
  MSP_ERROR_NET_NOTDGRAMSOCK = 10213,
  /* 0x27E5 */ /* The socket is not datagram type */
  MSP_ERROR_NET_DNS = 10214,
  /* 0x27E6 */ /* domain name is invalid or dns server does not function well */
  MSP_ERROR_NET_INIT = 10215,
  /* 0x27E7 */ /* ssl ctx create failed */

  /*nfl error*/
  MSP_ERROR_NFL_INNER_ERROR = 10216, /* NFL inner error */
  MSP_ERROR_MSS_TIME_OUT = 10217,    /* MSS TIMEOUT */
  MSP_ERROT_CLIENT_TIME_OUT = 10218, /* CLIENT TIMEOUT */
  MSP_ERROR_CLIENT_CLOSE = 10219,    /* CLIENT CLOSED CONNECTION */

  MSP_ERROR_CLIENT_AREA_CHANGE = 10220,
  MSP_ERROR_NET_SSL_HANDSHAKE = 10221,
  MSP_ERROR_NET_INVALID_ROOT_CERT = 10222,
  MSP_ERROR_NET_INVALID_CLIENT_CERT = 10223,
  MSP_ERROR_NET_INVALID_SERVER_CERT = 10224,
  MSP_ERROR_NET_INVALID_KEY = 10225,
  MSP_ERROR_NET_CERT_VERIFY_FAILED = 10226,

  /* Error codes of mssp message 10300(0x283C) */
  MSP_ERROR_MSG_GENERAL = 10300,                /* 0x283C */
  MSP_ERROR_MSG_PARSE_ERROR = 10301,            /* 0x283D */
  MSP_ERROR_MSG_BUILD_ERROR = 10302,            /* 0x283E */
  MSP_ERROR_MSG_PARAM_ERROR = 10303,            /* 0x283F */
  MSP_ERROR_MSG_CONTENT_EMPTY = 10304,          /* 0x2840 */
  MSP_ERROR_MSG_INVALID_CONTENT_TYPE = 10305,   /* 0x2841 */
  MSP_ERROR_MSG_INVALID_CONTENT_LENGTH = 10306, /* 0x2842 */
  MSP_ERROR_MSG_INVALID_CONTENT_ENCODE = 10307, /* 0x2843 */
  MSP_ERROR_MSG_INVALID_KEY = 10308,            /* 0x2844 */
  MSP_ERROR_MSG_KEY_EMPTY = 10309,              /* 0x2845 */
  MSP_ERROR_MSG_SESSION_ID_EMPTY = 10310,
  /* 0x2846 */ /* �ỰIDΪ�� */
  MSP_ERROR_MSG_LOGIN_ID_EMPTY = 10311,
  /* 0x2847 */                           /* ��Ƶ����IDΪ�� */
  MSP_ERROR_MSG_SYNC_ID_EMPTY = 10312,   /* 0x2848 */
  MSP_ERROR_MSG_APP_ID_EMPTY = 10313,    /* 0x2849 */
  MSP_ERROR_MSG_EXTERN_ID_EMPTY = 10314, /* 0x284A */
  MSP_ERROR_MSG_INVALID_CMD = 10315,     /* 0x284B */
  MSP_ERROR_MSG_INVALID_SUBJECT = 10316, /* 0x284C */
  MSP_ERROR_MSG_INVALID_VERSION = 10317, /* 0x284D */
  MSP_ERROR_MSG_NO_CMD = 10318,          /* 0x284E */
  MSP_ERROR_MSG_NO_SUBJECT = 10319,      /* 0x284F */
  MSP_ERROR_MSG_NO_VERSION = 10320,      /* 0x2850 */
  MSP_ERROR_MSG_MSSP_EMPTY = 10321,      /* 0x2851 */
  MSP_ERROR_MSG_NEW_RESPONSE = 10322,    /* 0x2852 */
  MSP_ERROR_MSG_NEW_CONTENT = 10323,     /* 0x2853 */
  MSP_ERROR_MSG_INVALID_SESSION_ID = 10324,
  /* 0x2854 */                           /* ��Ч�ĻỰID(sid) */
  MSP_ERROR_MSG_INVALID_CONTENT = 10325, /* 0x2855 */

  /* Error codes of DataBase 10400(0x28A0)*/
  MSP_ERROR_DB_GENERAL = 10400,
  /* 0x28A0 */                    /* ���ݿ��쳣 */
  MSP_ERROR_DB_EXCEPTION = 10401, /* 0x28A1 */
  MSP_ERROR_DB_NO_RESULT = 10402,
  /* 0x28A2 */                        /* redis��û���ҵ��ỰID(sid) */
  MSP_ERROR_DB_INVALID_USER = 10403,  /* 0x28A3 */
  MSP_ERROR_DB_INVALID_PWD = 10404,   /* 0x28A4 */
  MSP_ERROR_DB_CONNECT = 10405,       /* 0x28A5 */
  MSP_ERROR_DB_INVALID_SQL = 10406,   /* 0x28A6 */
  MSP_ERROR_DB_INVALID_APPID = 10407, /* 0x28A7 */
  MSP_ERROR_DB_NO_UID = 10408,

  /* Error codes of Resource 10500(0x2904)*/
  MSP_ERROR_RES_GENERAL = 10500, /* 0x2904 */
  MSP_ERROR_RES_LOAD = 10501,
  /* 0x2905 */ /* Load resource */
  MSP_ERROR_RES_FREE = 10502,
  /* 0x2906 */ /* Free resource */
  MSP_ERROR_RES_MISSING = 10503,
  /* 0x2907 */ /* Resource File Missing */
  MSP_ERROR_RES_INVALID_NAME = 10504,
  /* 0x2908 */ /* Invalid resource file name */
  MSP_ERROR_RES_INVALID_ID = 10505,
  /* 0x2909 */ /* Invalid resource ID */
  MSP_ERROR_RES_INVALID_IMG = 10506,
  /* 0x290A */ /* Invalid resource image pointer */
  MSP_ERROR_RES_WRITE = 10507,
  /* 0x290B */ /* Write read-only resource */
  MSP_ERROR_RES_LEAK = 10508,
  /* 0x290C */ /* Resource leak out */
  MSP_ERROR_RES_HEAD = 10509,
  /* 0x290D */ /* Resource head currupt */
  MSP_ERROR_RES_DATA = 10510,
  /* 0x290E */ /* Resource data currupt */
  MSP_ERROR_RES_SKIP = 10511,
  /* 0x290F */ /* Resource file skipped */

  /* Error codes of TTS 10600(0x2968)*/
  MSP_ERROR_TTS_GENERAL = 10600, /* 0x2968 */
  MSP_ERROR_TTS_TEXTEND = 10601,
  /* 0x2969 */ /* Meet text end */
  MSP_ERROR_TTS_TEXT_EMPTY = 10602,
  /* 0x296A */                      /* no synth text */
  MSP_ERROR_TTS_LTTS_ERROR = 10603, /* 0x296B */

  /* Error codes of Recognizer 10700(0x29CC) */
  MSP_ERROR_REC_GENERAL = 10700,
  /* 0x29CC */                              /* �����쳣 */
  MSP_ERROR_REC_INACTIVE = 10701,           /* 0x29CD */
  MSP_ERROR_REC_GRAMMAR_ERROR = 10702,      /* 0x29CE */
  MSP_ERROR_REC_NO_ACTIVE_GRAMMARS = 10703, /* 0x29CF */
  MSP_ERROR_REC_DUPLICATE_GRAMMAR = 10704,  /* 0x29D0 */
  MSP_ERROR_REC_INVALID_MEDIA_TYPE = 10705, /* 0x29D1 */
  MSP_ERROR_REC_INVALID_LANGUAGE = 10706,   /* 0x29D2 */
  MSP_ERROR_REC_URI_NOT_FOUND = 10707,      /* 0x29D3 */
  MSP_ERROR_REC_URI_TIMEOUT = 10708,        /* 0x29D4 */
  MSP_ERROR_REC_URI_FETCH_ERROR = 10709,    /* 0x29D5 */
  MSP_ERROR_REC_PROC_MOD = 10710,           /* 0x29D6 */

  /* Error codes of Speech Detector 10800(0x2A30) */
  MSP_ERROR_EP_GENERAL = 10800,         /* 0x2A30 */
  MSP_ERROR_EP_NO_SESSION_NAME = 10801, /* 0x2A31 */
  MSP_ERROR_EP_INACTIVE = 10802,        /* 0x2A32 */
  MSP_ERROR_EP_INITIALIZED = 10803,     /* 0x2A33 */

  /* Error codes of TUV */
  MSP_ERROR_TUV_GENERAL = 10900, /* 0x2A94 */
  MSP_ERROR_TUV_GETHIDPARAM = 10901,
  /* 0x2A95 */ /* Get Busin Param huanid*/
  MSP_ERROR_TUV_TOKEN = 10902,
  /* 0x2A96 */ /* Get Token */
  MSP_ERROR_TUV_CFGFILE = 10903,
  /* 0x2A97 */ /* Open cfg file */
  MSP_ERROR_TUV_RECV_CONTENT = 10904,
  /* 0x2A98 */ /* received content is error */
  MSP_ERROR_TUV_VERFAIL = 10905,
  /* 0x2A99 */ /* Verify failure */

  /* Error codes of IMTV */
  MSP_ERROR_LOGIN_SUCCESS = 11000,
  /* 0x2AF8 */ /* �ɹ� */
  MSP_ERROR_LOGIN_NO_LICENSE = 11001,
  /* 0x2AF9 */ /* ���ô����������û���Ҫ���� */
  MSP_ERROR_LOGIN_SESSIONID_INVALID = 11002,
  /* 0x2AFA */ /* SessionIdʧЧ����Ҫ���µ�¼ͨ��֤ */
  MSP_ERROR_LOGIN_SESSIONID_ERROR = 11003,
  /* 0x2AFB */ /* SessionIdΪ�գ����߷Ƿ� */
  MSP_ERROR_LOGIN_UNLOGIN = 11004,
  /* 0x2AFC */ /* δ��¼ͨ��֤ */
  MSP_ERROR_LOGIN_INVALID_USER = 11005,
  /* 0x2AFD */ /* �û�ID��Ч */
  MSP_ERROR_LOGIN_INVALID_PWD = 11006,
  /* 0x2AFE */ /* �û�������Ч */
  MSP_ERROR_LOGIN_SYSTEM_ERROR = 11099,
  /* 0x2B5B */ /* ϵͳ���� */

  /* Error codes of HCR */
  MSP_ERROR_HCR_GENERAL = 11100,
  MSP_ERROR_HCR_RESOURCE_NOT_EXIST = 11101,
  MSP_ERROR_HCR_CREATE = 11102,
  MSP_ERROR_HCR_DESTROY = 11103,
  MSP_ERROR_HCR_START = 11104,
  MSP_ERROR_HCR_APPEND_STROKES = 11105,
  MSP_ERROR_HCR_INIT = 11106,
  MSP_ERROR_HCR_POINT_DECODE = 11107,
  MSP_ERROR_HCR_DISPATCH = 11108,
  MSP_ERROR_HCR_GETRESULT = 11109,
  MSP_ERROR_HCR_RESOURCE = 11110,

  /* Error Codes using in local engine */
  MSP_ERROR_AUTH_NO_LICENSE = 11200,
  /* 0x2BC0 */ /* ����Ȩ */
  MSP_ERROR_AUTH_NO_ENOUGH_LICENSE = 11201,
  /* 0x2BC1 */ /* ��Ȩ���� */
  MSP_ERROR_AUTH_INVALID_LICENSE = 11202,
  /* 0x2BC2 */ /* ��Ч����Ȩ */
  MSP_ERROR_AUTH_LICENSE_EXPIRED = 11203,
  /* 0x2BC3 */ /* ��Ȩ���� */
  MSP_ERROR_AUTH_NEED_MORE_DATA = 11204,
  /* 0x2BC4 */ /* ���豸��Ϣ */
  MSP_ERROR_AUTH_LICENSE_TO_BE_EXPIRED = 11205,
  /* 0x2BC5 */ /* ��Ȩ�������ڣ������Դ����� */
  MSP_ERROR_AUTH_INVALID_MACHINE_ID = 11206,
  /* 0x2BC6 */ /* ��Ч�Ļ����� */
  MSP_ERROR_AUTH_LOCAL_ASR_FORBIDDEN = 11207,
  /* 0x2BC7 */ /* ��ֹʹ�ñ���ʶ������ */
  MSP_ERROR_AUTH_LOCAL_TTS_FORBIDDEN = 11208,
  /* 0x2BC8 */ /* ��ֹʹ�ñ��غϳ����� */
  MSP_ERROR_AUTH_LOCAL_IVW_FORBIDDEN = 11209,
  /* 0x2BC9 */ /* ��ֹʹ�ñ��ػ������� */
  MSP_ERROR_AUTH_APPID_NOT_MATCH = 11210,
  /* 0x2BCA */ /* ��Դappid��Ӧ��appid��ƥ�� */
  MSP_ERROR_AUTH_UID_NOT_MATCH = 11211,
  /* 0x2BCB */ /* ��Դuid�͵�¼�û�uid��ƥ�� */
  MSP_ERROR_AUTH_TRIAL_EXPIRED = 11212,
  /* 0x2BCC */ /* ������Դ���� */
  MSP_ERROR_AUTH_LOCAL_IFD_FORBIDDEN = 11213,
  /* 0x2BC9 */ /* ��ֹʹ�ñ����������� */

  /*Error Codes of Authorization*/
  MSP_ERROR_AUTH_DVC_NO_LICENSE = 11300,
  MSP_ERROR_AUTH_DVC_NO_ENOUGH_LICENSE = 11301,
  MSP_ERROR_AUTH_DVC_INVALID_LICENSE = 11302,
  MSP_ERROR_AUTH_DVC_LICENSE_EXPIRED = 11303,
  MSP_ERROR_AUTH_DVC_NEED_MORE_DATA = 11304,
  MSP_ERROR_AUTH_DVC_LICENSE_TO_BE_EXPIRED = 11305,
  MSP_ERROR_AUTH_DVC_EXCEED_LICENSE = 11306,

  /* Error codes of Ise */

  MSP_ERROR_ASE_EXCEP_SILENCE = 11401,
  MSP_ERROR_ASE_EXCEP_SNRATIO = 11402,
  MSP_ERROR_ASE_EXCEP_PAPERDATA = 11403,
  MSP_ERROR_ASE_EXCEP_PAPERCONTENTS = 11404,
  MSP_ERROR_ASE_EXCEP_NOTMONO = 11405,
  MSP_ERROR_ASE_EXCEP_OTHERS = 11406,
  MSP_ERROR_ASE_EXCEP_PAPERFMT = 11407,
  MSP_ERROR_ASE_EXCEP_ULISTWORD = 11408,

  /* Error codes of IVP */
  MSP_ERROR_IVP_GENERAL = 11600,  //  �ں��쳣
  MSP_ERROR_IVP_EXTRA_RGN_SOPPORT =
      11601,  //  ע��ʱ��������д��Ƶ������������(9��)
  MSP_ERROR_IVP_TRUNCATED =
      11602,  //  ��Ƶ�ط�(���źŲ��εķ���̫�󣬶�����ϵͳ�����Է�Χ)�����¼���������Ƶ
  MSP_ERROR_IVP_MUCH_NOISE = 11603,       //  ��Ƶ����ȹ���
  MSP_ERROR_IVP_TOO_LOW = 11604,          //  ��Ƶ��������
  MSP_ERROR_IVP_ZERO_AUDIO = 11605,       //  ����Ƶ
  MSP_ERROR_IVP_UTTER_TOO_SHORT = 11606,  //  ��Ƶ̫��
  MSP_ERROR_IVP_TEXT_NOT_MATCH =
      11607,  //  1.��Ƶ���ı���ƥ�䣬����ԭ��1.����(�ڰ���¼����֮ǰ��)
              //  2.¼����������������¼���������Ƶ��������Ƶ���г������ 3.ȷʵ��ƥ��"
  MSP_ERROR_IVP_NO_ENOUGH_AUDIO =
      11608,  //  ��Ƶ������ע������˵����д�����Ƶ�ֲ�����ʱ�ᱨ�����ߵ����߼�������Ƶ
  MSP_ERROR_IVP_MODEL_NOT_FOUND_IN_HBASE = 11610,  //  ģ����hbase��û�ҵ�

  /* Error codes of Face */

  MSP_ERROR_IFR_NOT_FACE_IMAGE =
      11700,  //	������������Ӧ�������������20200 ��
  MSP_ERROR_FACE_IMAGE_FULL_LEFT =
      11701,  //	���������󣬶�Ӧ�������������20201��
  MSP_ERROR_FACE_IMAGE_FULL_RIGHT =
      11702,  //	���������ң���Ӧ�������������20202��
  MSP_ERROR_IMAGE_CLOCKWISE_WHIRL =
      11703,  //	��˳ʱ����ת����Ӧ�������������20203��
  MSP_ERROR_IMAGE_COUNTET_CLOCKWISE_WHIRL =
      11704,  //	����ʱ����ת����Ӧ�������������20204��
  MSP_ERROR_VALID_IMAGE_SIZE =
      11705,                       //	��ͼƬ��С�쳣 ����Ӧ�������������20205��
  MSP_ERROR_ILLUMINATION = 11706,  //	�������쳣����Ӧ�������������20206��
  MSP_ERROR_FACE_OCCULTATION =
      11707,  //	���������ڵ�����Ӧ�������������20207��
  MSP_ERROR_FACE_INVALID_MODEL =
      11708,  //	���Ƿ�ģ�����ݣ���Ӧ�������������20208��
  MSP_ERROR_FUSION_INVALID_INPUT_TYPE =
      11709,  //	�������������ͷǷ�����Ӧ�������������20300��
  MSP_ERROR_FUSION_NO_ENOUGH_DATA =
      11710,  //	����������ݲ���������Ӧ�������������20301��
  MSP_ERROR_FUSION_ENOUGH_DATA =
      11711,  //	����������ݹ��࣬��Ӧ�������������20302��

  /*Error Codes of AIUI*/
  MSP_ERROR_AIUI_CID_EXPIRED = 11800,

  /* Error codes of http 12000(0x2EE0) */
  MSP_ERROR_HTTP_BASE = 12000, /* 0x2EE0 */
  MSP_ERROR_HTTP_400 = 12400,
  MSP_ERROR_HTTP_401 = 12401,
  MSP_ERROR_HTTP_402 = 12402,
  MSP_ERROR_HTTP_403 = 12403,
  MSP_ERROR_HTTP_404 = 12404,
  MSP_ERROR_HTTP_405 = 12405,
  MSP_ERROR_HTTP_406 = 12406,
  MSP_ERROR_HTTP_407 = 12407,
  MSP_ERROR_HTTP_408 = 12408,
  MSP_ERROR_HTTP_409 = 12409,
  MSP_ERROR_HTTP_410 = 12410,
  MSP_ERROR_HTTP_411 = 12411,
  MSP_ERROR_HTTP_412 = 12412,
  MSP_ERROR_HTTP_413 = 12413,
  MSP_ERROR_HTTP_414 = 12414,
  MSP_ERROR_HTTP_415 = 12415,
  MSP_ERROR_HTTP_416 = 12416,
  MSP_ERROR_HTTP_417 = 12417,
  MSP_ERROR_HTTP_500 = 12500,
  MSP_ERROR_HTTP_501 = 12501,
  MSP_ERROR_HTTP_502 = 12502,
  MSP_ERROR_HTTP_503 = 12503,
  MSP_ERROR_HTTP_504 = 12504,
  MSP_ERROR_HTTP_505 = 12505,
  /*Error codes of ISV */
  MSP_ERROR_ISV_NO_USER = 13000,
  /* 32C8 */ /* the user doesn't exist */

  /* Error codes of Lua scripts */
  MSP_ERROR_LUA_BASE = 14000,          /* 0x36B0 */
  MSP_ERROR_LUA_YIELD = 14001,         /* 0x36B1 */
  MSP_ERROR_LUA_ERRRUN = 14002,        /* 0x36B2 */
  MSP_ERROR_LUA_ERRSYNTAX = 14003,     /* 0x36B3 */
  MSP_ERROR_LUA_ERRMEM = 14004,        /* 0x36B4 */
  MSP_ERROR_LUA_ERRERR = 14005,        /* 0x36B5 */
  MSP_ERROR_LUA_INVALID_PARAM = 14006, /* 0x36B6 */

  /* Error codes of MMP */
  MSP_ERROR_MMP_BASE = 15000,            /* 0x3A98 */
  MSP_ERROR_MMP_MYSQL_INITFAIL = 15001,  /* 0x3A99 */
  MSP_ERROR_MMP_REDIS_INITFAIL = 15002,  /* 0x3A9A */
  MSP_ERROR_MMP_NETDSS_INITFAIL = 15003, /* 0x3A9B */
  MSP_ERROR_MMP_TAIR_INITFAIL = 15004,   /* 0x3A9C */
  MSP_ERROR_MMP_MAIL_SESSION_FAIL = 15006,
  /* 0x3A9E */ /* �ʼ���½������ʱ���Ự����*/
  MSP_ERROR_MMP_MAIL_LOGON_FAIL = 15007,
  /* 0x3A9F */ /* �ʼ���½������ʱ���ܾ���½��*/
  MSP_ERROR_MMP_MAIL_USER_ILLEGAL = 15008,
  /* 0x3AA0 */ /* �ʼ���½������ʱ���û����Ƿ���*/
  MSP_ERROR_MMP_MAIL_PWD_ERR = 15009,
  /* 0x3AA1 */ /* �ʼ���½������ʱ���������*/
  MSP_ERROR_MMP_MAIL_SOCKET_ERR = 15010,
  /* 0x3AA2 */ /* �ʼ����͹������׽��ִ���*/
  MSP_ERROR_MMP_MAIL_INIT_FAIL = 15011,
  /* 0x3AA3 */ /* �ʼ���ʼ������*/
  MSP_ERROR_MMP_STORE_MNR_NO_INIT = 15012,
  /* 0x3AA4 */ /* store_managerδ��ʼ�������ʼ��ʧ��*/
  MSP_ERROR_MMP_STORE_MNR_POOL_FULL = 15013,
  /* 0x3AA5 */ /* store_manager�����ӳ�����*/
  MSP_ERROR_MMP_STRATGY_PARAM_ILLEGAL = 15014,
  /* 0x3AA6 */ /* �������Ա��ʽ�Ƿ�*/
  MSP_ERROR_MMP_STRATGY_PARAM_TOOLOOG = 15015,
  /* 0x3AA7 */ /* �������Ա��ʽ̫��*/
  MSP_ERROR_MMP_PARAM_NULL = 15016,
  /* 0x3AA8 */ /* ��������Ϊ��*/
  MSP_ERROR_MMP_ERR_MORE_TOTAL = 15017,
  /* 0x3AA9 */ /* pms�������ݿ��д�����ܱ�����ݣ�������� > �ܴ�����*/
  MSP_ERROR_MMP_PROC_THRESHOLD = 15018,
  /* 0x3AAA */ /* ���̼�ط�ֵ���ô���*/
  MSP_ERROR_MMP_SERVER_THRESHOLD = 15019,
  /* 0x3AAB */ /* ��������ط�ֵ���ô���*/
  MSP_ERROR_MMP_PYTHON_NO_EXIST = 15020,
  /* 0x3AAC */ /* python�ű��ļ������� */
  MSP_ERROR_MMP_PYTHON_IMPORT_FAILED = 15021,
  /* 0x3AAD */ /* python�ű�������� */
  MSP_ERROR_MMP_PYTHON_BAD_FUNC = 15022,
  /* 0x3AAE */ /* python�ű�������ʽ���� */
  MSP_ERROR_MMP_DB_DATA_ILLEGAL = 15023,
  /* 0x3AAF */ /* �������ݿ��е����ݸ�ʽ���� */
  MSP_ERROR_MMP_REDIS_NOT_CONN = 15024,
  /* 0x3AB0 */ /* redisû�����ӵ������ */
  MSP_ERROR_MMP_PMA_NOT_FOUND_STRATEGY = 15025,
  /* 0x3AB1 */ /* û���ҵ��������� */
  MSP_ERROR_MMP_TAIR_CONNECT = 15026,
  /* 0x3AB2 */ /* ����tair��Ⱥʧ�� */
  MSP_ERROR_MMP_PMC_SERVINFO_INVALID = 15027,
  /* Ox3AB3 */ /* ��pmc�ķ�������Ϣ�Ѿ���Ч */
  MSP_ERROR_MMP_ALARM_GROUP_NULL = 15028,
  /* Ox3AB4 */ /* �����������Ķ��ű��������ʼ��������Ϊ�� */
  MSP_ERROR_MMP_ALARM_CONTXT_NULL = 15029,
  /* Ox3AB5 */ /* �����������ı�������Ϊ�� */

  /* Error codes of MSC(lmod loader) */
  MSP_ERROR_LMOD_BASE = 16000, /* 0x3E80 */
  MSP_ERROR_LMOD_NOT_FOUND = 16001,
  /* 0x3E81 */ /* û�ҵ�lmod�ļ� */
  MSP_ERROR_LMOD_UNEXPECTED_BIN = 16002,
  /* 0x3E82 */ /* ��Ч��lmod */
  MSP_ERROR_LMOD_LOADCODE = 16003,
  /* 0x3E83 */ /* ����lmodָ��ʧ�� */
  MSP_ERROR_LMOD_PRECALL = 16004,
  /* 0x3E84 */ /* ��ʼ��lmodʧ�� */
  MSP_ERROR_LMOD_RUNTIME_EXCEPTION = 16005,
  /* 0x3E85 */ /* lmod����ʱ�쳣 */
  MSP_ERROR_LMOD_ALREADY_LOADED = 16006,
  /* 0x3E86 */ /* lmod�ظ����� */

  // Error code of Third Business
  MSP_ERROR_BIZ_BASE = 17000,
  /* 0x4268 */ /* ����ҵ������� */

  // Error of Nginx errlog file increase exception
  MSP_ERROR_NGX_LOG_MORE_TOTEL_SIZE = 18000, /*nginx������־��С�쳣*/

  // Error of Flash client when network checking
  MSP_ERROR_FLASH_NETWORK_CONNECT_FIALED = 19000,   /*flash�������������ʧ��*/
  MSP_ERROR_FLASH_NETWORK_CHECK_FIALED = 19001,     /*flash�������Ӧ���쳣��Ϣ*/
  MSP_ERROR_FLASH_NETWORK_CHECK_TIMEOUT = 19002,    /*flash��������糬ʱ*/
  MSP_ERROR_FLASH_NETWORK_CLOSED_EXCEPTION = 19003, /*flash����������쳣�ر�*/

  /*Error Code Of Speech plus*/

  SPEECH_ERROR_NO_NETWORK = 20001,      /* ����Ч����������*/
  SPEECH_ERROR_NETWORK_TIMEOUT = 20002, /* �������ӳ�ʱ*/
  SPEECH_ERROR_NET_EXPECTION = 20003,   /* �����쳣*/
  SPEECH_ERROR_INVALID_RESULT = 20004,  /* ����Ч�Ľ��*/
  SPEECH_ERROR_NO_MATCH = 20005,        /* ��ƥ���� */
  SPEECH_ERROR_AUDIO_RECORD = 20006,    /* ¼��ʧ�� */
  SPEECH_ERROR_NO_SPPECH = 20007,       /* δ��⵽����*/

  SPEECH_ERROR_SPEECH_TIMEOUT = 20008,  /* ��Ƶ���볬ʱ*/
  SPEECH_ERROR_EMPTY_UTTERANCE = 20009, /* ��Ч���ı����� */
  SPEECH_ERROR_FILE_ACCESS = 20010,     /* �ļ���дʧ�� */
  SPEECH_ERROR_PLAY_MEDIA = 20011,      /* ��Ƶ����ʧ�� */

  SPEECH_ERROR_INVALID_PARAM = 20012,     /* ��Ч�Ĳ���*/
  SPEECH_ERROR_TEXT_OVERFLOW = 20013,     /* �ı���� */
  SPEECH_ERROR_INVALID_DATA = 20014,      /* ��Ч���� */
  SPEECH_ERROR_LOGIN = 20015,             /* �û�δ��½*/
  SPEECH_ERROR_PERMISSION_DENIED = 20016, /* ��Ч��Ȩ */
  SPEECH_ERROR_INTERRUPT = 20017,         /* ���쳣��� */

  SPEECH_ERROR_VERSION_LOWER = 20018,     /* �汾���� */
  SPEECH_CLIENT_ERROR_ISUSING = 20019,    /* ¼������ռ��(iOSƽ̨) */
  SPEECH_ERROR_SYSTEM_PREINSTALL = 20020, /* ϵͳԤ�ð汾 */
  SPEECH_ERROR_UNSATISFIED_LINK = 20021,  /* δʵ�ֵ�Native�������� */
  SPEECH_ERROR_UNKNOWN = 20999,           /* δ֪���� */

  SPEECH_ERROR_COMPONENT_NOT_INSTALLED = 21001, /* û�а�װ������� */
  SPEECH_ERROR_ENGINE_NOT_SUPPORTED = 21002,    /* ���治֧�� */
  SPEECH_ERROR_ENGINE_INIT_FAIL = 21003,        /* ��ʼ��ʧ�� */
  SPEECH_ERROR_ENGINE_CALL_FAIL = 21004,        /* ����ʧ�� */
  SPEECH_ERROR_ENGINE_BUSY = 21005,             /* ���深æ */

  SPEECH_ERROR_LOCAL_NO_INIT = 22001,  /* ��������δ��ʼ�� */
  SPEECH_ERROR_LOCAL_RESOURCE = 22002, /* ������������Դ */
  SPEECH_ERROR_LOCAL_ENGINE = 22003,   /* ���������ڲ����� */
  SPEECH_ERROR_IVW_INTERRUPT = 22004,  /* ���ػ������汻�쳣��� */

  /*Error Code Of Local iflytek Engines*/

  /*Error Code Of AiTalk*/

  /*Error Code Of AiTalk Operation*/
  SPEECH_SUCCESS = 0,  // ivErr_OK                  = 0 /*�ɹ�״̬*/

  SPEECH_ERROR_ASR_CLIENT = 23000,
  /*�ͻ���Ӧ�ó������*/                             //?????????
  SPEECH_ERROR_ASR_INVALID_PARA = 23001,         /*��Ч�Ĳ���*/
  SPEECH_ERROR_ASR_INVALID_PARA_VALUE = 23002,   /*��Ч�Ĳ���ֵ*/
  SPEECH_ERROR_ASR_OUT_OF_MEMORY = 23003,        /*�ڴ�ľ�*/
  SPEECH_ERROR_ASR_CREATE_HANDLE_FAILED = 23004, /*�������ʧ��*/
  SPEECH_ERROR_ASR_ENGINE_INIT_FAILED = 23005,   /*�����ʼ��ʧ��*/
  SPEECH_ERROR_ASR_ENGINE_STARTED = 23006,       /*�����Ѿ�����*/
  SPEECH_ERROR_ASR_ENGINE_UNINIT = 23007,        /*����δ��ʼ��*/
  SPEECH_ERROR_ASR_SPEECH_TIMEOUT =
      23008, /*ʶ��ʱ��VADû������û�м�⵽��˵㣩*/
  SPEECH_ERROR_ASR_NO_RECOGNIZED_RESULT = 23009, /*��ʶ����*/
  SPEECH_ERROR_ASR_INVALID_HANDLE = 23010,       /*��Ч�ľ��*/
  SPEECH_ERROR_ASR_FILE_ACCESS = 23011,          /*���ļ�ʧ��*/

  /*Error Code Of AiTalk Engine*/
  SPEECH_ERROR_AITALK_FALSE = 23100,  // ivErr_FALSE               = 1

  /* For license check */
  SPEECH_ERROR_AITALK_PERMISSION_DENIED =
      23101,  // ivErr_InvSN               = 2

  /* General */
  SPEECH_ERROR_AITALK_INVALID_PARA = 23102,  // ivErr_InvArg              = 3
  SPEECH_ERROR_AITALK_BUFFER_OVERFLOW =
      23103,  // ivErr_BufferFull          = 4  /*��Ƶ���ݻ���������*/
  SPEECH_ERROR_AITALK_FAILED = 23104,  // ivErr_Failed              = 5
  SPEECH_ERROR_AITALK_NOT_SUPPORTED =
      23105,  // ivErr_NotSupport          = 6  /*���治֧��*/
  SPEECH_ERROR_AITALK_OUT_OF_MEMORY = 23106,  // ivErr_OutOfMemory         = 7
  SPEECH_ERROR_AITALK_INVALID_RESOURCE =
      23107,  // ivErr_InvResource         = 8  /*��Դ��Ч*/
  SPEECH_ERROR_AITALK_NOT_FOUND =
      23108,  // ivErr_NotFound            = 9  /*���ļ�ʧ��*/
  SPEECH_ERROR_AITALK_INVALID_GRAMMAR =
      23109,  // ivErr_InvGrmr             = 10 /*ʶ���﷨����*/

  /* For object status */
  SPEECH_ERROR_AITALK_INVALID_CALL =
      23110,  // ivErr_InvCall             = 11 /*��Ч����*/

  /* For ASR Input */
  SPEECH_ERROR_AITALK_SYNTAX_ERROR = 23111,  // ivErr_InvCall             = 12

  /* For Message Call Back */
  SPEECH_ERROR_AITALK_RESET = 23112,  // ivErr_Reset               = 13
  SPEECH_ERROR_AITALK_ENDED = 23113,  // ivErr_Ended               = 14
  SPEECH_ERROR_AITALK_IDLE = 23114,   // ivErr_Idle                = 15
  SPEECH_ERROR_AITALK_CANNOT_SAVE_FILE =
      23115,  // ivErr_CanNotSaveFile      = 16

  /* For Lexicon name */
  SPEECH_ERROR_AITALK_INVALID_GRAMMAR_NAME =
      23116,  // ivErr_InvName             = 17 /*�ķ���ʵ����ƷǷ�*/

  SPEECH_ERROR_AITALK_BUFFER_EMPTY = 23117,  // ivErr_BufferEmpty         = 18

  SPEECH_ERROR_AITALK_GET_RESULT = 23118,  // ivErr_GetResult           = 19

  SPEECH_ERROR_AITALK_REACT_OUT_TIME =
      23119,  // ivErr_ReactOutTime        = 20 /*��Ӧ��ʱ*/
  SPEECH_ERROR_AITALK_SPEECH_OUT_TIME =
      23120,  // ivErr_SpeechOutTime       = 21 /*������ʱ*/

  SPEECH_ERROR_AITALK_AUDIO_CUT =
      23121,  // ivErr_CUT                 = 22 /*¼����������*/
  SPEECH_ERROR_AITALK_AUDIO_LOWER =
      23122,  // ivErr_LOWER               = 23 /*¼����������*/

  SPEECH_ERROR_AITALK_INSUFFICIENT_PERMISSIONS =
      23123,  // ivErr_Limitted            = 24 /*��Ȩ����*/
  SPEECH_ERROR_AITALK_RESULT_ERROR =
      23124,  // ivErr_ResultError         = 25 /*������Wfst�������Ȼ��cmd���*/
  SPEECH_ERROR_AITALK_SHORT_PAUSE = 23125,  // ivErr_ShortPause          = 26
  SPEECH_ERROR_AITALK_BUSY = 23126,         // ivErr_Busy                = 27
  SPEECH_ERROR_AITALK_GRM_NOT_UPDATE =
      23127,  // ivErr_GrmNotUpdate        = 28 /*�﷨δ����*/
  SPEECH_ERROR_AITALK_STARTED = 23128,  // ivErr_Started             = 29
  SPEECH_ERROR_AITALK_STOPPED = 23129,  // ivErr_Stopped             = 30
  SPEECH_ERROR_AITALK_ALREADY_STARTED =
      23130,  // ivErr_AlreadyStarted      = 31
  SPEECH_ERROR_AITALK_ALREADY_STOPPED =
      23131,  // ivErr_AlreadyStopped      = 32
  SPEECH_ERROR_AITALK_TOO_MANY_COMMAND =
      23132,                              // ivErr_TooManyCmd          = 33
  SPEECH_ERROR_AITALK_WAIT = 23133,       // ivErr_Wait                = 34
                                          // /*�����������һЩ���������߳���Ҫ�ȴ�*/
  SPEECH_ERROR_AITALK_MAE_RIGHT = 23134,  // ivErr_MAERight            = 35
  SPEECH_ERROR_AITALK_MAE_WRONG = 23135,  // ivErr_MAEWrong            = 36

  SPEECH_ERROR_AITALK_GRM_ERR = 23300,  // �﷨����

  /*Error Code Of AiSound*/

  /*Error Code Of AiSound Operation*/
  SPEECH_ERROR_TTS_INVALID_PARA = 24000,         /* ������� */
  SPEECH_ERROR_TTS_INVALID_PARA_VALUE = 24001,   /* ��Ч�Ĳ���ֵ*/
  SPEECH_ERROR_TTS_OUT_OF_MEMORY = 24002,        /* �ڴ治��*/
  SPEECH_ERROR_TTS_INVALID_HANDLE = 24003,       /* ��Ч�ľ��*/
  SPEECH_ERROR_TTS_CREATE_HANDLE_FAILED = 24004, /* �������ʧ��*/
  SPEECH_ERROR_TTS_INVALID_RESOURCE = 24005,     /* ��Ч��Դ */
  SPEECH_ERROR_TTS_INVALID_VOICE_NAME = 24006,   /* ��Ч������*/
  SPEECH_ERROR_TTS_ENGINE_UNINIT = 24007,        /* ����δ��ʼ�� */
  SPEECH_ERROR_TTS_ENGINE_INIT_FAILED = 24008,   /* �����ʼ��ʧ�� */
  SPEECH_ERROR_TTS_ENGINE_BUSY = 24009,          /* ����æ */

  /*Error Code Of AiSound Engine*/
  SPEECH_ERROR_AISOUND_BASE = 24100,
  SPEECH_ERROR_AISOUND_UNIMPEMENTED = 24100, /* unimplemented function */
  SPEECH_ERROR_AISOUND_UNSUPPORTED = 24101,  /* unsupported on this platform */
  SPEECH_ERROR_AISOUND_INVALID_HANDLE = 24102,    /* invalid handle */
  SPEECH_ERROR_AISOUND_INVALID_PARA = 24103,      /* invalid parameter(s) */
  SPEECH_ERROR_AISOUND_INSUFFICIENT_HEAP = 24104, /* insufficient heap size  */
  SPEECH_ERROR_AISOUND_STATE_REFUSE = 24105, /* refuse to do in current state */
  SPEECH_ERROR_AISOUND_INVALID_PARA_ID = 24106,    /* invalid parameter ID */
  SPEECH_ERROR_AISOUND_INVALID_PARA_VALUE = 24107, /* invalid parameter value */
  SPEECH_ERROR_AISOUND_RESOURCE = 24108,           /* Resource is error */
  SPEECH_ERROR_AISOUND_RESOURCE_READ = 24109,      /* read resource error */
  SPEECH_ERROR_AISOUND_LBENDIAN = 24110, /* the Endian of SDK  is error */
  SPEECH_ERROR_AISOUND_HEADFILE =
      24111, /* the HeadFile is different of the SDK */
  SPEECH_ERROR_AISOUND_BUFFER_OVERFLOW =
      24112, /* get data size exceed the data buffer */
  SPEECH_ERROR_AISOUND_INVALID_ISAMPA =
      24113, /* !Invalid iSampa format or input iSampa text contain invalid
                alphabet*/
  SPEECH_ERROR_AISOUND_INVALID_CSSML = 24114, /* !Invalid cssml format */

  /*Error Code Of ivw*/

  /*Error Code Of ivw Operation*/
  SPEECH_ERROR_IVW_ENGINE_UNINI = 25000,   /* ����δ��ʼ�� */
  SPEECH_ERROR_IVW_RESVER_NOMATCH = 25001, /* ��Դ�汾��ƥ�� */
  SPEECH_ERROR_IVW_BUFFERED_AUDIOD_LITTLE =
      25002, /* ���Ѽ�ʶ�𻺴���Ƶ���� */
  SPEECH_ERROR_IVW_INVALID_RESTYPE = 25003, /* ���Ϸ�����Դ���� */

  /*Error Code Of ivw Engine*/
  SPEECH_ERROR_IVW_INVALID_CALL = 25101,       // IvwErr_InvCal       = 1
  SPEECH_ERROR_IVW_INVALID_ARG = 25102,        // IvwErr_InvArg	    = 2
  SPEECH_ERROR_IVW_TELL_SIZE = 25103,          // IvwErr_TellSize     = 3
  SPEECH_ERROR_IVW_OUT_OF_MEMORY = 25104,      // IvwErr_OutOfMemory  = 4
  SPEECH_ERROR_IVW_OUT_BUFFER_FULL = 25105,    // IvwErr_BufferFull	= 5
  SPEECH_ERROR_IVW_OUT_BUFFER_EMPTY = 25106,   // IvwErr_BufferEmpty	= 6
  SPEECH_ERROR_IVW_INVALID_RESOURCE = 25107,   // IvwErr_InvRes		= 7
  SPEECH_ERROR_IVW_REPETITIOPN_ENTER = 25108,  // IvwErr_ReEnter = 8
  SPEECH_ERROR_IVW_NOT_SUPPORT = 25109,        // IvwErr_NotSupport	= 9
  SPEECH_ERROR_IVW_NOT_FOUND = 25110,          // IvwErr_NotFound		= 10
  SPEECH_ERROR_IVW_INVALID_SN = 25111,         // IvwErr_InvSN		= 11
  SPEECH_ERROR_IVW_LIMITTED = 25112,           // IvwErr_Limitted		= 12
  SPEECH_ERROR_IVW_TIME_OUT = 25113,           // IvwErr_TimeOut		= 13

  SPEECH_ERROR_IVW_ENROLL1_SUCESS = 25114,    // IvwErr_Enroll1_Success = 14
  SPEECH_ERROR_IVW_ENROLL1_FAILED = 25115,    // IvwErr_Enroll1_Failed  = 15
  SPEECH_ERROR_IVW_ENROLL2_SUCESS = 25116,    // IvwErr_Enroll2_Success = 16
  SPEECH_ERROR_IVW_ENROLL2_FAILED = 25117,    // IvwErr_Enroll2_Failed  = 17
  SPEECH_ERROR_IVW_ENROLL3_SUCESS = 25118,    // IvwErr_Enroll3_Success = 18
  SPEECH_ERROR_IVW_ENROLL3_FAILED = 25119,    // IvwErr_Enroll3_Failed  = 19
  SPEECH_ERROR_IVW_SPEECH_TOO_SHORT = 25120,  // IvwErr_SpeechTooShort  = 20
  SPEECH_ERROR_IVW_SPEECH_STOP = 25121        // IvwErr_SpeechStop      = 21

};

#endif /* __MSP_ERRORS_H__ */