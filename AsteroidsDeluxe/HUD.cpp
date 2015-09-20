#include "HUD.h"

HUD::HUD(void)
{
	m_GameOverLetters = new SceneMesh(Mesh::LINE_MESH);
	m_HighScoreTitleLetters = new SceneMesh(Mesh::LINE_MESH);
	m_CopyrightLetters = new SceneMesh(Mesh::LINE_MESH);
	
	m_GameOverLocation = Vector3(0, m_WindowHeight / 1.666, 0);
	m_PlayerScoreLocation = Vector3(m_WindowWidth / 1.5, m_WindowHeight - 0.25f, 0);
	m_HighScoreTitleLocation = Vector3(0, m_WindowHeight / 2.5, 0);
	m_CopyrightLocation = Vector3(0, -m_WindowHeight / 1.05, 0);
	m_SelectionLettersLocation = 0;

	for (int i = 0; i < 6; i++)
	{
		m_GameInstructionLetters[i] = new SceneMesh(Mesh::LINE_MESH);

		m_GameTextLocation[i] = Vector3(0, -m_WindowHeight / 2 + (i * -2.5), 0);
	}

	for (int i = 0; i < 2; i++)
	{
		m_NewHighScoreLetters[i] = new SceneMesh(Mesh::LINE_MESH);

		m_NewHighScoreTextLocation[i] = Vector3(0, -m_WindowHeight / 1.25 + (i * -2.5), 0);
	}

	for (int i = 0; i < 5; i++)
	{
		m_HighScoreListLocation[i].Spot = Vector3(10, m_WindowHeight / 4 - (3.5 * i), 0);
		m_HighScoreListLocation[i].Letters = Vector3(6, m_WindowHeight / 4 - (3.5 * i), 0);
		m_HighScoreListLocation[i].Numbers = Vector3(-10, m_WindowHeight / 4 - (3.5 * i), 0);
		m_HighScoreListLocation[i + 5].Spot = Vector3(10, m_WindowHeight / 4 - (3.5 * i), 0);
		m_HighScoreListLocation[i + 5].Letters = Vector3(6, m_WindowHeight / 4 - (3.5 * i), 0);
		m_HighScoreListLocation[i + 5].Numbers = Vector3(-10, m_WindowHeight / 4 - (3.5 * i), 0);
	}

	m_GameText[0] = "GAME OVER";
	m_GameText[1] = "HIGH SCORE HEROES";
	m_GameText[2] = "NEW HIGH SCORE";
	m_GameText[3] = "ROTATE TO SELECT LETTER FIRE KEY TO SELECT";
	m_GameText[4] = "ASTEROIDS DELUXE COPYRIGHT ATARI MCMLXXX";
	m_GameText[5] = "N OR ENTER KEY TO START OR RESTART GAME";
	m_GameText[6] = "P KEY TO PAUSE GAME";
	m_GameText[7] = "ARROW KEYS TO CONTROL SHIP UP FOR THRUST";
	m_GameText[8] = "LEFT CTRL KEY OR SPACE TO FIRE";
	m_GameText[9] = "LEFT SHIFT KEY OR LEFT ALT KEY FOR SHIELD";
	m_GameText[10] = "WATCH OUT FOR THE DEATH STAR";

	m_GameOver = true;
	m_NewHighScore = false;
	m_HighScoreUpdated = false;
	m_HighScoreNameEntered = false;

	InitializeNumberLine();
	InitializeLetterLine();

	// High Scores Setup ----------------------------------
	MakeDefaultHighScores();

	m_SaveFileName = "HiScores.bin";

	LoadHighScores();
	// Pass high score from file (or default if no file), to high score display on top.
	m_HighScore = m_HighScores[0].Score;
	m_HighScoreName = m_HighScores[0].Name;
}

void HUD::Setup(std::shared_ptr<Scene> scene)
{
	p_Scene = scene;
	m_Score = 0;
	SetupTextMeshs();
	SetupHighScoreList();
	DisplayHighScores(0, true);
	Add(0);
	DisplayHighScore();

	//Sound -------
	p_NewShipSound = std::unique_ptr<Sound>(new Sound("audio/NewShip.ogg"));
	p_NewShipSound->setPitch(1.25);
	p_NewShipSound->setVolume(0.5);
}

void HUD::DisplayHighScores(int list, bool on)
{
	m_HighScoreTitleLetters->enabled = on;
	m_GameOverLetters->enabled = on;
	m_CopyrightLetters->enabled = on;

	for (int i = 0; i < 5; i++)
	{
		m_HighScoreList[i + 5 - (list * 5)].Letters->enabled = false;
		m_HighScoreList[i + 5 - (list * 5)].Numbers->enabled = false;
		m_HighScoreList[i + 5 - (list * 5)].Spot->enabled = false;

		m_HighScoreList[i + (list * 5)].Letters->enabled = on;
		m_HighScoreList[i + (list * 5)].Numbers->enabled = on;
		m_HighScoreList[i + (list * 5)].Spot->enabled = on;

		m_GameInstructionLetters[i]->enabled = on;
	}
}

