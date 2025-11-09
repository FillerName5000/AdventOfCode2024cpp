#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

namespace DayFifteenPartTwo
{
	// [0,0]
	//       [xLen-1,yLen-1]
	namespace {
#pragma region charDef
		namespace GridChar {
			constexpr char Wall = '#';
			constexpr char Empty = '.';
			constexpr char SmallBox = 'O';
			constexpr char Bot = '@';
			constexpr char Up = '^';
			constexpr char Right = '>';
			constexpr char Down = 'v';
			constexpr char Left = '<';
			constexpr char LeftBigBox = '[';
			constexpr char RightBigBox = ']';
		}
#pragma endregion
		using coord_t = std::pair<size_t, size_t>;
		using char_square_t = std::vector<std::vector<char>>; // heap, too large for the stack

		enum Direction { Up, Right, Down, Left };
		enum PossibleNextRowState { Blocker, HasMoreBoxes, FullyEmpty };

		void handleLine(std::string line);
		void initializeGrid(const size_t kXLen, const size_t kYLen);
		void coutCharSquareAsGrid(const std::string& prefix);
		void parseStartingStateLine(const std::string& line, const size_t kXLen, const size_t kYLen, size_t& currentRow);
		bool isValidCoord(const size_t kXLen, const size_t kYLen, const coord_t& coord);
		coord_t getNextCoord(const char gridChar, const coord_t& pos, bool isDoubleDistance = false);
		Direction getDirectionFromAToB(const coord_t& a, const coord_t& b);
		char directionToChar(Direction dir);
		void updateBotPos(const coord_t& newPos);
		size_t calcTotal(const size_t kLen);

		const bool toLog = true; // true   false

		const int kMaxIter = 10'000;
		const size_t kYLenTest = 7;
		const size_t kYLenFull = 50;

		char_square_t grid;
		coord_t botPos{ -1, -1 };

#pragma region vert
		void moveRowVertically(const std::vector<coord_t>& rowOfBoxCoords, const Direction boxDir) {
			int offset;
			switch (boxDir) {
			case Direction::Up:
				offset = -1;
				break;
			case Direction::Down:
				offset = 1;
				break;
			default:
				throw std::runtime_error("Invalid vertical direction");
			}

			for (const coord_t& coord : rowOfBoxCoords) {
				char currentChar = grid[coord.first][coord.second];
				grid[coord.first + offset][coord.second] = currentChar;
				grid[coord.first][coord.second] = GridChar::Empty;
			}
		}

		void moveBoxesVertically(const std::vector<std::vector<coord_t>>& rowsToMove, const Direction boxDir, const size_t kXLen, const size_t kYLen) {
			if (boxDir == Direction::Up) {
				for (const auto& row : rowsToMove) {
					moveRowVertically(row, boxDir);
				}
			}
			else if (boxDir == Direction::Down) {
				for (auto it = rowsToMove.rbegin(); it != rowsToMove.rend(); ++it) {
					moveRowVertically(*it, boxDir);
				}
			}
			else {
				throw std::runtime_error("Invalid vertical direction");
			}
		}

		PossibleNextRowState getNextRowState(std::vector<coord_t>& currentRowOfCoords, const size_t kXLen, const size_t kYLen, const Direction dir)
		{ // meant for above & below checks
			bool hasBoxes = false;

			for (const coord_t& coord : currentRowOfCoords) {
				coord_t nextCoord = getNextCoord(directionToChar(dir), coord); // this file is big enough already, no more overloads

				if (!isValidCoord(kXLen, kYLen, nextCoord)) {
					return PossibleNextRowState::Blocker;
				}

				char nextChar = grid[nextCoord.first][nextCoord.second];

				if (nextChar == GridChar::Wall) {
					return PossibleNextRowState::Blocker;
				}

				if (nextChar == GridChar::LeftBigBox || nextChar == GridChar::RightBigBox) {
					hasBoxes = true;
				}
			}

			return hasBoxes ? PossibleNextRowState::HasMoreBoxes : PossibleNextRowState::FullyEmpty;
		}

