// LucidConstants.h: global constants
//
//////////////////////////////////////////////////////////////////////
#ifndef _LUCIDCONSTANTS_H
#define _LUCIDCONSTANTS_H

#pragma once
#include "Porting_Classes/INXString.h"
//! Label ths code against the formats it writes- for lpj files, folder-structures etc etc 
#define CODE_FORMAT_MAJOR_REV 1 //!
#define CODE_FORMAT_MINOR_REV 4

#define FLAG_DIFF_VALUE_FIELD "----"


#define PMD_LOCK_FAILURE_MESSAGE \
"Lucid couldn't get a lock on the Project ('.lpj') file\n\
Is a dialog open in any other Lucid Tool,\n\
Or is maybe a 3rd-party application editing it?\n\
\n\
Please try again later"

#define VERY_LARGE_INT 1000000000

#define TIMER_IDS_LOADHTMLHELP 1201
#define TIMER_IDS_DRAWANIMATEDSTUFF 1202
#define TIMER_WAITMS_LOADHTMLHELP 200
#define TIMER_WAITMS_DRAWANIMATEDSTUFF 1000

//port type
#define INPUTPORT 0
#define OUTPUTPORT 1
#define STARTPORT 2
#define FINISHPORT 3
#define INTERNALPORT 4

//data type
#define BOOLEAN 0
#define INTEGER 1
#define FLOAT 2
#define STRING 3
#define EVENT 4

#define MAXPORTS 256
#define MAXFUNCS 256
#define MAXPARAMS 32
#define MAXXPORTS 4096

#define WORK_DIR_SIZE 4096

#define MAXBLOCKPORTS 20

// tolerance values for closeness to objects
#define NODECLOSENESS 3
#define LINECLOSENESS 5.0
#define CLOSENESS 7

// selection tree level limits
#define MAX_FIRST_LEV 256
#define MAX_SECOND_LEV 4096
#define MAX_THIRD_LEV 64000

// snap to grid
#define GRIDSIZE 5

// debug states
#define DBGSTOP '0'
#define DBGSTART '1'
#define DBGPAUSE '2'
#define DBGSTEP '3'

// debug
#define DEBUG_SLEEP 50

// project tree limits
#define MAX_PROJECTS 8
#define MAX_HIER_LEVELS 32
#define MAX_BLOCKS 128
#define MAX_GUIFILES 32
#define MAX_TEXTFILES 128
#define MAX_BMPFILES 128
#define MAX_RESOURCES 130

// page sizes for printing
#define PAGE_WIDTH 800.0
#define PAGE_LENGTH 1000.0
//#define PAGE_WIDTH 4800.0
//#define PAGE_LENGTH 6400.0

// Length of port labels
#define PORT_LABEL 5

// Group defaults
#define DEFAULT_PERIOD 100000
#define DEFAULT_ALLOC 90000

// undo
#define MAX_UNDO_FILE_NUM 5

// zoom
#define MAX_ZOOM_CHAR 8
#define ZOOM_DELTA 10	// was 5 but was causing font rendering problems in wine
#define MAX_ZOOM 100
#define MIN_ZOOM 5

// int to string
#define RADIX 10

// transfer
#define TRANSFER_SLEEP_TIME 5

// scroll
#define INIT_SCROLL_LEFT_MARGIN 25
#define INIT_SCROLL_TOP_MARGIN 25

// encapsulation
#define ENCAPS_XPOS 150
#define ENCAPS_YPOS 25
#define ENCAPS_ICON_WIDTH 86

#define LGB_NULL_COMBO_CHOICE "---"

// RTA-TRACE
#define RTA_TRACE_SIZE 64
#define TRACE_TIMER_ID 1
#define TRACE_PERIOD 1000
#define RTA_TRACE_PATH "C:\\rta\\bin\\RTA-TRACE.exe"

// lock functions
#define getLock() getRealLock(__FILE__, __LINE__)
#define releaseLock() releaseRealLock(__FILE__, __LINE__)

