#include "stdafx.h"
#include "Emu/ConLog.h"
#include "Emu/Memory/Memory.h"
#include "Emu/System.h"
#include "rpcs3.h"
#include "Ini.h"
#include "Gui/ConLogFrame.h"
#include "Emu/GameInfo.h"

#ifdef _WIN32
#include <wx/msw/wrapwin.h>
#endif

#ifdef __UNIX__
#include <X11/Xlib.h>
#endif

wxDEFINE_EVENT(wxEVT_DBG_COMMAND, wxCommandEvent);

IMPLEMENT_APP(Rpcs3App)
Rpcs3App* TheApp;

bool Rpcs3App::OnInit()
{
	TheApp = this;
	SetAppName(_PRGNAME_);
	wxInitAllImageHandlers();

	Ini.Load();

	m_MainFrame = new MainFrame();
	SetTopWindow(m_MainFrame);
	Emu.Init();

	m_MainFrame->Show();
	m_MainFrame->DoSettings(true);

	OnArguments();

	return true;
}

void Rpcs3App::OnArguments()
{
	// Usage:
	//   rpcs3-*.exe               Initializes RPCS3
	//   rpcs3-*.exe [(S)ELF]      Initializes RPCS3, then loads and runs the specified (S)ELF file.

	if (Rpcs3App::argc > 1)
	{
		// Force this value to be true
		Ini.HLEExitOnStop.SetValue(true);

		Emu.SetPath(fmt::ToUTF8(argv[1]));
		Emu.Load();
		Emu.Run();
	}
}

void Rpcs3App::Exit()
{
	Emu.Stop();
	Ini.Save();

	if(ConLogFrame && !ConLogFrame->IsBeingDeleted()) ConLogFrame->Close();

	wxApp::Exit();
}

void Rpcs3App::SendDbgCommand(DbgCommand id, CPUThread* thr)
{
	wxCommandEvent event(wxEVT_DBG_COMMAND, id);
	event.SetClientData(thr);
	AddPendingEvent(event);
}

Rpcs3App::Rpcs3App()
{
	#if defined(__UNIX__) && !defined(__APPLE__)
	XInitThreads();
	#endif
}
/*
CPUThread& GetCPU(const u8 core)
{
	return Emu.GetCPU().Get(core);
}*/

//TODOB: remove this
//convert a wxString to a std::string encoded in utf8
//CAUTION, only use this to interface with wxWidgets classes
std::string fmt::ToUTF8(const wxString& right)
{
	auto ret = std::string(((const char *)right.utf8_str()));
	return ret;
}

//convert a std::string encoded in utf8 to a wxString
//CAUTION, only use this to interface with wxWidgets classes
wxString fmt::FromUTF8(const std::string& right)
{
	auto ret = wxString::FromUTF8(right.c_str());
	return ret;
}

GameInfo CurGameInfo;
