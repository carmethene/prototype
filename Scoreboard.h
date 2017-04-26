//------------------------------------------------------------------------------
// File: Scoreboard.h
// Desc: Manages display, entry and storage of high scores
//
// Created: 26 September 2005 12:11:15
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


#ifndef INCLUSIONGUARD_SCOREBOARD_H
#define INCLUSIONGUARD_SCOREBOARD_H


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include <d3dx9.h>
#include "constants.h"


//------------------------------------------------------------------------------
// Prototypes and declarations:
//------------------------------------------------------------------------------
class GScoreboard
{
public:
	static bool				InitialiseOneTimeOnly();
	static bool				ShutdownOneTimeOnly();

	static bool				Initialise();
	static bool				Shutdown();

	static bool				Process();
	static bool				Render();

	static bool				SubmitScore( unsigned int score );

private:
	static bool				ProcessInput();
	static bool				ProcessOneInputChar( unsigned int key, unsigned int output );

	static bool				RenderScores();

	static bool				LoadScores();
	static bool				SaveScores();

	enum SCOREBOARD_STATE
	{
		SCOREBOARD_STATE_DISPLAY,
		SCOREBOARD_STATE_ENTER,
	};
	
	struct SCORE_ENTRY
	{
		unsigned int Score;
		char Name[ PT_GUI_SCORE_MAX_NAME ];
	};

	static SCOREBOARD_STATE	ScoreboardState;

	static SCORE_ENTRY		Scores[ PT_GUI_SCORE_NUM_ENTRIES ];

	static unsigned int		TextMeshes[ 38 ];
	static unsigned int		TitleText;
	static unsigned int		EntryText;
	static unsigned int		ContinueText;

	static unsigned int		LastUpdateTime;

	static unsigned int		SubmittedPosition;
	static unsigned int		TextEntryColumn;
};


#endif //INCLUSIONGUARD_SCOREBOARD_H