// versions
#define GUI_FILE_VERSION "Version 1.3\n"

// start function
const INXString START = "Run_Start";
const INXString NOPARAMS = "*";

// paths

#define PMD_LOCK_FILE_NAME "\\ProjMetaData.lock"
#define PROJ_LOCK_FILE_NAME "\\Project.lock"

const int NODE_TYPE_ATTRIBUTE_FIELD = 2;
const int NODE_TYPE_COMMENT = 8;
const int NODE_TYPE_START_ELEMENT = 1; //start tag element
const int NODE_TYPE_END_ELEMENT = 15; //end tag element
const int NODE_TYPE_STARTTEXT_ELEMENT = 3; //#text associated with start tag element
const int NODE_TYPE_ENDTEXT_ELEMENT = 14; //#text associated with end tag element

const INXString COMPONENTMENU_STYLE_ELEMENT = "Styles";
const INXString COMPONENTMENU_ITEM_ELEMENT = "MenuItem";
const INXString COMPONENTMENU_ITEM_NAME_ELEMENT = "Name";
const INXString COMPONENTMENU_ITEM_PRIORITY_ELEMENT = "Priority";
const INXString COMPONENTMENU_ITEM_STYLE_ELEMENT = "Style";


const INXString NODE_MENU_ELEMENT = "Menu";
const INXString NODE_ICON_ELEMENT = "Icon";
const INXString NODE_CLASS_ELEMENT = "Class";
const INXString NODE_SDESC_ELEMENT = "ShortDescription";
const INXString NODE_LDESC_ELEMENT = "LongDescription";
const INXString NODE_PARAM_ELEMENT = "Parameter";
const INXString NODE_PARAM_NAME_ELEMENT = "Name";
const INXString NODE_PARAM_DATATYPE_ELEMENT = "DataType";
const INXString NODE_PARAM_DEFAULTVALUE_ELEMENT = "DefaultValue";
const INXString NODE_PARAM_MINVALUE_ELEMENT = "MinValue";
const INXString NODE_PARAM_MAXVALUE_ELEMENT = "MaxValue";
const INXString NODE_PARAM_DESCRIPTION_ELEMENT = "Description";
const INXString NODE_PARAM_DISPLAYONWIDGET_ELEMENT = "DisplayOnWidget";
const INXString NODE_PARAM_DISPLAYONWIDGET_X_ELEMENT = "x";
const INXString NODE_PARAM_DISPLAYONWIDGET_Y_ELEMENT = "y";
const INXString NODE_PARAM_LISTPLACE_ELEMENT = "ListPlacement";
const INXString NODE_PARAM_ARGPLACE_ELEMENT = "ArgPlacement";
const INXString NODE_MODULE_ELEMENT = "Module";
const INXString NODE_PORT_ELEMENT = "Port";
const INXString NODE_PORT_DATATYPE_ELEMENT = "DataType";
const INXString NODE_PORT_DESCRIPTION_ELEMENT = "Description";
const INXString NODE_PORT_PORTTYPE_ELEMENT = "PortType";
const INXString NODE_PORT_XCOORD_ELEMENT = "XCoordinate";
const INXString NODE_PORT_YCOORD_ELEMENT = "YCoordinate";
const INXString NODE_PORT_FUNCTIONNAME_ELEMENT = "FunctionName";
const INXString NODE_PORT_ATOMICFLAG_ELEMENT = "AtomicFlag";
const INXString NODE_PORT_MANDATORYFLAG_ELEMENT = "MandatoryFlag";
const INXString NODE_PORT_WCET_ELEMENT = "Wcet";
const int NODE_PORT_CDF_DRAW_LEFT_OFFSET = 6;
const int NODE_PORT_CDF_DRAW_RIGHT_OFFSET = 8;
const int NODE_PIXELS_TO_INSET_FIRST_NODE_ON_LINE = 5;
const INXString FILE_TYPE_CDF = ".cdf";
const INXString FILE_TYPE_IDF = ".ini";