void HUD::Add(int score)
{
	m_Score += score;

	if (m_Score > m_NextNewShipScore)
	{
		m_Lives++;
		m_NextNewShipScore += 10000;

		if (p_NewShipSound != NULL)
			p_NewShipSound->Play();
	}

	if (m_Score > m_HighScore)
	{
		m_HighScore = m_Score;
		m_HighScoreName = "   ";
		DisplayHighScore();
	}

	p_Scene->removeEntity(m_ScoreNumbers);
	m_ScoreNumbers = NULL;
	m_ScoreNumbers = new SceneMesh(Mesh::LINE_MESH);


	ProcessNumber(m_ScoreNumbers, m_Score, m_PlayerScoreLocation, 1);
	m_ScoreNumbers->cacheToVertexBuffer(true);
}

void HUD::LostLife(void)
{
	m_Lives--;

	if (m_Lives < 1)
		m_Lives = 0;
}

int HUD::Lives(void)
{
	return m_Lives;
}

void HUD::NewGame(void)
{
	m_Lives = 4;
	m_Score = 0;
	m_NextNewShipScore = 10000;
	m_GameOverLetters->enabled = false;
	DisplayHighScores(0, false);

	for (int i = 0; i < 6; i++)
	{
		m_GameInstructionLetters[i]->enabled = false;
	}
}

void HUD::GameOver(void)
{
	for (int rank = 0; rank < 10; rank++)
	{
		if (m_Score > m_HighScores[rank].Score)
		{
			if (rank < 9)
			{
				// Move High Score at rank list to make room for new High Score.
				HighScoreData oldScores[10];

				for (int oldranks = rank; oldranks < 10; oldranks++)
				{
					oldScores[oldranks].Score = m_HighScores[oldranks].Score;
					oldScores[oldranks].Name = m_HighScores[oldranks].Name;
				}

				for (int oldranks = rank; oldranks < 9; oldranks++)
				{
					m_HighScores[oldranks + 1].Score = oldScores[oldranks].Score;
					m_HighScores[oldranks + 1].Name = oldScores[oldranks].Name;
				}
			}

			m_NewHighScore = true;
			m_HighScoreNameEntered = false;
			m_HighScoreRank = rank;
			m_NewHighScoreData.Score = m_Score;
			m_NewHighScoreData.Name = "";
			m_HighScoreSelectionOn = 0;
			m_HighScoreSelectedLetters = "AAA";

			SetupNewScoreSelectMesh();

			NewHighScore(true);

			break;
		}
	}
}

void HUD::DisplayGameOver(bool on)
{
	m_GameOverLetters->enabled = on;
}

void HUD::NewHighScore(bool on)
{
	for (int i = 0; i < 2; i++)
		m_NewHighScoreLetters[i]->enabled = on;
}

void HUD::SelectLetterUp(void)
{
	(int)m_HighScoreSelectedLetters[m_HighScoreSelectionOn] ++;

	if ((int)m_HighScoreSelectedLetters[m_HighScoreSelectionOn] > 90)
		m_HighScoreSelectedLetters[m_HighScoreSelectionOn] = char(65);

	SetupNewScoreSelectMesh();
}

void HUD::SelectLetterDown(void)
{
	(int)m_HighScoreSelectedLetters[m_HighScoreSelectionOn] --;

	if ((int)m_HighScoreSelectedLetters[m_HighScoreSelectionOn] < 65)
		m_HighScoreSelectedLetters[m_HighScoreSelectionOn] = char(90);

	SetupNewScoreSelectMesh();	
}

void HUD::SelectNextLetter(void)
{
	if (m_HighScoreSelectionOn < 2)
	{		
		m_HighScoreSelectionOn++;

		m_HighScoreSelectedLetters[m_HighScoreSelectionOn] = m_HighScoreSelectedLetters[m_HighScoreSelectionOn - 1];

		SetupNewScoreSelectMesh();
	}
	else
	{
		m_HighScoreNameEntered = true;
		m_NewHighScore = false;
		m_SelectionLetters->enabled = false;
		m_NewHighScoreData.Name = m_HighScoreSelectedLetters;
		m_HighScores[m_HighScoreRank] = m_NewHighScoreData;

		SaveHighScores();
		SetupHighScoreList();
		NewHighScore(false);

		m_GameOverLetters->enabled = true;

		for (int i = 0; i < 4; i++)
		{
			m_GameInstructionLetters[i]->enabled = true;
		}
	}
}

void HUD::SaveHighScores(void)
{
		// Save then close high scores to file.
		std::ofstream file(m_SaveFileName, std::ios::binary);
		file.write((char *)& m_HighScores, sizeof(m_HighScores));
		file.close();
}

void HUD::LoadHighScores(void)
{
	// Test for HiScore.bin file.
	std::ifstream file(m_SaveFileName, std::ios::binary | std::ios::ate);
	int fileSize = file.tellg();
	// If file found, load data.
	if (!file.bad())
	{
		//Debug save file is larger, for unknown reasons. If wrong one is read, game crashes.
#if defined(_DEBUG)
		int reqfileSize = 320;
#else
		int reqfileSize = 280;
#endif

		if (reqfileSize == fileSize)
		{
			file.close(); // Close file.
			std::ifstream file(m_SaveFileName, std::ios::binary); // open again.
			file.read((char *)& m_HighScores, sizeof(m_HighScores)); // Read file.
			file.close(); // Close file.
		}
	}
}

void HUD::MakeDefaultHighScores(void)
{
	// Set Default high scores list
	for (int rank = 0; rank < 10; rank++)
	{
		m_HighScores[rank].Name = "AAA";
		m_HighScores[rank].Score = (rank * -2000) + 20000;
	}
}

