#pragma once
#include "Location.h"
#include <fstream>
#include <memory>

struct NumberData
{
	bool Lines[7];
};

struct LetterData
{
	bool Lines[16];
};

struct HighScoreData
{
	std::string Name;
	unsigned int Score;
};

struct HighScoreList
{
	SceneMesh *Spot;
	SceneMesh *Letters;
	SceneMesh *Numbers;
};

struct HighScoreLocation
{
	Vector3 Spot;
	Vector3 Letters;
	Vector3 Numbers;
};

class HUD : public Location
{
public:
	HUD(void);

	bool m_NewHighScore;

	void Setup(std::shared_ptr<Scene> scene);
	void DisplayHighScores(int list, bool on);
	void Add(int score);
	void LostLife(void);
	void NewGame(void);
	void GameOver(void);
	void DisplayGameOver(bool on);
	void NewHighScore(bool on);
	void SelectLetterUp(void);
	void SelectLetterDown(void);
	void SelectNextLetter(void);
	int Lives(void);

private:
	bool m_GameOver;
	bool m_HighScoreUpdated;
	bool m_HighScoreNameEntered;
	int m_HighScoreRank; //The place where the new high score ranks at.
	float m_HighScoreTimer;
	float m_HighScoreTimerAmount;
	unsigned int m_Score;
	unsigned int m_PlayerHighScore;
	unsigned int m_NextNewShipScore;
	int m_Lives;
	int m_HighScoreSelectionOn;

	std::string m_HighScoreSelectedLetters;
	std::string m_GameText[10];
	std::string m_SaveFileName;

	Vector3 m_HitsLocation;
	Vector3 m_GameOverLocation;
	Vector3 m_CopyrightLocation;
	Vector3 m_GameTextLocation[5];
	Vector3 m_PlayerScoreLocation;
	Vector3 m_PlayerHighScoreLocation;
	Vector3 m_HighScoreTitleLocation;
	Vector3 m_SelectionLettersLocation;
	Vector3 m_NewHighScoreTextLocation[2];
	Vector3 m_NumberLineStart[7];
	Vector3 m_NumberLineEnd[7];
	Vector3 m_LetterLineStart[16];
	Vector3 m_LetterLineEnd[16];

	HighScoreData m_HighScores[10];
	HighScoreList m_HighScoreList[10];
	HighScoreLocation m_HighScoreListLocation[10];
	HighScoreData m_NewHighScoreData;
	NumberData Numbers[10];
	LetterData Letters[26];

	std::shared_ptr<Scene> p_Scene;
	std::unique_ptr<Sound> p_NewShipSound;

	SceneMesh *m_ScoreNumbers;
	SceneMesh *m_HighScoreNumbers;
	SceneMesh *m_GameOverLetters;
	SceneMesh *m_SelectionLetters;
	SceneMesh *m_HighScoreTitleLetters;
	SceneMesh *m_NewHighScoreLetters[2];
	SceneMesh *m_GameInstructionLetters[5];
	SceneMesh *m_CopyrightLetters;

	void SaveHighScores(void);
	void LoadHighScores(void);
	void MakeDefaultHighScores(void);
	void SetupTextMeshs(void);
	void SetupNewScoreSelectMesh(void);
	void SetupHighScoreList(void);
	void ProcessNumber(SceneMesh *numbers, int number, Vector3 locationStart, float size);
	void MakeNumbersMesh(SceneMesh *numbers, float locationX, int number, float size);
	void ProcessTextLine(SceneMesh *letters, std::string textLine, Vector3 locationStart, float size);
	void MakeLettersMesh(SceneMesh *letters, float locationX, int letter, float size);
	void InitializeNumberLine(void);
	void InitializeLetterLine(void);
};