const INXString USERDEFDIR = "\\userdefined\\";
const INXString IDFDIR = "\\IDF\\";
const INXString CDFDIR = "\\CDF\\";
const INXString NECDIR = "\\microcontroller\\";
const INXString BMPDIR = "\\BMP\\";
const INXString IMAGEDIR = "\\Images\\";
const INXString TRACEDIR = "\\RTA-Trace\\";
const INXString LOGDIR = "\\log\\";
const INXString LUCID_EXECUTABLES_DIR = "\\bin";
const INXString LUCID_DIST_DIR = "\\dist";
const INXString HTML_HELP_FILE = "\\docs\\help.html";
const INXString MENUCONFIGFILE = "\\config\\menu.xml";
const INXString DEFAULT_HELPFILE = "\\defaultHelp.html";

//const INXString FTPSODL = "\\SODL\\t.sdl";
const INXString OPTIONSFILE = "\\options.ini";
const INXString DEBUGDIR = "\\DEPDebug\\";
const INXString DEBUGFILE = "debug";
const INXString TEMPDIR = "\\temp\\";
const INXString LGB_TEMPDIR = "\\temp\\LGB\\";
const INXString DEPDIR = "\\DEP\\";
const INXString DEVICEDATADIR = "\\DeviceData\\";
const INXString DDATADIR = "\\DeviceData\\data\\";
const INXString GUIDIR = "\\DeviceData\\GUI\\";
const INXString EXPORTDIR = "\\export\\";
const INXString SODLDIR = "\\DeviceData\\SODL\\";
const INXString OLDSODLDIR = "\\SODL\\";
const INXString SODLFILENAME = "t.sdl";
const INXString DEFAULTPROJECTROOTDIR = "C:\\Lucid Projects";
const INXString MANUALSDIR = "\\manuals";
const INXString ACROBAT_READER_EXENAME = "AcroRd32.exe";
const INXString TRACE_CONFIG_FILE = "lucid.rta";
const INXString TRACE_HEADER_FILE = "header.rta";
const INXString LAB_LOG_FILE = "lab.log";
const INXString LGB_LOG_FILE = "lgb.log";
const INXString FONTDIR = "\\fonts\\";
const INXString NLSDIR = "\\DeviceData\\NLS\\";
const INXString NLSFILE = "nls.csv";
const INXString TOOLSFILE = "tools.ini";
const INXString RESOURCEDIR = "\\DeviceData\\resource\\";

//--------------------------------------------------------------
// Manuals.
const INXString LGB_USERGUIDE_FILENAME = "\\GUI Builder User Manual.pdf";
const INXString LGB_REFMANUAL_FILENAME = "\\GUI Builder Reference Manual.pdf";
const INXString LAB_USERGUIDE_FILENAME = "\\Lucid User Guide.pdf";
const INXString LAB_REFMANUAL_FILENAME = "\\Lucid Reference Manual.pdf";
const INXString LGBLAB_TUTORIAL_FILENAME = "\\Lucid Tutorial.pdf";
const INXString LOB_REFMANUAL_FILENAME = "\\Object Builder User Manual.pdf";

//--------------------------------------------------------------



// constants used to define IDF section and keys
// Section names
const INXString SECTIONL1 = "MenuLevel1";
const INXString SECTIONL2 = "MenuLevel2";
const INXString SECTIONL3 = "MenuLevel3";
const INXString SECTIONL4 = "MenuLevel4";
const INXString SECTIONICON = "Icon";
const INXString SECTIONCLASS = "Class";
const INXString SECTIONLDESC = "Long Description";
const INXString SECTIONSDESC = "Short Description";
const INXString SECTIONPORT = "";
const INXString SECTIONPARAM = "";