void HUD::DisplayHighScore(void)
{
	p_Scene->removeEntity(m_HighScoreNumbers);
	p_Scene->removeEntity(m_HighScoreLetters);
	m_HighScoreNumbers = NULL;
	m_HighScoreNumbers = new SceneMesh(Mesh::LINE_MESH);
	m_HighScoreLetters = NULL;
	m_HighScoreLetters = new SceneMesh(Mesh::LINE_MESH);

	float loc = 0;
	unsigned int number = m_HighScore;

	do
	{
		unsigned int digit = number % 10;
		number /= 10;
		loc++;
	} while (number > 0);

	m_PlayerHighScoreLocation = Vector3(-loc / 2, m_WindowHeight - 0.25f, 0);
	ProcessNumber(m_HighScoreNumbers, m_HighScore, m_PlayerHighScoreLocation, 0.5);
	ProcessTextLine(m_HighScoreLetters, m_HighScoreName, m_PlayerHighScoreLocation + Vector3(-3, 0, 0), 0.25);
	m_HighScoreNumbers->cacheToVertexBuffer(true);
	m_HighScoreLetters->cacheToVertexBuffer(true);
}

void HUD::SetupTextMeshs(void)
{
	ProcessTextLine(m_GameOverLetters, m_GameText[0], m_GameOverLocation, 1);
	ProcessTextLine(m_HighScoreTitleLetters, m_GameText[2], m_HighScoreTitleLocation, 0.5);
	ProcessTextLine(m_CopyrightLetters, m_GameText[4], m_CopyrightLocation, 0.25);
	m_HighScoreTitleLetters->enabled = false;

	for (int i = 0; i < 6; i++)
	{
		ProcessTextLine(m_GameInstructionLetters[i], m_GameText[i + 5], m_GameTextLocation[i], 0.5);
	}

	for (int i = 0; i < 2; i++)
	{
		ProcessTextLine(m_NewHighScoreLetters[i], m_GameText[i + 1], m_NewHighScoreTextLocation[i], 0.5);
		m_NewHighScoreLetters[i]->enabled = false;
	}
}

void HUD::SetupNewScoreSelectMesh(void)
{
	p_Scene->removeEntity(m_SelectionLetters);
	m_SelectionLetters = NULL;
	m_SelectionLetters = new SceneMesh(Mesh::LINE_MESH);

	ProcessTextLine(m_SelectionLetters, m_HighScoreSelectedLetters, m_SelectionLettersLocation, 1);
}

void HUD::SetupHighScoreList(void)
{
	for (int i = 0; i < 10; i++)
	{
		p_Scene->removeEntity(m_HighScoreList[i].Spot);
		p_Scene->removeEntity(m_HighScoreList[i].Letters);
		p_Scene->removeEntity(m_HighScoreList[i].Numbers);
		m_HighScoreList[i].Spot = NULL;
		m_HighScoreList[i].Numbers = NULL;
		m_HighScoreList[i].Letters = NULL;
		m_HighScoreList[i].Letters = new SceneMesh(Mesh::LINE_MESH);
		m_HighScoreList[i].Numbers = new SceneMesh(Mesh::LINE_MESH);
		m_HighScoreList[i].Spot = new SceneMesh(Mesh::LINE_MESH);

		ProcessNumber(m_HighScoreList[i].Spot, i + 1, m_HighScoreListLocation[i].Spot, 1);
		ProcessTextLine(m_HighScoreList[i].Letters, m_HighScores[i].Name, m_HighScoreListLocation[i].Letters, 0.6);
		ProcessNumber(m_HighScoreList[i].Numbers, m_HighScores[i].Score, m_HighScoreListLocation[i].Numbers, 1);
		m_HighScoreList[i].Letters->enabled = false;
		m_HighScoreList[i].Numbers->enabled = false;
		m_HighScoreList[i].Spot->enabled = false;
	}
}

void HUD::ProcessNumber(SceneMesh *numbers, int number, Vector3 locationStart, float size)
{
	int numberIn = number;
	float space = 0;

	do
	{
		//Make digit the modulus of 10 from number.
		int digit = numberIn % 10;
		//This sends a digit to the draw function with the location and size.
		MakeNumbersMesh(numbers, space, digit, size);
		// Dividing the int by 10, we discard the digit that was derived from the modulus operation.
		numberIn /= 10;
		// Move the location for the next digit location to the left. We start on the right hand side with the lowest digit.
		space += size * 2;
	} while (numberIn > 0);

	numbers->setPosition(locationStart);
	numbers->cacheToVertexBuffer(true);
	numbers->lineSmooth = true;
	numbers->setColor(0.7, 0.7, 0.9, 1.0);
	p_Scene->addChild(numbers);
}

void HUD::MakeNumbersMesh(SceneMesh *numbers, float location, int number, float size)
{
	if (number > -1 && number < 10)
	{
		for (int line = 0; line < 7; line++)
		{
			if (Numbers[number].Lines[line])
			{
				Number Xstart = m_NumberLineStart[line].x * size + location;
				Number Ystart = m_NumberLineStart[line].y * size;

				Number Xend = m_NumberLineEnd[line].x * size + location;
				Number Yend = m_NumberLineEnd[line].y * size;

				numbers->getMesh()->addVertex(Xstart, Ystart, 0);
				numbers->getMesh()->addVertex(Xend, Yend, 0);
			}
		}
	}	
}

