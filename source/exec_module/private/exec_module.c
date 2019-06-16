
#include <string.h>

#include "exec_module.h"
#include "common_module.h"
#include "board_module.h"
#include "xlat_module.h"



//---------------- Private --------------------------//

//---------------- Definitions ---------------------//

#define SYS_TICK_MSEC 10
#define SER_PORT 0
#define SER_BUFF_LEN 128


/// Current script contents. TODO need to support multiple script files.
static QString p_scriptText;

/// The main Lua context.
static lua_State* p_LMain;

/// The Lua context where the script is running.
static lua_State* p_LScript;

/// The script execution status.
static bool p_scriptRunning = false;

/// Processing loop Status.
static bool p_loopRunning;

/// Serial receive buffer.
static char p_rxBuf[SER_BUFF_LEN];

/// Serial send buffer.
static char p_txBuf[SER_BUFF_LEN];

/// Current tick.
static int p_tick;


//---------------- Functions ---------------------//

/// System tick timer. Handle script yielding and serial IO.
static void p_timerHandler(void);

/// Digital input handler.
/// @param which The digital input whose state has changed.
/// @param value The new state of the input.
static void p_digInputHandler(unsigned int which, bool value);

/// @brief Process for all commands from clients.
/// @param[in] bin The arbitrary command and args.
/// @return status
status_t p_processCommand(const char* bin);

/// @brief Starts the script running.
/// @return status
static status_t p_runScript(void);

/// @brief Stop the currently running script.
/// @return status
static status_t p_stopScript(void);

/// @brief Common handler for execution errors.
/// @return status
static status_t p_processExecError(void);


//---------------- Public API Implementation -------------//

//--------------------------------------------------------//
status_t exec_init(void)
{
    status_t stat = STATUS_OK;

    // Init memory.
    memset(p_rxBuf, 0x00, sizeof(p_rxBuf));
    memset(p_txBuf, 0x00, sizeof(p_txBuf));

    p_loopRunning = false;
    p_tick = 0;

    // Init modules.
    common_setLogLevel(2);
    CHECKED_FUNC(stat, common_init);
    CHECKED_FUNC(stat, board_init);
    CHECKED_FUNC(stat, xlat_init);

    // Set up all board-specific stuff.
    CHECKED_FUNC(stat, board_regTimerInterrupt, SYS_TICK_MSEC, p_timerHandler);
    CHECKED_FUNC(stat, board_serOpen, SER_PORT);

    // Register for input interrupts.
    CHECKED_FUNC(stat, board_regDigInterrupt, p_digInputHandler);
    CHECKED_FUNC(stat, board_regDigInterrupt, p_digInputHandler);
    CHECKED_FUNC(stat, board_regDigInterrupt, p_digInputHandler);

    // Init outputs.
    CHECKED_FUNC(stat, board_writeDig, DIG_OUT_1, true);
    CHECKED_FUNC(stat, board_writeDig, DIG_OUT_2, false);
    CHECKED_FUNC(stat, board_writeDig, DIG_OUT_3, true);

    return stat;
}

//--------------------------------------------------------//
status_t exec_run(void)
{
    status_t stat = STATUS_OK;

    // Let her rip!
    board_enbInterrupts(true);
    p_loopRunning = true;

    while(p_loopRunning)
    {
        // Forever loop.
    }

    board_enbInterrupts(false);

    /// Done, do any cleanup.
    p_stopScript(); // just in case
    lua_close(p_LMain);
    p_LMain = NULL;


    return stat;
}

//--------------------------------------------------------//
status_t exec_exit(void)
{
    status_t stat = STATUS_OK;

    p_loopRunning = false;
    
    return stat;
}

//--------------------------------------------------------//
status_t exec_destroy(void)
{
    status_t stat = STATUS_OK;

    CHECKED_FUNC(stat, board_destroy);
    CHECKED_FUNC(stat, xlat_destroy);
    CHECKED_FUNC(stat, common_destroy);
    
    return stat;
}


//---------------- Private --------------------------//

//--------------------------------------------------------//
void p_timerHandler(void)
{
    // This arrives every SYS_TICK_MSEC.
    // Do the real work of the application.

    status_t stat = STATUS_OK;

    p_tick++;

    if(p_tick % 5 == 0)
    {
        // Poll serial.
        CHECKED_FUNC(stat, board_serReadLine, SER_PORT, p_rxBuf, SER_BUFF_LEN);

        if(strlen(p_rxBuf) > 0)
        {
            // Got something. TODO ser like CHECKED_FUNC(stat, cli_process, p_rxBuf, p_txBuf);
            CHECKED_FUNC(stat, board_serWriteLine, SER_PORT, p_txBuf);
        }
    }


    // Script stuff.
    if(p_scriptRunning && p_LScript != NULL)
    {
        // Find out where we are in the script sequence.
        int lstat = lua_status(p_LScript);

        switch(lstat)
        {
        case LUA_YIELD:
            // Still running.
            //LOG(tex_TL_DETAIL, "Continue yielding");
            // Continue the script.
            lua_resume(p_LScript, 0, 0);
            break;

        case 0:
            /// It is complete now.
            p_scriptRunning = false;
            common_log(LOG_INFO, "Finished script.");
            break;

        default:
            /// Unexpected error.
            SPX p_processExecError();
            break;
        }
    }
}