		std::vector<coord_t> getRowBoxCoords(const coord_t& startPos, const size_t kXLen, const size_t kYLen) {
			std::vector<coord_t> currentRowOfCoords;
			coord_t leftPos = startPos;
			coord_t rightPos = startPos;

			while (isValidCoord(kXLen, kYLen, leftPos)
				&& grid[leftPos.first][leftPos.second] != GridChar::Wall
				&& grid[leftPos.first][leftPos.second] != GridChar::Empty)
			{
				currentRowOfCoords.push_back(leftPos);
				leftPos = getNextCoord(GridChar::Left, leftPos);
			}

			while (isValidCoord(kXLen, kYLen, rightPos)
				&& grid[rightPos.first][rightPos.second] != GridChar::Wall
				&& grid[rightPos.first][rightPos.second] != GridChar::Empty)
			{
				currentRowOfCoords.push_back(rightPos);
				rightPos = getNextCoord(GridChar::Right, rightPos);
			}

			std::sort(currentRowOfCoords.begin(), currentRowOfCoords.end(), [](const coord_t& a, const coord_t& b) {
				return a.second < b.second;
			});

			return currentRowOfCoords;
		}

		void handleVerticalBoxDir(const size_t kXLen, const size_t kYLen, const coord_t& botPos, const Direction boxDir)
		{
			std::vector<std::vector<coord_t>> rowsToMove;
			int iter = 0;
			coord_t nextBotPos = botPos;
			bool exitLoop = false;
			bool canMove = true;

			while (!exitLoop && iter < kMaxIter) {
				++iter;
				nextBotPos = getNextCoord(directionToChar(boxDir), nextBotPos);
				std::vector<coord_t> currentRowOfCoords = getRowBoxCoords(nextBotPos, kXLen, kYLen);
				rowsToMove.push_back(currentRowOfCoords);

				switch (getNextRowState(currentRowOfCoords, kXLen, kYLen, boxDir)) {
				case PossibleNextRowState::HasMoreBoxes:
					// keep going
					break;
				case PossibleNextRowState::Blocker:
					exitLoop = true;
					canMove = false;
					break;
				case PossibleNextRowState::FullyEmpty:
					exitLoop = true;
					canMove = true;
					break;
				default:
					throw std::runtime_error("Invalid enum value");
				}
			}

			if (canMove)
			{
				moveBoxesVertically(rowsToMove, boxDir, kXLen, kYLen);
			}
			// do nothing
		}
#pragma endregion
#pragma region horiz
		void moveBoxesHorizontally(const std::vector<coord_t> bigBoxCoords, const Direction boxDir) {
			if (boxDir == Direction::Left || boxDir == Direction::Right) {
				size_t coordsSize = bigBoxCoords.size();
				size_t underFlow = SIZE_MAX;
				bool isLeftBox = (boxDir == Direction::Left);

				for (size_t i = coordsSize - 1; i > underFlow; --i) {
					const coord_t& currentCoord = bigBoxCoords[i];
					grid[currentCoord.first][currentCoord.second] = isLeftBox ? GridChar::LeftBigBox : GridChar::RightBigBox;
					isLeftBox = !isLeftBox;
				}

				const coord_t& firstCoord = bigBoxCoords.front();
				grid[firstCoord.first][firstCoord.second] = GridChar::Empty;
				updateBotPos(bigBoxCoords.back());
			}
			else {
				throw std::runtime_error("Invalid horizontal direction");
			}
		}

		void handleHorizontalBoxDir(const size_t kXLen, const size_t kYLen, const coord_t firstBoxPos, std::vector<coord_t>& bigBoxCoords, const Direction boxDir)
		{
			int iter = 0;
			bool endCharIsEmpty = false;
			coord_t currentPos = firstBoxPos;
			while (isValidCoord(kXLen, kYLen, currentPos) || iter > kMaxIter) {
				++iter;
				char currentChar = grid[currentPos.first][currentPos.second];
				bigBoxCoords.push_back(currentPos); // inclusive first big box to empty char coord

				if (currentChar == GridChar::Wall || currentChar == GridChar::Empty) {
					endCharIsEmpty = (currentChar == GridChar::Empty);
					break;
				}

				currentPos = getNextCoord(directionToChar(boxDir), currentPos);
			}
			if (endCharIsEmpty) {
				switch (boxDir) {
				case Direction::Left:
				case Direction::Right:
					moveBoxesHorizontally(bigBoxCoords, boxDir);
					break;
				default:
					throw std::runtime_error("Invalid enum value");
				}
			}
			else {
				if (toLog) std::cout << "Wall end found at [" << currentPos.first << "," << currentPos.second << "]\n";
			}
		}
#pragma endregion