// Key names
const INXString KEYL1 = "level1 menu"; //@todo make all of tjhes the same
const INXString KEYL2 = "level2 menu";
const INXString KEYL3 = "level3 menu";
const INXString KEYL4 = "level4 menu";
const INXString KEYICON = "graphic filename";
const INXString KEYCLASS = "class name";
const INXString KEYDESC = "description";
const INXString KEYPORTTYPE = "port type";
const INXString KEYXCOORD = "x coordinate";
const INXString KEYYCOORD = "y coordinate";
const INXString KEYDATATYPE = "data type";
const INXString KEYATOMICFLAG = "atomic flag";
const INXString KEYPORTVERT = "port vertical";
const INXString KEYFUNCNAME = "";
const INXString KEYNAME = "name";
const INXString KEYMINRANGE = "min range";
const INXString KEYMAXRANGE = "max range";
const INXString KEYDEFAULT = "default";
const INXString KEYUSERDEF = "user defined";
const INXString KEYENUMVAL = "value num";
const INXString KEYENUMLABEL = "value label";
const INXString KEYMANDATORYFLAG = "mandatory flag";

// Extensions
const INXString PROJECTEXT = ".lpj";

const INXString APP = "APP";

// Constants
const INXString INTCONSTTYPE = "const_i1";
const INXString BOOLCONSTTYPE = "const_b1";
const INXString REALCONSTTYPE = "const_r1";
const INXString STRINGCONSTTYPE = "const_s1";
const INXString INT_COMP = "operator_equibx1";
const INXString REAL_COMP = "operator_equrbx1";
const INXString INT_GREATER_THAN = "operator_cmpibx1";
const INXString REAL_GREATER_THAN = "operator_cmprbx1";
const INXString INT_GREATER_THAN_EQUALS = "operator_cmeibx1";
const INXString REAL_GREATER_THAN_EQUALS = "operator_cmerbx1";

// Debug
const INXString EVENT_MSG_TYPE = "E";
const INXString DATA_MSG_TYPE = "D";
const INXString OVERFLOW_MSG_TYPE = "O";
const INXString START_FUNC_MSG_TYPE = "S";
const INXString FINISH_FUNC_MSG_TYPE = "F";

// LucidConstants.h: global constants
//
//////////////////////////////////////////////////////////////////////


//const INXString OPTIONSFILE = "c:\\work\\lucid\\source\\Lucid_GUIBuilder\\options.ini";
//const INXString FTPGUI = "c:\\work\\lucid\\source\\Lucid_GUIBuilder\\ftp\\GUI.txt";

#define TEXT_ICON_DEFAULT_OPACITY 255
#define IMAGE_ICON_DEFAULT_OPACITY 255
#define IMAGE_ICON_DEFAULT_LOCK_ASPECT_RATIO 0

#define LABELHEIGHT 20

#define PATHBUFFER_SIZE 4096
#define STD_LINE_LENGTH 4096
#define STD_FIELD_LENGTH 100
#define FILEBUFFER_SIZE 1000


//paths
//#ifdef _DEBUG
//const INXString GUIDIR = "\\DeviceData\\GUI\\";
//const INXString GUITXT = "\\DeviceData\\GUI\\GUI.txt";

//const INXString SODL_DIR = "\\SODL";
//const INXString SODL_FILE_NAME = "t.sdl";

//const INXString OPTIONSFILE = "\\options.ini";
//const INXString BITMAPDIR = "\\BMP\\";

const INXString LGB_APP_NAME = "LucidGuiBuilder: ";
const INXString LAB_APP_NAME = "LucidApplicationBuilder: ";
const INXString NO_PROJECT_WARNING = "<NO PROJECT SET>";
const INXString ICON_PLACEHOLDER_FILE = "placeholder.png";

// These are used at an earl stage of the application when the INXString above haven't
// yet been instantiated - so use simple define's instead.
#define GUI_LAYOUT_DEFAULT_DESCRIPTION "Auto-generated default layout";
#define GUI_LAYOUT_DEFAULT_FILENAMEPREFIX  "DEFAULT_LAYOUT_FOR_WIDGET_GROUP_"