void HUD::ProcessTextLine(SceneMesh * letters, std::string textLine, Vector3 locationStart, float size)
{
	int textSize = textLine.size();
	float charsize = 2.666;
	float space = float(((-size * charsize) * (textSize - 1)) / 2);

	for (int letter = 0; letter < textSize; letter++)
	{
		if ((int)textLine[letter] > 64 && (int)textLine[letter] < 91)
			MakeLettersMesh(letters, space, (int)textLine[letter] - 65, size);

		space += float(size * charsize);
	}

	letters->setPosition(locationStart);
	letters->cacheToVertexBuffer(true);
	letters->lineSmooth = true;
	letters->setColor(0.8, 0.8, 1.0, 1.0);
	p_Scene->addChild(letters);
}

void HUD::MakeLettersMesh(SceneMesh *letters, float location, int letter, float size)
{
	//Line LetterLine;

	if (letter > -1 && letter < 26)
	{
		for (int line = 0; line < 16; line++)
		{
			if (Letters[letter].Lines[line])
			{
				Number Xstart = m_LetterLineStart[line].x * size - location;
				Number Ystart = m_LetterLineStart[line].y * size;

				Number Xend = m_LetterLineEnd[line].x * size - location;
				Number Yend = m_LetterLineEnd[line].y * size;

				letters->getMesh()->addVertex(Xstart, Ystart, 0);
				letters->getMesh()->addVertex(Xend, Yend, 0);
			}
		}
	}
}

void HUD::InitializeNumberLine(void)
{
	// LED Grid location of line start and end points. 0, 0 is the top left corner.
	//The left of screen, positive X is the direction for rotation zero.
	//The top of the screen, positive Y is the direction for rotation zero.
	m_NumberLineStart[0] = Vector3(0, 0, 0);
	m_NumberLineStart[1] = Vector3(-1, 0, 0);
	m_NumberLineStart[2] = Vector3(-1, -1, 0);
	m_NumberLineStart[3] = Vector3(0, -2, 0);
	m_NumberLineStart[4] = Vector3(0, -1, 0);
	m_NumberLineStart[5] = Vector3(0, 0, 0);
	m_NumberLineStart[6] = Vector3(0, -1, 0);

	m_NumberLineEnd[0] = Vector3(-1, 0, 0);
	m_NumberLineEnd[1] = Vector3(-1, -1, 0);
	m_NumberLineEnd[2] = Vector3(-1, -2, 0);
	m_NumberLineEnd[3] = Vector3(-1, -2, 0);
	m_NumberLineEnd[4] = Vector3(0, -2, 0);
	m_NumberLineEnd[5] = Vector3(0, -1, 0);
	m_NumberLineEnd[6] = Vector3(-1, -1, 0);

	// LED Grid, what lines are on for each number.
	// Line 0 is the top line.
	// Line 1 is upper right side line.
	// Line 2 is lower right side line.
	// Line 3 is bottom line.
	// Line 4 is lower left side line.
	// Line 5 is upper left side line.
	// Line 6 is the middle line.

	Numbers[0].Lines[0] = true;
	Numbers[0].Lines[1] = true;
	Numbers[0].Lines[2] = true;
	Numbers[0].Lines[3] = true;
	Numbers[0].Lines[4] = true;
	Numbers[0].Lines[5] = true;
	Numbers[0].Lines[6] = false;

	Numbers[1].Lines[0] = false;
	Numbers[1].Lines[1] = true;
	Numbers[1].Lines[2] = true;
	Numbers[1].Lines[3] = false;
	Numbers[1].Lines[4] = false;
	Numbers[1].Lines[5] = false;
	Numbers[1].Lines[6] = false;

	Numbers[2].Lines[0] = true;
	Numbers[2].Lines[1] = true;
	Numbers[2].Lines[2] = false;
	Numbers[2].Lines[3] = true;
	Numbers[2].Lines[4] = true;
	Numbers[2].Lines[5] = false;
	Numbers[2].Lines[6] = true;

	Numbers[3].Lines[0] = true;
	Numbers[3].Lines[1] = true;
	Numbers[3].Lines[2] = true;
	Numbers[3].Lines[3] = true;
	Numbers[3].Lines[4] = false;
	Numbers[3].Lines[5] = false;
	Numbers[3].Lines[6] = true;

	Numbers[4].Lines[0] = false;
	Numbers[4].Lines[1] = true;
	Numbers[4].Lines[2] = true;
	Numbers[4].Lines[3] = false;
	Numbers[4].Lines[4] = false;
	Numbers[4].Lines[5] = true;
	Numbers[4].Lines[6] = true;

	Numbers[5].Lines[0] = true;
	Numbers[5].Lines[1] = false;
	Numbers[5].Lines[2] = true;
	Numbers[5].Lines[3] = true;
	Numbers[5].Lines[4] = false;
	Numbers[5].Lines[5] = true;
	Numbers[5].Lines[6] = true;

	Numbers[6].Lines[0] = true;
	Numbers[6].Lines[1] = false;
	Numbers[6].Lines[2] = true;
	Numbers[6].Lines[3] = true;
	Numbers[6].Lines[4] = true;
	Numbers[6].Lines[5] = true;
	Numbers[6].Lines[6] = true;

	Numbers[7].Lines[0] = true;
	Numbers[7].Lines[1] = true;
	Numbers[7].Lines[2] = true;
	Numbers[7].Lines[3] = false;
	Numbers[7].Lines[4] = false;
	Numbers[7].Lines[5] = false;
	Numbers[7].Lines[6] = false;

	Numbers[8].Lines[0] = true;
	Numbers[8].Lines[1] = true;
	Numbers[8].Lines[2] = true;
	Numbers[8].Lines[3] = true;
	Numbers[8].Lines[4] = true;
	Numbers[8].Lines[5] = true;
	Numbers[8].Lines[6] = true;

	Numbers[9].Lines[0] = true;
	Numbers[9].Lines[1] = true;
	Numbers[9].Lines[2] = true;
	Numbers[9].Lines[3] = false;
	Numbers[9].Lines[4] = false;
	Numbers[9].Lines[5] = true;
	Numbers[9].Lines[6] = true;
}