//--------------------------------------------------------//
void p_digInputHandler(unsigned int which, bool value)
{
    (void)value;

    // Real simple logic.
    switch(which)
    {
       case DIG_IN_1:
           break;

       case DIG_IN_2:
           break;

       case DIG_IN_3:
           break;

        default:
            break;
    }
}

//--------------------------------------------------------//
status_t p_processCommand(const char* bin)
{
    status_t stat = STATUS_OK;

    QString sin = bin; // String version for most processing.
//    sin = sin.trimmed();

//    //LOG(tex_TL_INFO, QString("ProcessCommand::<%1>").arg(sin));
//    LOG(tex_TL_INFO, QString("ProcessCommand::<%1>%2'").arg(sin.count()).arg(sin.left((130))));

//    QStringList parts = sin.split(" ");
//    if(parts.count() == 0)
//        throw(QString("Invalid number of args"));

//    /// First part is the specific command type. Specific commands will parse the other values.
//    /// See the Tex User Guide for description of commands.
//    QString cmd = parts[0].toLower();
    QString cmd = "???";

//    if(cmd == "reset")
//    {
//        //Reset();
//    }
//    else
    if(cmd == "stop")
    {
        p_stopScript();
    }
//    else if(cmd == "debugger")
//    {
//        debuggerMode = true;
//    }
//    else if(cmd == "dryrun")
//    {
//        dryRun = true;
//    }
//    else if(cmd == "failcnt")
//    {
//        if(parts.count() != 2)
//            throw(QString("Invalid number of args"));
//        failCnt = parts[1].toInt();
//        if(failCnt < 0)
//            throw(QString("Invalid failcnt arg ").arg(failCnt));
//    }
//    else if(cmd == "report")
//    {
//        if(parts.count() != 2)
//            throw(QString("Invalid number of args"));
//        reportFileName = parts[1].replace("\\", "/");
//    }
//    else if(cmd == "loadfile")
//    {
//        if(parts.count() != 2)
//            throw(QString("Invalid number of args"));
//        if(p_scriptRunning)
//            throw(QString("Script already running."));
//        scriptFileName = parts[1];

//        // Load the file contents into our buffer.
//        QString scrfile = parts[1].replace("\\", "/"); // just in case
//        QFile file(scrfile);
//        if(file.open(QIODevice::ReadOnly))
//        {
//            p_scriptText = file.readAll();
//            LOG(tex_TL_INFO, QString("ProcessCommand::loadfile --file--> %1").arg(p_scriptText.left(130)));
//        }
//        else
//        {
//            bout = "NG Couldn't open script file.";
//            bout += file.errorString();
//        }
//    }
    else if(cmd == "run")
    {
//        if(p_scriptRunning)
//            throw(QString("Script already running."));
        SPX p_runScript();
    }
    else if(cmd == "load")//XXXX this is it
    {
//        if(parts.count() != 2)
//            throw(QString("Invalid number of args"));
//        if(p_scriptRunning)
//            throw(QString("Script already running."));
//        // Load the  contents into our buffer.
//        p_scriptText = parts[1];
    }
//    else if(cmd == "getfile")
//    {
//        // getfile <srvrfile> Get a file from the server.
//        // Returns the file contents as: OK 123 b1b2b3...bN where
//        // 123 is the file length and b1 etc are the bytes.

//        if(parts.count() != 2)
//            throw(QString("Invalid number of args"));
//        QString srvrfile = parts[1].replace("\\", "/"); // just in case

//        QFile file(srvrfile);
//        if(file.open(QIODevice::ReadOnly))
//        {
//            QByteArray bf = file.readAll();
//            bout.append(QString("%1 ").arg(bf.length()));
//            bout.append(bf);
//        }
//        else
//        {
//            bout = "NG Couldn't open requested file.";
//            bout += file.errorString();
//        }
//    }
//    else if(cmd == "putfile")
//    {
//        // putfile <srvrfile> <filelen> <filecontents>
//        // Push the contents of a file to the server
//        // e.g.: putfile my_dir/myfile.txt 456 b1b2b3...bN.

//        if(parts.count() < 4)
//            throw(QString("Invalid number of args %1 instead of %2").arg(parts.count()).arg(4));
//        QString srvrfile = parts[1].replace("\\", "/"); // just in case

//        // Get the length as sender knows it.
//        // TODO2 support block send/rcv? or not? http://www.tcpipguide.com/free/t_FTPGeneralDataCommunicationandTransmissionModes.htm
//        //int sendLen = parts[2].toInt();

//        QFile file(srvrfile);
//        if(file.open(QIODevice::WriteOnly))
//        {
//            // Get the rest of the command as a byte array and write it.
//            int start = parts[0].length() + parts[1].length() + parts[2].length() + 3;
//            const char* pdata = bin.constData() + start;
//            if(file.write(pdata, bin.length() - start) == -1)
//            {
//                bout = "NG Couldn't write to requested file." ;
//                bout += file.errorString();
//            }
//        }
//        else
//        {
//            bout = "NG Couldn't open requested file. ";
//            bout += file.errorString();
//        }
//    }
//    else if(cmd == "rcmd")
//    {
//        //  rcmd <cmdline>  Run an arbitrary command on the target.
//        //  Returns stdout as: OK 123 b1b2b3...bN where 123 is the stdout text length and b1 etc are the bytes.

//        LOG(tex_TL_INFO, QString("ProcessCommand::rcmd --> %1").arg(sin));

//        if(parts.count() < 2)
//            throw(QString("Invalid number of args"));

//        QList<QString> args = parts.mid(2);
//        QProcess process;
//        process.setReadChannel(QProcess::StandardOutput);
//        process.start(parts[1], args);

//        // Process the stdout.
//        QByteArray bf = process.readAll();
//        bout.append(QString("%1 ").arg(bf.length()));
//        bout.append(bf);
//    }
//    else // TODO2 stitch in the other commands.
//    {
//        throw(QString("Invalid command. %1").arg(parts.count()));
//    }

    // catch(...)
    // {
    //     /// Reset everything and log the error.
    //     Reset();
    //     status rs = TEX_INVALID_CMD_ARGS_ERR;
    //     LOG_ERR(rs, QString("Unknown command error."));
    //     bout = "NG ";
    //     bout.append(QString("NG Unknown command error."));
    // }

    //emit sendReplyChanged(bout);

    //LOG(tex_TL_INFO, QString("ProcessCommand end, bout --> (%2) %1 ").arg(QString(bout).left(130)).arg(bout.length()));

    return stat;
}