// Target definitions
const INXString DEFAULT_TARGET_REMOTE_IP = "0.0.0.0";
const INXString DEFAULT_TARGET_LOCAL_IP = "127.0.0.1";
const INXString DEFAULT_TARGET_PORT = "11425";
const INXString DEFAULT_TARGET_LOCATION = "Local";
const INXString DEFAULT_TARGET_APPSERVER_URL = "http://devport.inx-systems.com/appStore/uploadApplication.php";
const INXString DEFAULT_TARGET_APPSERVER_USERNAME = "123@abc";
const INXString DEFAULT_TARGET_APPSERVER_PASSWORD = "password";
const INXString TARGET_DISPLAY_NAME_LOCAL = "My Computer";
const INXString TARGET_DISPLAY_NAME_REMOTE = "IP Address";
const INXString TARGET_DISPLAY_NAME_APPSERVER = "App Server";
const INXString TARGET_DISPLAY_NAME_BLANK = "";

// IDs for text boxes in IconParams dialog
#define ICONPARAMS_ID_FUNCTIONNAME_LABEL 1001
#define ICONPARAMS_ID_FUNCTIONNAME_TEXT 1002
#define ICONPARAMS_ID_INSTANCENAME_LABEL 1003
#define ICONPARAMS_ID_INSTANCENAME_TEXT 1004
#define ICONPARAMS_ID_DESCRIPTION_LABEL 1005
#define ICONPARAMS_ID_DESCRIPTION_TEXT 1006
#define ICONPARAMS_ID_FIRST_PARAMETER 2001 // parameter fields are dynamically created, so assign a range of IDs, starting at 2001

// buffer size for TCPIP data transfer
const int TCPIP_BUFFER_SIZE = (1024 * 1024) * sizeof(char);

// EHS related paths
#define EHS_WIN_VAR_DIR "\\EHS\\ehs_env-win_x86_gtk_gst"
#define EHS_LINUX_VAR_DIR "\\EHS\\ehs_env-linux_x86_gtk_gst"
#define EHS_LINUX_OPT_INSTALL_DIR "/opt/ehs" /*This is the fixed path for a linux package installer*/
#define EHS_LINUX_OPT_RUNTARGET "bin/run_ehs.sh"
#define EHS_SYSDATA_DIR "\\sysdata"
#define EHS_GRAPHICS_INFO_FILE "\\graphics.nfo"
#define EHS_APPDATA_DEFAULT_DIR "\\appdata\\default"
#define EHS_SODL_FILE "\\t.sdl"
#define EHS_BIN_DIR "\\bin"

// App Uploader
#define APP_TAR_FILE "\\app.tar.gz"
#define APP_DESC_DIR "\\DeviceData\\description\\"
#define APP_DESC_FILE "info.xml"

// Registry settings for Target
#define REG_TARGET_KEY "Software\\Lucid\\LucidApplicationBuilder\\Target"
#define REG_TARGET_LOCATION_VALUE "Location"
#define REG_TARGET_LOCAL_PORT_VALUE "LocalPort"
#define REG_TARGET_REMOTE_IP_VALUE "RemoteIp"
#define REG_TARGET_REMOTE_PORT_VALUE "RemotePort"
#define REG_TARGET_APPSERVER_URL_VALUE "AppServerURL"
#define REG_TARGET_APPSERVER_USERNAME_VALUE "AppServerUserName"
#define REG_TARGET_APPSERVER_PASSWORD_VALUE "AppServerPassword"
#define REG_TARGET_LOCATION_DATA_LOCAL "Local"
#define REG_TARGET_LOCATION_DATA_REMOTE "Remote"
#define REG_TARGET_LOCATION_DATA_APPSERVER "AppServer"

// Display Items in Target Combo box
#define TARGET_COMBO_INDEX_LOCAL 0
#define TARGET_COMBO_INDEX_REMOTE 1
#define TARGET_COMBO_INDEX_APPSERVER 2

// disk space thresholds
#define DISK_SPACE_WARNING_MB 50.0
#define DISK_SPACE_CRITICAL_MB 10.0
#define DISK_SPACE_NONE_MB 0.1

#endif
