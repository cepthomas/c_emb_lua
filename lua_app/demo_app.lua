--[[
Lua scripts for performing unit tests on core C functions.

$URL: http://ocdusrow3rndd3:8686/NEPTUNE_SANDBOX/trunk/RT/dev_tools/test/tex/lua/tex_unit_test_2.lua $
$Revision: 133 $
$Date: 2014-07-08 09:10:59 -0400 (Tue, 08 Jul 2014) $
$Author: cthoma54 $
--]]

local sys = require("system")
local frc = require("force")
local em = require("emul")
local st = require("state")
local ut = require("utils")
local tr = require("test_runner")

function do_system_tests()
  tr.test_set("TaskEx-2", "Unit testing of TEX_SystemFunctions.cc, TEX_StateFunctions.cc, TEX_ForceFunctions.cc and TEX_EmulFunctions.cc.", ut.clean_svn("$URL: http://ocdusrow3rndd3:8686/NEPTUNE_SANDBOX/trunk/RT/dev_tools/test/tex/lua/tex_unit_test_2.lua $"))
      
      
  ----------------------------- case 1 ------------------------------------------------
  tr.test_case("TaskEx-2-1", "Test system functions in TEX_SystemFunctions.cc.")
  
  tr.test_step(1, "Test environment variables access with get_env_var().")
  s = sys.get_env_var("envDATALOG_SW_DEV_FILES_PATH")
  tr.test_check_e(s, s, "/sol/workfiles/datalog/sw_dev/")
  
  s = sys.get_env_var("envVDOCSECONNHOMEURL")
  tr.test_check_e(s, s, "http://148.177.3.14")
  
  -- This step appears here as the ability to collect debug log entries figures prominently in subsequent steps. TODO3 not quite right yet - see TT_StartDlogCache()
  --tr.test_step(2, "Initialize the collection of log messages and add several entries.")
  --sys.set_dlog_flag("TL_SCRIPT_INFO", true)
  --sys.start_dlog_cache("grenade") -- the filter string

  --ut.sleep(500) -- wait until the thread starts TODO4 a better way than this?
  
  -- The log lines.
  --sys.dlog("And Saint Attila raised the hand grenade up on high")
  --sys.dlog("O Lord, bless this Thy hand grenade that with it Thou mayest blow Thine enemies to tiny bits, in Thy mercy")
  --sys.dlog("and the people did feast upon the lambs and sloths and carp and anchovies and orangutans and breakfast cereals.")
  --sys.dlog("First shalt thou take out the Holy Pin, then shalt thou count to three, no more, no less.")
  --sys.dlog("Five is right out.")
  --sys.dlog("Once the number three, being the third number, be reached, then lobbest thou thy Holy Hand Grenade of Antioch.")
  --sys.dlog("Once the number three, being the third number, be reached, then lobbest thou thy Holy Hand grenade of Antioch.")
  --sys.dlog("Arthur then holds up the Holy Hand Grenade and cries out ONE! TWO! FIVE!")
  
  --tr.test_step(2, "Stop the collection and assess the results.")
  --coll = sys.stop_dlog_cache()
  --tr.test_check_e(#coll, 3, "Number collected.")
  --tr.test_check_ne(string.find(coll[1], "And Saint Attila raised"), nil)
  --tr.test_check_ne(string.find(coll[2], "Thou mayest blow Thine enemies to tiny bits"), nil)
  --tr.test_check_ne(string.find(coll[3], "then lobbest thou thy Holy Hand grenade of Antioch"), nil)
  
  tr.test_step(3, "Test emulator logging to debug log.")
  sys.set_dlog_flag("TL_EMUL_LOG", true)
  frc.force_async("CardBufferCover", "ASYNC=CardBufferCover&STATUS=PASS&DATA={ 1 9 0 1 }&")
  tr.test_inspect("Verify that the CardBufferCover emul event appears in the debug log.", true)

  sys.set_dlog_flag("TL_EMUL_LOG", false)
  frc.force_async("CardSupplyLoadDoor", "ASYNC=CardSupplyLoadDoor&STATUS=PASS&DATA={ 2 9 0 1 }&")
  tr.test_inspect("Verify that the CardSupplyLoadDoor emul event does not appear in the debug log.", true)

  sys.set_dlog_flag("TL_EMUL_LOG", true)

  tr.test_step(4, "Test condition code logging to debug log.")
  sys.set_dlog_flag("TL_COND_CODES", true)
  frc.force_error("XXX", "888", "A fake condition code")
  tr.test_inspect("Verify that the XXX-888 condition code appears in the debug log.", true)

  sys.set_dlog_flag("TL_COND_CODES", false)
  frc.force_error("ZZZ", "777", "A fake condition code")
  tr.test_inspect("Verify that the ZZZ-777 condition code does not appear in the debug log.", true)

  sys.set_dlog_flag("TL_COND_CODES", true)
  
  
  ----------------------------- case 2 ------------------------------------------------
  tr.test_case("TaskEx-2-2", "Test get/set machine and device state functions in TEX_StateFunctions.cc.")
  tr.test_step(1, "Test set_machine_state() and get_machine_state().")
  
  -- Set and test some machine states.
  st.set_machine_state("CARD_BUFFER_PURGE_RQRD", true)
  st.set_machine_state("DIAGNOSTICS_PENDING", false)
  st.set_machine_state("TOP_COVER_OPEN", true)
  
  mst = st.get_machine_state("CARD_BUFFER_PURGE_RQRD")
  tr.test_check_e(mst, true, "CARD_BUFFER_PURGE_RQRD")
  mst = st.get_machine_state("DIAGNOSTICS_PENDING")
  tr.test_check_e(mst, false, "DIAGNOSTICS_PENDING")
  mst = st.get_machine_state("TOP_COVER_OPEN")
  tr.test_check_e(mst, true, "TOP_COVER_OPEN")

  -- Flip them and test again.
  st.set_machine_state("CARD_BUFFER_PURGE_RQRD", false)
  st.set_machine_state("DIAGNOSTICS_PENDING", true)
  st.set_machine_state("TOP_COVER_OPEN", false)
  
  mst = st.get_machine_state("CARD_BUFFER_PURGE_RQRD")
  tr.test_check_e(mst, false, "CARD_BUFFER_PURGE_RQRD")
  mst = st.get_machine_state("DIAGNOSTICS_PENDING")
  tr.test_check_e(mst, true, "DIAGNOSTICS_PENDING")
  mst = st.get_machine_state("TOP_COVER_OPEN")
  tr.test_check_e(mst, false, "TOP_COVER_OPEN")


  tr.test_step(2, "Test set_dev_state() and get_dev_state().")
  -- Set and test some device states.
  st.set_dev_state("SAMP_SUP_TRAY_2_DEV", "CMN_INITIALIZING_DEV_STATE_BIT", true)
  st.set_dev_state("CARD_CENT_1_RING_DEV", "CMN_DIAG_DEV_STATE_BIT", false)
  st.set_dev_state("TIP_WASTE_BIN_DEV", "CMN_CFG_DEV_STATE_BIT", true)
  
  dst = st.get_dev_state("SAMP_SUP_TRAY_2_DEV", "CMN_INITIALIZING_DEV_STATE_BIT")
  tr.test_check_e(dst, true, "SAMP_SUP_TRAY_2_DEV")
  dst = st.get_dev_state("CARD_CENT_1_RING_DEV", "CMN_DIAG_DEV_STATE_BIT")
  tr.test_check_e(dst, false, "CARD_CENT_1_RING_DEV")
  dst = st.get_dev_state("TIP_WASTE_BIN_DEV", "CMN_CFG_DEV_STATE_BIT")
  tr.test_check_e(dst, true, "TIP_WASTE_BIN_DEV")

  -- Flip them and test again.
  st.set_dev_state("SAMP_SUP_TRAY_2_DEV", "CMN_INITIALIZING_DEV_STATE_BIT", false)
  st.set_dev_state("CARD_CENT_1_RING_DEV", "CMN_DIAG_DEV_STATE_BIT", true)
  st.set_dev_state("TIP_WASTE_BIN_DEV", "CMN_CFG_DEV_STATE_BIT", false)
  
  dst = st.get_dev_state("SAMP_SUP_TRAY_2_DEV", "CMN_INITIALIZING_DEV_STATE_BIT")
  tr.test_check_e(dst, false, "SAMP_SUP_TRAY_2_DEV")
  dst = st.get_dev_state("CARD_CENT_1_RING_DEV", "CMN_DIAG_DEV_STATE_BIT")
  tr.test_check_e(dst, true, "CARD_CENT_1_RING_DEV")
  dst = st.get_dev_state("TIP_WASTE_BIN_DEV", "CMN_CFG_DEV_STATE_BIT")
  tr.test_check_e(dst, false, "TIP_WASTE_BIN_DEV")
  
  tr.test_step(3, "Test all_devs_ready() and set_all_devs_state().")
  adr = st.all_devs_ready()
  tr.test_check_e(adr, false)
  st.set_all_devs_state("CMN_DEV_STATE_BIT_MASK", true)
  adr = st.all_devs_ready()
  tr.test_check_e(adr, true)
  
  ----------------------------- case 3 ------------------------------------------------
  tr.test_case("TaskEx-2-3", "Test force functions in TEX_ForceFunctions.cc.")
  
  tr.test_step(1, "Test force_pulse().")
  frc.force_pulse("GEN_STATUS_RPT_PULSE_ID", "SubsysEmulChnl", "and some data too")
  tr.test_inspect("Verify that the GEN_STATUS_RPT_PULSE_ID pulse is sent.", true)

  tr.test_step(2, "Test force_notif().")
  frc.force_notif("NF_SHMEM_SAHA_SAMPLE_INFO_PULSE_ID")
  tr.test_inspect("Verify that the NF_SHMEM_SAHA_SAMPLE_INFO_PULSE_ID notification is sent.", true)

  tr.test_step(3, "Test force_async().")
  frc.force_async("SS_ASYNC_SAMP_SUPPLY_PULSE_ID", "ASYNC=SampleTray1&STATUS=PASS&DATA={ 11 7 0 0 }&")
  tr.test_inspect("Verify that the SampleTray1 async is sent.", true)

  tr.test_step(4, "Test force_error().")
  frc.force_error("ABC", "123", "Bad thing happened")
  tr.test_inspect("Verify that the ABC-123 condition code is processed.", true)
  
  tr.test_step(5, "Test force_sample_error().")
  frc.force_sample_error("DEF", "456", 1111, 3, 222, 333, "Bad sample happened")
  tr.test_inspect("Verify that the DEF-456 condition code is processed.", true)
  
  ----------------------------- case 4 ------------------------------------------------
  tr.test_case("TaskEx-2-4", "Test emulator response manipulation functions in TEX_EmulFunctions.cc.") -- TODO3 Needs more, and some from scht_unit_test_1.lua.

  tr.test_step(1, "Initial reset with clear_all_emul_resps().")
  em.clear_all_emul_resps()
  tr.test_inspect("Verify that the emulator responses have been set to defaults.", true)
  
  tr.test_step(2, "Set some emul responses with set_emul_resp().")
  em.set_emul_resp("OP=ReagSupTray2Init&DEV=REAG_SUP_TRAY_2_DEV&CMD=INIT&STATUS=PASS&")
  tr.test_inspect("Verify that the emulator responses appear in ???", true)

  tr.test_step(3, "Clear some emul responses with clear_emul_resp().")
  em.clear_emul_resp("ReagSupTray2Init")
  tr.test_inspect("Verify that the emulator responses are removed from ???", true)

  tr.test_step(4, "Set sequence of emulator responses.")
  em.set_emul_resp("OP=CardIncInvNext&DEV=GRIP_JAW_DEV&CMD=GET_INFO&STATUS=PASS&CARD_PRESENT=TRUE&", 0)
  em.set_emul_resp("OP=CardIncInvNext&DEV=GRIP_JAW_DEV&CMD=GET_INFO&STATUS=PASS&CARD_PRESENT=FALSE&", 1)
  tr.test_inspect("Verify that the emulator first reponse to a CardIncInvNext command is TRUE and second response is FALSE from ???", true)

  tr.test_step(5, "Final reset.")
  em.clear_all_emul_resps()
  tr.test_inspect("Verify that the emulator responses have been set to defaults. ???", true)
		
  ----------------------------- case 5 ------------------------------------------------
  tr.test_case("TaskEx-2-5", "Miscellaneous system functions.")
		
  -- Not technically a part of the source files, but a good place to run this test.		
  tr.test_step(1, "Run a loop with yields.")
		
  for i=1, 10 do
    sys.dlog("loop num:"..i)
    counter = 0
    while counter < 1000 do
      counter = counter + 1
    end
    coroutine.yield()
  end
  tr.test_inspect("Verify that this function's messages are interleaved with C core timer ticks.", true)
		
  ------------------ test complete -----------------------
  -- Don't forget to finish up the test.
  tr.test_end()
  
end


-----------------------------------------------------------------------------
-- Module initialization.

sys.set_dlog_flag("TL_DETAIL", true)
sys.set_dlog_flag("TL_ARGS", true)
sys.set_dlog_flag("TL_RET", true)
-- sys.set_dlog_flag("TL_EMUL_LOG", true)
-- sys.set_dlog_flag("TL_COND_CODES", true)
sys.set_dlog_flag("TL_DEV", true)

do_system_tests()