		void handleBoxMovement(const coord_t botPos, const coord_t firstBoxPos, const Direction boxDir, const size_t kXLen, const size_t kYLen)
		{
			std::vector<coord_t> bigBoxCoords;
			bigBoxCoords.push_back(firstBoxPos);
			bool endCharIsEmpty = false;

			if (boxDir == Direction::Left || boxDir == Direction::Right)
			{
				handleHorizontalBoxDir(kXLen, kYLen, firstBoxPos, bigBoxCoords, boxDir);
			}
			else {
				handleVerticalBoxDir(kXLen, kYLen, botPos, boxDir);
			}
		}

		void handleDirections(const std::string& directions, const size_t kXLen, const size_t kYLen)
		{
			if (toLog) std::cout << directions << '\n';
			for (const char dir : directions)
			{
				if (toLog) std::cout << dir << '\n';
				coord_t nextBotPos = getNextCoord(dir, botPos);
				if (!isValidCoord(kXLen, kYLen, nextBotPos)) throw std::runtime_error("Bot wants to escape wall containment");

				char nextPosChar = grid[nextBotPos.first][nextBotPos.second];
				switch (nextPosChar) {
				case GridChar::Wall:
					if (toLog) std::cout << "Wall at [" << nextBotPos.first << "," << nextBotPos.second << "]\n";
					continue;
				case GridChar::LeftBigBox:
				case GridChar::RightBigBox:
					handleBoxMovement(botPos, nextBotPos, getDirectionFromAToB(botPos, nextBotPos), kXLen, kYLen);
					continue;
				case GridChar::Empty:
					updateBotPos(nextBotPos);
					continue;
				default:
					throw std::runtime_error("Invalid char in grid");
				}
			}
		}