//--------------------------------------------------------//
status_t p_runScript()
{
    status_t stat = STATUS_OK;

    //LOG(tex_TL_INFO, QString("Starting scriptFileName:%1 reportFileName:%2 dryRun:%3 failCnt:%4").arg(scriptFileName).arg(reportFileName).arg(dryRun).arg(failCnt));

    /// Do the real work - run the Lua script.
    int result = 0;

    /// Set up a second Lua thread so we can background execute the script.
    p_LScript = lua_newthread(p_LMain);

    CHECKED_FUNC(stat, xlat_loadLibs, p_LScript);

    /// Load the script we are going to run.
    result = luaL_loadstring(p_LScript, p_scriptText);

    if (result)
    {
        /// If something went wrong, error message is at the top of the stack.
        // LOG_ERR(TEX_SCRIPT_LOAD_ERR, QString("Script load error:%1:%2").arg(result).arg(lua_tostring(p_LScript, -1)));
    }
    else
    {
        /// Pass the context vals to the Lua world in a table named "script_context".
        lua_newtable(p_LScript);

       // lua_pushstring(p_LScript, "report_filename");
       // lua_pushstring(p_LScript, reportFileName);
       // lua_settable(p_LScript, -3);

       // lua_pushstring(p_LScript, "dry_run");
       // lua_pushboolean(p_LScript, dryRun);
       // lua_settable(p_LScript, -3);

       // lua_pushstring(p_LScript, "fail_count");
       // lua_pushinteger(p_LScript, failCnt);
       // lua_settable(p_LScript, -3);

        lua_setglobal(p_LScript, "script_context");

        /// Start the script running.
        int nargs = 0;
        int lstat = lua_resume(p_LScript, 0, nargs);

        switch(lstat)
        {
        case LUA_YIELD:
            /// If it is long running, it will yield and get resumed in the timer callback.
            p_scriptRunning = true;
            // LOG(tex_TL_INFO, "Yielding");
            break;

        case 0:
            /// If it is not long running, it is complete now.
            p_scriptRunning = false;
            common_log(LOG_INFO, "Finished script.");
            break;

        default:
            /// Unexpected error.
            SPX p_processExecError();
            break;
        }
    }

    return stat;
}

//--------------------------------------------------------//
status_t p_stopScript()
{
    status_t status = STATUS_OK;
    p_scriptRunning = false;
    return status;
}

//--------------------------------------------------------//
status_t p_processExecError()
{
    status_t status = STATUS_OK;

    // The Lua error string may be of one of these two forms:
    // tt_dev_test_1.lua:42: blabla
    // do_errors:28: blabla

    p_scriptRunning = false;
    common_log(LOG_ERROR, lua_tostring(p_LScript, -1));

    return status;
}