void HUD::InitializeLetterLine(void)
{
	m_LetterLineStart[0] = Vector3(0, 0, 0); //1
	m_LetterLineStart[1] = Vector3(-1, 0, 0); //2
	m_LetterLineStart[2] = Vector3(-2, 0, 0); //3
	m_LetterLineStart[3] = Vector3(-2, -2, 0); //4
	m_LetterLineStart[4] = Vector3(-1, -4, 0); //5
	m_LetterLineStart[5] = Vector3(0, -4, 0); //6
	m_LetterLineStart[6] = Vector3(0, -2, 0); //7
	m_LetterLineStart[7] = Vector3(0, 0, 0); //8
	m_LetterLineStart[8] = Vector3(0, 0, 0); //9
	m_LetterLineStart[9] = Vector3(-2, 0, 0); //10
	m_LetterLineStart[10] = Vector3(-1, -2, 0); //11
	m_LetterLineStart[11] = Vector3(-1, -2, 0); //12
	m_LetterLineStart[12] = Vector3(-1, -2, 0); //13
	m_LetterLineStart[13] = Vector3(0, -2, 0); //14
	m_LetterLineStart[14] = Vector3(-1, 0, 0); //15
	m_LetterLineStart[15] = Vector3(-1, -2, 0); //16

	m_LetterLineEnd[0] = Vector3(-1, 0, 0); //1
	m_LetterLineEnd[1] = Vector3(-2, 0, 0); //2
	m_LetterLineEnd[2] = Vector3(-2, -2, 0); //3
	m_LetterLineEnd[3] = Vector3(-2, -4, 0); //4
	m_LetterLineEnd[4] = Vector3(-2, -4, 0); //5
	m_LetterLineEnd[5] = Vector3(-1, -4, 0); //6
	m_LetterLineEnd[6] = Vector3(0, -4, 0); //7
	m_LetterLineEnd[7] = Vector3(0, -2, 0); //8
	m_LetterLineEnd[8] = Vector3(-1, -2, 0); //9
	m_LetterLineEnd[9] = Vector3(-1, -2, 0); //10
	m_LetterLineEnd[10] = Vector3(-2, -2, 0); //11
	m_LetterLineEnd[11] = Vector3(-2, -4, 0); //12
	m_LetterLineEnd[12] = Vector3(0, -4, 0); //13
	m_LetterLineEnd[13] = Vector3(-1, -2, 0); //14
	m_LetterLineEnd[14] = Vector3(-1, -2, 0); //15
	m_LetterLineEnd[15] = Vector3(-1, -4, 0); //16

	// A
	Letters[0].Lines[0] = true;
	Letters[0].Lines[1] = true;
	Letters[0].Lines[2] = true;
	Letters[0].Lines[3] = true;
	Letters[0].Lines[4] = false;
	Letters[0].Lines[5] = false;
	Letters[0].Lines[6] = true;
	Letters[0].Lines[7] = true;
	Letters[0].Lines[8] = false;
	Letters[0].Lines[9] = false;
	Letters[0].Lines[10] = true;
	Letters[0].Lines[11] = false;
	Letters[0].Lines[12] = false;
	Letters[0].Lines[13] = true;
	Letters[0].Lines[14] = false;
	Letters[0].Lines[15] = false;
	// B
	Letters[1].Lines[0] = true;
	Letters[1].Lines[1] = true;
	Letters[1].Lines[2] = true;
	Letters[1].Lines[3] = true;
	Letters[1].Lines[4] = true;
	Letters[1].Lines[5] = true;
	Letters[1].Lines[6] = false;
	Letters[1].Lines[7] = false;
	Letters[1].Lines[8] = false;
	Letters[1].Lines[9] = false;
	Letters[1].Lines[10] = true;
	Letters[1].Lines[11] = false;
	Letters[1].Lines[12] = false;
	Letters[1].Lines[13] = false;
	Letters[1].Lines[14] = true;
	Letters[1].Lines[15] = true;
	// C
	Letters[2].Lines[0] = true;
	Letters[2].Lines[1] = true;
	Letters[2].Lines[2] = false;
	Letters[2].Lines[3] = false;
	Letters[2].Lines[4] = true;
	Letters[2].Lines[5] = true;
	Letters[2].Lines[6] = true;
	Letters[2].Lines[7] = true;
	Letters[2].Lines[8] = false;
	Letters[2].Lines[9] = false;
	Letters[2].Lines[10] = false;
	Letters[2].Lines[11] = false;
	Letters[2].Lines[12] = false;
	Letters[2].Lines[13] = false;
	Letters[2].Lines[14] = false;
	Letters[2].Lines[15] = false;
	// D
	Letters[3].Lines[0] = true;
	Letters[3].Lines[1] = true;
	Letters[3].Lines[2] = true;
	Letters[3].Lines[3] = true;
	Letters[3].Lines[4] = true;
	Letters[3].Lines[5] = true;
	Letters[3].Lines[6] = false;
	Letters[3].Lines[7] = false;
	Letters[3].Lines[8] = false;
	Letters[3].Lines[9] = false;
	Letters[3].Lines[10] = false;
	Letters[3].Lines[11] = false;
	Letters[3].Lines[12] = false;
	Letters[3].Lines[13] = false;
	Letters[3].Lines[14] = true;
	Letters[3].Lines[15] = true;
	// E
	Letters[4].Lines[0] = true;
	Letters[4].Lines[1] = true;
	Letters[4].Lines[2] = false;
	Letters[4].Lines[3] = false;
	Letters[4].Lines[4] = true;
	Letters[4].Lines[5] = true;
	Letters[4].Lines[6] = true;
	Letters[4].Lines[7] = true;
	Letters[4].Lines[8] = false;
	Letters[4].Lines[9] = false;
	Letters[4].Lines[10] = false;
	Letters[4].Lines[11] = false;
	Letters[4].Lines[12] = false;
	Letters[4].Lines[13] = true;
	Letters[4].Lines[14] = false;
	Letters[4].Lines[15] = false;
	// F
	Letters[5].Lines[0] = true;
	Letters[5].Lines[1] = true;
	Letters[5].Lines[2] = false;
	Letters[5].Lines[3] = false;
	Letters[5].Lines[4] = false;
	Letters[5].Lines[5] = false;
	Letters[5].Lines[6] = true;
	Letters[5].Lines[7] = true;
	Letters[5].Lines[8] = false;
	Letters[5].Lines[9] = false;
	Letters[5].Lines[10] = false;
	Letters[5].Lines[11] = false;
	Letters[5].Lines[12] = false;
	Letters[5].Lines[13] = true;
	Letters[5].Lines[14] = false;
	Letters[5].Lines[15] = false;
	// G
	Letters[6].Lines[0] = true;
	Letters[6].Lines[1] = true;
	Letters[6].Lines[2] = false;
	Letters[6].Lines[3] = true;
	Letters[6].Lines[4] = true;
	Letters[6].Lines[5] = true;
	Letters[6].Lines[6] = true;
	Letters[6].Lines[7] = true;
	Letters[6].Lines[8] = false;
	Letters[6].Lines[9] = false;
	Letters[6].Lines[10] = true;
	Letters[6].Lines[11] = false;
	Letters[6].Lines[12] = false;
	Letters[6].Lines[13] = false;
	Letters[6].Lines[14] = false;
	Letters[6].Lines[15] = false;
	// H
	Letters[7].Lines[0] = false;
	Letters[7].Lines[1] = false;
	Letters[7].Lines[2] = true;
	Letters[7].Lines[3] = true;
	Letters[7].Lines[4] = false;
	Letters[7].Lines[5] = false;
	Letters[7].Lines[6] = true;
	Letters[7].Lines[7] = true;
	Letters[7].Lines[8] = false;
	Letters[7].Lines[9] = false;
	Letters[7].Lines[10] = true;
	Letters[7].Lines[11] = false;
	Letters[7].Lines[12] = false;
	Letters[7].Lines[13] = true;
	Letters[7].Lines[14] = false;
	Letters[7].Lines[15] = false;
	// I
	Letters[8].Lines[0] = true;
	Letters[8].Lines[1] = true;
	Letters[8].Lines[2] = false;
	Letters[8].Lines[3] = false;
	Letters[8].Lines[4] = true;
	Letters[8].Lines[5] = true;
	Letters[8].Lines[6] = false;
	Letters[8].Lines[7] = false;
	Letters[8].Lines[8] = false;
	Letters[8].Lines[9] = false;
	Letters[8].Lines[10] = false;
	Letters[8].Lines[11] = false;
	Letters[8].Lines[12] = false;
	Letters[8].Lines[13] = false;
	Letters[8].Lines[14] = true;
	Letters[8].Lines[15] = true;
	// J
	Letters[9].Lines[0] = false;
	Letters[9].Lines[1] = true;
	Letters[9].Lines[2] = true;
	Letters[9].Lines[3] = true;
	Letters[9].Lines[4] = true;
	Letters[9].Lines[5] = true;
	Letters[9].Lines[6] = true;
	Letters[9].Lines[7] = false;
	Letters[9].Lines[8] = false;
	Letters[9].Lines[9] = false;
	Letters[9].Lines[10] = false;
	Letters[9].Lines[11] = false;
	Letters[9].Lines[12] = false;
	Letters[9].Lines[13] = false;
	Letters[9].Lines[14] = false;
	Letters[9].Lines[15] = false;
	// K
	Letters[10].Lines[0] = false;
	Letters[10].Lines[1] = false;
	Letters[10].Lines[2] = false;
	Letters[10].Lines[3] = false;
	Letters[10].Lines[4] = false;
	Letters[10].Lines[5] = false;
	Letters[10].Lines[6] = true;
	Letters[10].Lines[7] = true;
	Letters[10].Lines[8] = false;
	Letters[10].Lines[9] = true;
	Letters[10].Lines[10] = false;
	Letters[10].Lines[11] = true;
	Letters[10].Lines[12] = false;
	Letters[10].Lines[13] = true;
	Letters[10].Lines[14] = false;
	Letters[10].Lines[15] = false;
	// L
	Letters[11].Lines[0] = false;
	Letters[11].Lines[1] = false;
	Letters[11].Lines[2] = false;
	Letters[11].Lines[3] = false;
	Letters[11].Lines[4] = true;
	Letters[11].Lines[5] = true;
	Letters[11].Lines[6] = true;
	Letters[11].Lines[7] = true;
	Letters[11].Lines[8] = false;
	Letters[11].Lines[9] = false;
	Letters[11].Lines[10] = false;
	Letters[11].Lines[11] = false;
	Letters[11].Lines[12] = false;
	Letters[11].Lines[13] = false;
	Letters[11].Lines[14] = false;
	Letters[11].Lines[15] = false;
	// M
	Letters[12].Lines[0] = false;
	Letters[12].Lines[1] = false;
	Letters[12].Lines[2] = true;
	Letters[12].Lines[3] = true;
	Letters[12].Lines[4] = false;
	Letters[12].Lines[5] = false;
	Letters[12].Lines[6] = true;
	Letters[12].Lines[7] = true;
	Letters[12].Lines[8] = true;
	Letters[12].Lines[9] = true;
	Letters[12].Lines[10] = false;
	Letters[12].Lines[11] = false;
	Letters[12].Lines[12] = false;
	Letters[12].Lines[13] = false;
	Letters[12].Lines[14] = false;
	Letters[12].Lines[15] = false;
	// N
	Letters[13].Lines[0] = false;
	Letters[13].Lines[1] = false;
	Letters[13].Lines[2] = true;
	Letters[13].Lines[3] = true;
	Letters[13].Lines[4] = false;
	Letters[13].Lines[5] = false;
	Letters[13].Lines[6] = true;
	Letters[13].Lines[7] = true;
	Letters[13].Lines[8] = true;
	Letters[13].Lines[9] = false;
	Letters[13].Lines[10] = false;
	Letters[13].Lines[11] = true;
	Letters[13].Lines[12] = false;
	Letters[13].Lines[13] = false;
	Letters[13].Lines[14] = false;
	Letters[13].Lines[15] = false;
	// O
	Letters[14].Lines[0] = true;
	Letters[14].Lines[1] = true;
	Letters[14].Lines[2] = true;
	Letters[14].Lines[3] = true;
	Letters[14].Lines[4] = true;
	Letters[14].Lines[5] = true;
	Letters[14].Lines[6] = true;
	Letters[14].Lines[7] = true;
	Letters[14].Lines[8] = false;
	Letters[14].Lines[9] = false;
	Letters[14].Lines[10] = false;
	Letters[14].Lines[11] = false;
	Letters[14].Lines[12] = false;
	Letters[14].Lines[13] = false;
	Letters[14].Lines[14] = false;
	Letters[14].Lines[15] = false;
	// P
	Letters[15].Lines[0] = true;
	Letters[15].Lines[1] = true;
	Letters[15].Lines[2] = true;
	Letters[15].Lines[3] = false;
	Letters[15].Lines[4] = false;
	Letters[15].Lines[5] = false;
	Letters[15].Lines[6] = true;
	Letters[15].Lines[7] = true;
	Letters[15].Lines[8] = false;
	Letters[15].Lines[9] = false;
	Letters[15].Lines[10] = true;
	Letters[15].Lines[11] = false;
	Letters[15].Lines[12] = false;
	Letters[15].Lines[13] = true;
	Letters[15].Lines[14] = false;
	Letters[15].Lines[15] = false;
	// Q
	Letters[16].Lines[0] = true;
	Letters[16].Lines[1] = true;
	Letters[16].Lines[2] = true;
	Letters[16].Lines[3] = true;
	Letters[16].Lines[4] = true;
	Letters[16].Lines[5] = true;
	Letters[16].Lines[6] = true;
	Letters[16].Lines[7] = true;
	Letters[16].Lines[8] = false;
	Letters[16].Lines[9] = false;
	Letters[16].Lines[10] = false;
	Letters[16].Lines[11] = true;
	Letters[16].Lines[12] = false;
	Letters[16].Lines[13] = false;
	Letters[16].Lines[14] = false;
	Letters[16].Lines[15] = false;
	// R
	Letters[17].Lines[0] = true;
	Letters[17].Lines[1] = true;
	Letters[17].Lines[2] = true;
	Letters[17].Lines[3] = false;
	Letters[17].Lines[4] = false;
	Letters[17].Lines[5] = false;
	Letters[17].Lines[6] = true;
	Letters[17].Lines[7] = true;
	Letters[17].Lines[8] = false;
	Letters[17].Lines[9] = false;
	Letters[17].Lines[10] = true;
	Letters[17].Lines[11] = true;
	Letters[17].Lines[12] = false;
	Letters[17].Lines[13] = true;
	Letters[17].Lines[14] = false;
	Letters[17].Lines[15] = false;
	// S
	Letters[18].Lines[0] = true;
	Letters[18].Lines[1] = true;
	Letters[18].Lines[2] = false;
	Letters[18].Lines[3] = true;
	Letters[18].Lines[4] = true;
	Letters[18].Lines[5] = true;
	Letters[18].Lines[6] = false;
	Letters[18].Lines[7] = true;
	Letters[18].Lines[8] = false;
	Letters[18].Lines[9] = false;
	Letters[18].Lines[10] = true;
	Letters[18].Lines[11] = false;
	Letters[18].Lines[12] = false;
	Letters[18].Lines[13] = true;
	Letters[18].Lines[14] = false;
	Letters[18].Lines[15] = false;
	// T
	Letters[19].Lines[0] = true;
	Letters[19].Lines[1] = true;
	Letters[19].Lines[2] = false;
	Letters[19].Lines[3] = false;
	Letters[19].Lines[4] = false;
	Letters[19].Lines[5] = false;
	Letters[19].Lines[6] = false;
	Letters[19].Lines[7] = false;
	Letters[19].Lines[8] = false;
	Letters[19].Lines[9] = false;
	Letters[19].Lines[10] = false;
	Letters[19].Lines[11] = false;
	Letters[19].Lines[12] = false;
	Letters[19].Lines[13] = false;
	Letters[19].Lines[14] = true;
	Letters[19].Lines[15] = true;
	// U
	Letters[20].Lines[0] = false;
	Letters[20].Lines[1] = false;
	Letters[20].Lines[2] = true;
	Letters[20].Lines[3] = true;
	Letters[20].Lines[4] = true;
	Letters[20].Lines[5] = true;
	Letters[20].Lines[6] = true;
	Letters[20].Lines[7] = true;
	Letters[20].Lines[8] = false;
	Letters[20].Lines[9] = false;
	Letters[20].Lines[10] = false;
	Letters[20].Lines[11] = false;
	Letters[20].Lines[12] = false;
	Letters[20].Lines[13] = false;
	Letters[20].Lines[14] = false;
	Letters[20].Lines[15] = false;
	// V
	Letters[21].Lines[0] = false;
	Letters[21].Lines[1] = false;
	Letters[21].Lines[2] = false;
	Letters[21].Lines[3] = false;
	Letters[21].Lines[4] = false;
	Letters[21].Lines[5] = false;
	Letters[21].Lines[6] = true;
	Letters[21].Lines[7] = true;
	Letters[21].Lines[8] = false;
	Letters[21].Lines[9] = true;
	Letters[21].Lines[10] = false;
	Letters[21].Lines[11] = false;
	Letters[21].Lines[12] = true;
	Letters[21].Lines[13] = false;
	Letters[21].Lines[14] = false;
	Letters[21].Lines[15] = false;
	// W
	Letters[22].Lines[0] = false;
	Letters[22].Lines[1] = false;
	Letters[22].Lines[2] = true;
	Letters[22].Lines[3] = true;
	Letters[22].Lines[4] = false;
	Letters[22].Lines[5] = false;
	Letters[22].Lines[6] = true;
	Letters[22].Lines[7] = true;
	Letters[22].Lines[8] = false;
	Letters[22].Lines[9] = false;
	Letters[22].Lines[10] = false;
	Letters[22].Lines[11] = true;
	Letters[22].Lines[12] = true;
	Letters[22].Lines[13] = false;
	Letters[22].Lines[14] = false;
	Letters[22].Lines[15] = false;
	// X
	Letters[23].Lines[0] = false;
	Letters[23].Lines[1] = false;
	Letters[23].Lines[2] = false;
	Letters[23].Lines[3] = false;
	Letters[23].Lines[4] = false;
	Letters[23].Lines[5] = false;
	Letters[23].Lines[6] = false;
	Letters[23].Lines[7] = false;
	Letters[23].Lines[8] = true;
	Letters[23].Lines[9] = true;
	Letters[23].Lines[10] = false;
	Letters[23].Lines[11] = true;
	Letters[23].Lines[12] = true;
	Letters[23].Lines[13] = false;
	Letters[23].Lines[14] = false;
	Letters[23].Lines[15] = false;
	// Y
	Letters[24].Lines[0] = false;
	Letters[24].Lines[1] = false;
	Letters[24].Lines[2] = false;
	Letters[24].Lines[3] = false;
	Letters[24].Lines[4] = false;
	Letters[24].Lines[5] = false;
	Letters[24].Lines[6] = false;
	Letters[24].Lines[7] = false;
	Letters[24].Lines[8] = true;
	Letters[24].Lines[9] = true;
	Letters[24].Lines[10] = false;
	Letters[24].Lines[11] = false;
	Letters[24].Lines[12] = false;
	Letters[24].Lines[13] = false;
	Letters[24].Lines[14] = false;
	Letters[24].Lines[15] = true;
	// Z
	Letters[25].Lines[0] = true;
	Letters[25].Lines[1] = true;
	Letters[25].Lines[2] = false;
	Letters[25].Lines[3] = false;
	Letters[25].Lines[4] = true;
	Letters[25].Lines[5] = true;
	Letters[25].Lines[6] = false;
	Letters[25].Lines[7] = false;
	Letters[25].Lines[8] = false;
	Letters[25].Lines[9] = true;
	Letters[25].Lines[10] = false;
	Letters[25].Lines[11] = false;
	Letters[25].Lines[12] = true;
	Letters[25].Lines[13] = false;
	Letters[25].Lines[14] = false;
	Letters[25].Lines[15] = false;
}