		void handleFile(std::ifstream& inputFile, const size_t kXLen, const size_t kYLen)
		{
			if (inputFile.is_open()) {
				initializeGrid(kXLen, kYLen); // & botpos init too

				std::string line;
				bool afterBlankLine = false;
				size_t currentRow = 0;

				while (getline(inputFile, line)) {
					if (line.empty()) {
						afterBlankLine = true;
						continue;
					}

					if (!afterBlankLine) {
						parseStartingStateLine(line, kXLen, kYLen, currentRow);
					}
					else {
						if (toLog) coutCharSquareAsGrid("Start: \n");
						handleDirections(line, kXLen, kYLen); // 20 000 line length full
						if (toLog) coutCharSquareAsGrid("End: \n");
					}
				}

				std::cout << "Finished reading file\n";

				inputFile.close(); // automatically happens when going out of scope but no longer needed. More about explicitness.

				//	std::cout << "Total: " << calcTotal(kLen) << '\n';

				std::cout << "\nFinished running program\n";
			}
			else {
				std::cout << "Unable to open file\n";
			}
		}


#pragma region helper
		void updateBotPos(const coord_t& newPos) {
			grid[botPos.first][botPos.second] = GridChar::Empty;
			grid[newPos.first][newPos.second] = GridChar::Bot;
			botPos = newPos;
			if (toLog) coutCharSquareAsGrid("++E: State: \n");
		}
		char directionToChar(Direction dir) {
			switch (dir) {
			case Direction::Up:    return GridChar::Up;
			case Direction::Right: return GridChar::Right;
			case Direction::Down:  return GridChar::Down;
			case Direction::Left:  return GridChar::Left;
			}
			std::cout << dir;
			throw std::runtime_error("Invalid dir passed");
		}
		Direction charToDirection(char dirChar) {
			switch (dirChar) {
			case GridChar::Up:    return Direction::Up;
			case GridChar::Right: return Direction::Right;
			case GridChar::Down:  return Direction::Down;
			case GridChar::Left:  return Direction::Left;
			default:
				throw std::runtime_error("Invalid char passed");
			}
		}
		Direction getDirectionFromAToB(const coord_t& a, const coord_t& b) {
			size_t dx = b.second - a.second;
			size_t dy = b.first - a.first;

			if (dx > dy) {
				return b.second > a.second ? Direction::Right : Direction::Left;
			}
			else {
				return b.first > a.first ? Direction::Down : Direction::Up;
			}
		}
#pragma endregion

#pragma region validHelpers
		bool isValidCoord(const size_t kXLen, const size_t kYLen, const coord_t& coord) {
			return coord.first >= 0 && coord.first < kYLen && coord.second >= 0 && coord.second < kXLen;
		}
		coord_t getNextCoord(const char gridChar, const coord_t& pos, bool isDoubleDistance) {
			coord_t newPos = pos;
			switch (gridChar) {
			case GridChar::Up:
				newPos.first -= (isDoubleDistance ? 2 : 1);
				break;
			case GridChar::Right:
				newPos.second += (isDoubleDistance ? 2 : 1);
				break;
			case GridChar::Down:
				newPos.first += (isDoubleDistance ? 2 : 1);
				break;
			case GridChar::Left:
				newPos.second -= (isDoubleDistance ? 2 : 1);
				break;
			}
			return newPos;
		}
#pragma endregion

#pragma region calcs
		size_t calcTotal(const size_t kLen)
		{
			const size_t gpsMult = 100;
			size_t total = 0;

			/*for (size_t y = 0; y < kLen; ++y) {
				for (size_t x = 0; x < kLen; ++x) {
					if (grid[y][x] == GridChar::Box) {
						if (toLog) std::cout << "Box at [" << y << "," << x << "] adds " << (y * gpsMult) + x << '\n';
						total += (y * gpsMult) + x;
					}
				}
			}*/

			return total;
		}
#pragma endregion

#pragma region cout
		void coutCharSquareAsGrid(const std::string& prefix) {
			std::cout << prefix;
			for (const auto& row : grid) {
				for (const auto& c : row) {
					std::cout << c;
				}
				std::cout << '\n';
			}
			std::cout << '\n';
		}
#pragma endregion

#pragma region startState
		void parseStartingStateLine(const std::string& line, const size_t kXLen, const size_t kYLen, size_t& currentRow)
		{
			if (grid.empty() || currentRow >= grid.size()) {
				return;
			}

			for (size_t inputX = 0; inputX < kYLen; ++inputX) {
				size_t gridX = inputX * 2;
				if (gridX >= kXLen) break;
				switch (line[inputX]) {
				case GridChar::Wall:
					grid[currentRow][gridX] = GridChar::Wall;
					grid[currentRow][gridX + 1] = GridChar::Wall;
					break;
				case GridChar::SmallBox:
					grid[currentRow][gridX] = GridChar::LeftBigBox;
					grid[currentRow][gridX + 1] = GridChar::RightBigBox;
					break;
				case GridChar::Empty:
					grid[currentRow][gridX] = GridChar::Empty;
					grid[currentRow][gridX + 1] = GridChar::Empty;
					break;
				case GridChar::Bot:
					grid[currentRow][gridX] = GridChar::Bot;
					grid[currentRow][gridX + 1] = GridChar::Empty;
					botPos = { currentRow, gridX };
					break;
				default:
					std::cout << "Invalid character '" << line[inputX] << "' at position " << inputX << " in row " << currentRow << '\n';
					// probably goofed the len values
					throw std::runtime_error("Invalid char in input");
				}
			}
			++currentRow;
		}
		void initializeGrid(const size_t kXLen, const size_t kYLen) {
			grid.resize(kYLen, std::vector<char>(kXLen, GridChar::Empty));
		}
#pragma endregion
	}
	void dayFifteenPartTwo() {
		std::system("cls"); // clear terminal pre-boot
		std::cout << "Running program DayFifteenPartTwo" << '\n';
		const bool isFullFile = false; // true   false
		const size_t kYLen = isFullFile ? kYLenFull : kYLenTest;
		const size_t kXLen = kYLen * 2;

		std::string line;
		std::ifstream inputFile;
		if (isFullFile) {
			std::cout << "Using full file\n\n";
		}
		else {
			std::cout << "Using test file\n\n";
		}
		(isFullFile) ? inputFile.open("dayFifteenFull.txt") : inputFile.open("dayFifteenTest.txt");
		handleFile(inputFile, kXLen, kYLen);
	}
}
